#include "Mesh.h"

Mesh::Mesh()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	stride = sizeof(VertexTextureCoord);
	offset = 0;
}

bool Mesh::InitializeBuffer(ID3D11Device* pDevice)
{
	HRESULT hr;

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexTextureCoord) * m_vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = m_vertices.data();

	hr = pDevice->CreateBuffer(&vertexBufferDesc, &initData, &m_vertexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * m_indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	initData.pSysMem = m_indices.data();

	hr = pDevice->CreateBuffer(&indexBufferDesc, &initData, &m_indexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool Mesh::SetResource(ID3D11Device* pDevice , std::string str)
{
	HRESULT result;
	std::string filepath = str;

	//���� ��ο��� ������ \\�� //���� ������ ������ ���� �̸� ������
	std::string targetStr = "/";
	std::string replaceStr = "\\";

	auto pos = std::find(filepath.begin(), filepath.end(), targetStr[0]);
	if (pos != filepath.end())
	{
		filepath.replace(filepath.find(targetStr), targetStr.size(), replaceStr);
	}

	int strlen = filepath.size() + 1;
	const char* str_utf8 = filepath.c_str();
	wchar_t* textureName = new wchar_t[strlen];
	if (textureName)
	{
		//��Ƽ����Ʈ ���ڷ� ��ȯ
		MultiByteToWideChar(CP_UTF8, 0, str_utf8, -1, textureName, strlen);// -1�̸� �Լ��� ���� null ���ڸ� �����Ͽ� ��ü �Է� ���ڿ��� ó��

		//Diffuse �ؽ�ó ���� ��θ� ���� �ؽ�ó ���ҽ� ����
		result = D3DX11CreateShaderResourceViewFromFile(pDevice, textureName, 0, 0, &m_diffuseTexture, 0);
		if (FAILED(result))
		{
			delete[] textureName;

			m_diffuseTexture = NULL;

			return false;
		}
	}

	delete[] textureName;

	return true;
}


void Mesh::Render(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->PSSetShaderResources(0, 1, &m_diffuseTexture);
	pDeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->DrawIndexed(m_indices.size(), 0, 0);

	return;
}

void Mesh::Shutdown()
{
	if (m_diffuseTexture)
	{
		m_diffuseTexture->Release();
		m_diffuseTexture = nullptr;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	return;
}
