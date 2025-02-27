#include "FbxLoader.h"

FbxLoader::FbxLoader()
{
	m_filepath = 0;
	m_previousTexturePath = "";
	m_worldMatrix = DirectX::XMMatrixIdentity();
}

FbxLoader::FbxLoader(const FbxLoader& other)
{
}

FbxLoader::~FbxLoader()
{
}

bool FbxLoader::LoadFile(ID3D11Device* pDevice, HWND hwnd)
{
	bool result;

	//���� ��θ� �ޱ� ���� ��ȭ ���� ����
	OpenFileDialog(hwnd);

	if (m_filepath == "")
	{
		MessageBox(hwnd, L"Null filePath", L"Error", MB_OK);

		return false;
	}

	FbxIOSettings* pIos;
	FbxImporter* pImporter;
	FbxScene* pScene;

	//FBX �Ŵ��� ����
	FbxManager* pManager = FbxManager::Create();

	//���� IO ����� ����(IOSROOT �⺻ ����)
	pIos = FbxIOSettings::Create(pManager, IOSROOT);

	//���� ����
	pManager->SetIOSettings(pIos);

	//FileName�� �ι�° �Ű������� �־� �� �̸��� �����ϴµ� ���� ������ ����
	//Scene ����
	pScene = FbxScene::Create(pManager, "");
	
	//�ҷ��� �����Ϳ� ���� �� �� �ִ� �������̽��� ����
	pImporter = FbxImporter::Create(pManager, "");

	//������ ����� �ε尡 �ƴ��� Ȯ��
	result = pImporter->Initialize(m_filepath, -1, pManager->GetIOSettings());
	if (!result)
	{
		std::string str = pImporter->GetStatus().GetErrorString();
		std::wstring w_trans = std::wstring(str.begin(), str.end());
		MessageBox(hwnd, w_trans.c_str(), L"Error", MB_OK);

		return false;
	}

	//�ε�� ������ �����͸� Scene�� ���
	pImporter->Import(pScene);
	
	//���� ������ ���°� �簢������ �Ǿ� �ֱ⵵ �Ͽ� �ϴ� �ﰢ�� ���������� ��ȯ��
	FbxGeometryConverter converter(pManager);
	converter.Triangulate(pScene, true);//true�� ���� ������ ���� 

	//�� ������ ��� Ž��
	processNode(pScene->GetRootNode(), pDevice);

	//�޽����� ������ �ε��� ���۸� ����
	for (Mesh& m : m_meshes)
	{
		result = m.InitializeBuffer(pDevice);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize Vertex Buffer", L"Error", MB_OK);
			return false;
		}
	}

	//Importer ����
	pImporter->Destroy();
	pImporter = nullptr;

	return true;
}

void FbxLoader::processNode(FbxNode* pNode, ID3D11Device* pDevice)
{
	//����� �Ӽ��� ������
	FbxNodeAttribute* nodeAttribute = pNode->GetNodeAttribute();

	if (nodeAttribute != nullptr)
	{
		//��� Ÿ���� �޽��� ���̷����� ��쿡�� �۾�
		switch (nodeAttribute->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			processMesh(pNode, pDevice);
			break;
		case FbxNodeAttribute::eSkeleton:
			break;
		}
	}

	//�ڽ� ��尡 �ִ��� Ȯ���ϸ� Ž��
	if (pNode->GetChildCount() > 0)
	{
		for (int i = 0; i < pNode->GetChildCount(); i++)
		{
			processNode(pNode->GetChild(i), pDevice);
		}
	}
	
	return;
}

void FbxLoader::processMesh(FbxNode* pNode, ID3D11Device* pDevice)
{
	FbxMesh* pMesh = pNode->GetMesh();
	Mesh lMesh;

	if (pMesh->IsTriangleMesh())
	{
		//�޽��� ��� ���� ������ �޾ƿ� 
		FbxVector4* positions = pMesh->GetControlPoints();

		//UVSet �̸����� �ҷ���
		FbxStringList uvNames;
		pMesh->GetUVSetNames(uvNames);
		
		//��� �ؽ�ó ��ǥ�� �迭�� ����
		int polygonCount = pMesh->GetPolygonCount();
		for (int j = 0; j < polygonCount; j++)
		{
			for (int i = 0; i < 3; i++)//�ﰢ�� ������ ���� ���� �׸��� ������ ���� �����͸� �迭�� �о�ִ´�.
			{
				bool unmappedUV;//���� ���� (����*) false�� ���Ϲް� �Ǹ� ������ �Ǿ������� �ǹ�
				VertexTextureCoord vt;

				//���� �׸��� ������ �޾ƿ�
				UINT index = pMesh->GetPolygonVertex(j, i);

				//���� ��ǥ�� ����
				vt.position = DirectX::XMFLOAT3(
					static_cast<float>(positions[index].mData[0]),
					static_cast<float>(positions[index].mData[1]),
					static_cast<float>(positions[index].mData[2]));
				
				//�ؽ�ó ��ǥ�� ����
				FbxVector2 fv2;
				pMesh->GetPolygonVertexUV(j, i, uvNames[0], fv2, unmappedUV);
				vt.textureCoord = DirectX::XMFLOAT2(
					static_cast<float>(fv2.mData[0]),
					1.0f - static_cast<float>(fv2.mData[1])
				);
				
				//�迭�� �߰�
				lMesh.m_vertices.emplace_back(vt);
			}
		}

		//�ε��� ������� ����
		int vertexCount = pMesh->GetPolygonVertexCount();
		for (int i = 0; i < vertexCount; i++)
		{
			lMesh.m_indices.emplace_back(i);
		}

		//�ؽ�ó�� �ִ� ��� ����
		int materialCount = pNode->GetMaterialCount();
		for (int index = 0; index < materialCount; index++)
		{
			FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(index);
			if (!pMaterial)
			{
				break;
			}
			
			bool result;
			result = GetTextureFromMaterial(pMaterial, pDevice, lMesh);
			if (result)
			{
				break;
			}
		}

		m_meshes.emplace_back(lMesh);
		pMesh->Destroy();
	}

	return;
}

//Material�� ���� �ؽ�ó ���� ��θ� �˾Ƴ��� �ش� ��θ� ���� �ؽ�ó�� �ε��ϴ� �Լ�
bool FbxLoader::GetTextureFromMaterial(FbxSurfaceMaterial* pMaterial, ID3D11Device* pDevice, Mesh& mesh)
{
	bool result;

	//Diffuse �Ӽ��� ������ 
	FbxProperty prop = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	if (!prop.IsValid()) 
	{
		return false;
	}

	//�ؽ�ó ���� Ȯ��
	int textureCount = prop.GetSrcObjectCount<FbxFileTexture>();
	if (textureCount == 0)
	{
		//Diffuse ������ 0�ε� Normal�� ���������� ������ ��찡 ���� ���� �׷��� NormalMap �� ���� ��� ���� �޽��� �ؽ�ó�� ������ �����ϰ� �Ͽ���
		FbxProperty temp = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
		if (temp.IsValid())
		{
			int tempCount = temp.GetSrcObjectCount<FbxFileTexture>();
			if (tempCount > 0)
			{
				result = mesh.SetResource(pDevice, m_previousTexturePath);//�ؽ�ó ��θ� �����Ͽ� ���̴� ���ҽ��� ����
				if (!result)
				{
					return false;
				}
			}

			return true;
		}
		
		return false;
	}

	//�ؽ�ó ���� ������ ������
	FbxString filepath;
	std::string tmpstr;
	FbxFileTexture* pTexture = prop.GetSrcObject<FbxFileTexture>();
	if (!pTexture) 
	{
		return false;
	}
	
	//���� ��θ� ����
	filepath = pTexture->GetRelativeFileName(); //GetFileName()�Լ��� ������ \\�� /�� �����Ǵ� ������ ����

	//���� ���� ��ġ�� �ؽ�ó ��ġ ���ڿ��� ��ħ
	tmpstr = m_filepath;
	tmpstr = tmpstr.substr(0, tmpstr.find_last_of("\\"));
	tmpstr.append("\\");
	tmpstr.append(filepath.Buffer());

	result = mesh.SetResource(pDevice, filepath.Buffer());//�ؽ�ó ��θ� �����Ͽ� ���̴� ���ҽ��� ����
	if (!result)
	{
		return false;
	}

	//���� �޽��� �ؽ�ó ��θ� �����ϰ� FileTexture ����
	m_previousTexturePath = filepath.Buffer();
	pTexture->Destroy();

	return true;
}

void FbxLoader::RotateLeftModel() 
{
	m_worldMatrix = m_worldMatrix * DirectX::XMMatrixRotationY(0.1f);
}

void FbxLoader::RotateRightModel() 
{
	m_worldMatrix = m_worldMatrix * DirectX::XMMatrixRotationY(-0.1f);
}

void FbxLoader::ZoomInModel() 
{
	m_worldMatrix = m_worldMatrix * DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f);
}

void FbxLoader::ZoomOutModel() 
{
	m_worldMatrix = m_worldMatrix * DirectX::XMMatrixScaling(0.9f, 0.9f, 0.9f);
}

DirectX::XMMATRIX FbxLoader::GetWorldMatrix()
{
	return m_worldMatrix;
}

void FbxLoader::OpenFileDialog(HWND hwnd)
{
	OPENFILENAMEA  ofn;       // ���� ��ȭ ���� ����ü

	if (m_filepath)
	{
		delete[] m_filepath;
		m_filepath = nullptr;
	}

	m_filepath = new char[MAX_PATH];//���ϸ��� ��ü ��θ� �����Ͽ� �ִ� 259 ����

	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));//����ü �ʱ�ȭ
	ofn.lStructSize = sizeof(OPENFILENAMEA);//����ü ũ�� ����
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = m_filepath; //���� ��θ� ������ ����
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";//���� ���� ����
	ofn.nFilterIndex = 1; //���� �ε���
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;//��ȭ ���� �÷���

	// ���� ���� ��ȭ ���� ǥ��
	GetOpenFileNameA(&ofn);//char������ ���� ��θ� �ޱ� ���� wchar�� GetOpenFileNameW��

	return;
}

void FbxLoader::Render(ID3D11DeviceContext* pDeviceContext)
{
	//���� �޽� �ϳ��� ������ ��
	for (Mesh& m : m_meshes)
	{
		m.Render(pDeviceContext);
	}

	return;
}

void FbxLoader::Shutdown()
{
	if (m_filepath)
	{
		delete[] m_filepath;
		m_filepath = nullptr;
	}

	//����ü �迭���� �����ʹ� ���� ����
	for (Mesh& m : m_meshes)
	{
		m.Shutdown();
	}

	m_meshes.clear();
}

