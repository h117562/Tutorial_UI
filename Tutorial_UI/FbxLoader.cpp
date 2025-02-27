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

	//파일 경로를 받기 위해 대화 상자 생성
	OpenFileDialog(hwnd);

	if (m_filepath == "")
	{
		MessageBox(hwnd, L"Null filePath", L"Error", MB_OK);

		return false;
	}

	FbxIOSettings* pIos;
	FbxImporter* pImporter;
	FbxScene* pScene;

	//FBX 매니저 생성
	FbxManager* pManager = FbxManager::Create();

	//파일 IO 입출력 설정(IOSROOT 기본 설정)
	pIos = FbxIOSettings::Create(pManager, IOSROOT);

	//설정 적용
	pManager->SetIOSettings(pIos);

	//FileName을 두번째 매개변수에 넣어 씬 이름을 저장하는데 딱히 쓸데는 없다
	//Scene 생성
	pScene = FbxScene::Create(pManager, "");
	
	//불러온 데이터에 접근 할 수 있는 인터페이스를 제공
	pImporter = FbxImporter::Create(pManager, "");

	//파일이 제대로 로드가 됐는지 확인
	result = pImporter->Initialize(m_filepath, -1, pManager->GetIOSettings());
	if (!result)
	{
		std::string str = pImporter->GetStatus().GetErrorString();
		std::wstring w_trans = std::wstring(str.begin(), str.end());
		MessageBox(hwnd, w_trans.c_str(), L"Error", MB_OK);

		return false;
	}

	//로드된 파일의 데이터를 Scene에 담기
	pImporter->Import(pScene);
	
	//보통 폴리곤 형태가 사각형으로 되어 있기도 하여 일단 삼각형 폴리곤으로 변환함
	FbxGeometryConverter converter(pManager);
	converter.Triangulate(pScene, true);//true시 원본 데이터 유지 

	//모델 파일의 노드 탐색
	processNode(pScene->GetRootNode(), pDevice);

	//메쉬마다 정점과 인덱스 버퍼를 생성
	for (Mesh& m : m_meshes)
	{
		result = m.InitializeBuffer(pDevice);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize Vertex Buffer", L"Error", MB_OK);
			return false;
		}
	}

	//Importer 해제
	pImporter->Destroy();
	pImporter = nullptr;

	return true;
}

void FbxLoader::processNode(FbxNode* pNode, ID3D11Device* pDevice)
{
	//노드의 속성을 가져옴
	FbxNodeAttribute* nodeAttribute = pNode->GetNodeAttribute();

	if (nodeAttribute != nullptr)
	{
		//노드 타입이 메쉬나 스켈레톤일 경우에만 작업
		switch (nodeAttribute->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			processMesh(pNode, pDevice);
			break;
		case FbxNodeAttribute::eSkeleton:
			break;
		}
	}

	//자식 노드가 있는지 확인하며 탐색
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
		//메쉬내 모든 정점 정보를 받아옴 
		FbxVector4* positions = pMesh->GetControlPoints();

		//UVSet 이름들을 불러옴
		FbxStringList uvNames;
		pMesh->GetUVSetNames(uvNames);
		
		//모든 텍스처 좌표를 배열에 저장
		int polygonCount = pMesh->GetPolygonCount();
		for (int j = 0; j < polygonCount; j++)
		{
			for (int i = 0; i < 3; i++)//삼각형 폴리곤 기준 정점 그리는 순서에 따라 데이터를 배열에 밀어넣는다.
			{
				bool unmappedUV;//매핑 여부 (주의*) false를 리턴받게 되면 매핑이 되어있음을 의미
				VertexTextureCoord vt;

				//정점 그리는 순서를 받아옴
				UINT index = pMesh->GetPolygonVertex(j, i);

				//정점 좌표를 저장
				vt.position = DirectX::XMFLOAT3(
					static_cast<float>(positions[index].mData[0]),
					static_cast<float>(positions[index].mData[1]),
					static_cast<float>(positions[index].mData[2]));
				
				//텍스처 좌표를 저장
				FbxVector2 fv2;
				pMesh->GetPolygonVertexUV(j, i, uvNames[0], fv2, unmappedUV);
				vt.textureCoord = DirectX::XMFLOAT2(
					static_cast<float>(fv2.mData[0]),
					1.0f - static_cast<float>(fv2.mData[1])
				);
				
				//배열에 추가
				lMesh.m_vertices.emplace_back(vt);
			}
		}

		//인덱스 순서대로 나열
		int vertexCount = pMesh->GetPolygonVertexCount();
		for (int i = 0; i < vertexCount; i++)
		{
			lMesh.m_indices.emplace_back(i);
		}

		//텍스처가 있는 경우 저장
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

//Material을 통해 텍스처 파일 경로를 알아내고 해당 경로를 통해 텍스처를 로드하는 함수
bool FbxLoader::GetTextureFromMaterial(FbxSurfaceMaterial* pMaterial, ID3D11Device* pDevice, Mesh& mesh)
{
	bool result;

	//Diffuse 속성을 가져옴 
	FbxProperty prop = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	if (!prop.IsValid()) 
	{
		return false;
	}

	//텍스처 보유 확인
	int textureCount = prop.GetSrcObjectCount<FbxFileTexture>();
	if (textureCount == 0)
	{
		//Diffuse 개수는 0인데 Normal은 정상적으로 나오는 경우가 가끔 있음 그래서 NormalMap 이 있을 경우 이전 메쉬의 텍스처를 강제로 적용하게 하였음
		FbxProperty temp = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
		if (temp.IsValid())
		{
			int tempCount = temp.GetSrcObjectCount<FbxFileTexture>();
			if (tempCount > 0)
			{
				result = mesh.SetResource(pDevice, m_previousTexturePath);//텍스처 경로를 전달하여 쉐이더 리소스로 저장
				if (!result)
				{
					return false;
				}
			}

			return true;
		}
		
		return false;
	}

	//텍스처 파일 정보를 가져옴
	FbxString filepath;
	std::string tmpstr;
	FbxFileTexture* pTexture = prop.GetSrcObject<FbxFileTexture>();
	if (!pTexture) 
	{
		return false;
	}
	
	//파일 경로를 얻음
	filepath = pTexture->GetRelativeFileName(); //GetFileName()함수는 마지막 \\가 /로 변형되는 오류가 있음

	//원본 파일 위치와 텍스처 위치 문자열을 합침
	tmpstr = m_filepath;
	tmpstr = tmpstr.substr(0, tmpstr.find_last_of("\\"));
	tmpstr.append("\\");
	tmpstr.append(filepath.Buffer());

	result = mesh.SetResource(pDevice, filepath.Buffer());//텍스처 경로를 전달하여 쉐이더 리소스로 저장
	if (!result)
	{
		return false;
	}

	//현재 메쉬의 텍스처 경로를 저장하고 FileTexture 해제
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
	OPENFILENAMEA  ofn;       // 공통 대화 상자 구조체

	if (m_filepath)
	{
		delete[] m_filepath;
		m_filepath = nullptr;
	}

	m_filepath = new char[MAX_PATH];//파일명은 전체 경로를 포함하여 최대 259 글자

	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));//구조체 초기화
	ofn.lStructSize = sizeof(OPENFILENAMEA);//구조체 크기 설정
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = m_filepath; //파일 경로를 저장할 버퍼
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";//파일 필터 설정
	ofn.nFilterIndex = 1; //필터 인덱스
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;//대화 상자 플래그

	// 파일 열기 대화 상자 표시
	GetOpenFileNameA(&ofn);//char형으로 파일 경로를 받기 위함 wchar는 GetOpenFileNameW로

	return;
}

void FbxLoader::Render(ID3D11DeviceContext* pDeviceContext)
{
	//모델의 메쉬 하나씩 렌더링 함
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

	//구조체 배열안의 포인터는 직접 해제
	for (Mesh& m : m_meshes)
	{
		m.Shutdown();
	}

	m_meshes.clear();
}

