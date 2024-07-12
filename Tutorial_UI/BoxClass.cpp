#include "BoxClass.h"

BoxClass::BoxClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_verticesTex = 0;
	m_verticesColor = 0;
	m_indices = 0;

	m_normalTexture = NULL;
	
}

BoxClass::BoxClass(const BoxClass& other)
{

}

BoxClass::~BoxClass()
{

}

//�ؽ�ó�� �ִ� �ڽ��� ������ �� ����ϴ� �Լ�
bool BoxClass::Initialize(ID3D11Device* pDevice, const WCHAR* filepath, float posX, float posY, float buttonWidth, float buttonHeight)
{
	HRESULT result;

	m_verticesTex = new VertexTextureCoord[6];
	m_indices = new UINT[6];

	//�簢�� ��ư�� ������ �ʱ�ȭ�ϰ� �ؽ�ó ��ǥ�� �̿� ���缭 ����
	m_verticesTex[0].position = XMFLOAT3((posX + buttonWidth / 2), (posY + buttonHeight / 2), 0.0f);
	m_verticesTex[1].position = XMFLOAT3((posX - buttonWidth / 2), (posY - buttonHeight / 2), 0.0f);
	m_verticesTex[2].position = XMFLOAT3((posX - buttonWidth / 2), (posY + buttonHeight / 2), 0.0f);

	m_verticesTex[3].position = XMFLOAT3((posX - buttonWidth / 2), (posY - buttonHeight / 2), 0.0f);
	m_verticesTex[4].position = XMFLOAT3((posX + buttonWidth / 2), (posY + buttonHeight / 2), 0.0f);
	m_verticesTex[5].position = XMFLOAT3((posX + buttonWidth / 2), (posY - buttonHeight / 2), 0.0f);

	m_verticesTex[0].textureCoord = XMFLOAT2(1.0f, 0.0f); // ������ ���
	m_verticesTex[1].textureCoord = XMFLOAT2(0.0f, 1.0f); // ���� �ϴ�
	m_verticesTex[2].textureCoord = XMFLOAT2(0.0f, 0.0f); // ���� ���

	m_verticesTex[3].textureCoord = XMFLOAT2(0.0f, 1.0f); // ���� �ϴ�
	m_verticesTex[4].textureCoord = XMFLOAT2(1.0f, 0.0f); // ������ ���
	m_verticesTex[5].textureCoord = XMFLOAT2(1.0f, 1.0f); // ������ �ϴ�

	//�ε��� �ʱ�ȭ
	int indexCount = 6;
	for (int i = 0; i < indexCount; i++)
	{
		m_indices[i] = i;
	}

	//�ؽ�ó ��θ� ���� �̹����� ���̴� ���ҽ� ��� ��ȯ
	result = D3DX11CreateShaderResourceViewFromFile(pDevice, filepath, 0, 0, &m_normalTexture, 0);
	if (FAILED(result))
	{
		m_normalTexture = NULL;
	}

	//���� ����
	result = InitializeBuffers(pDevice);
	if (FAILED(result))
	{
		return false;
	}

	m_stride = sizeof(VertexTextureCoord);
	m_offset = 0;

	return true;
}

//�ؽ�ó ���� �ܻ����� �������ϱ� ���� �Լ�
bool BoxClass::Initialize(ID3D11Device* pDevice, XMFLOAT4 color, float posX, float posY, float buttonWidth, float buttonHeight)
{
	HRESULT result;

	m_verticesColor = new VertexColor[6];
	m_indices = new UINT[6];

	//�簢�� ��ư�� ������ �ʱ�ȭ�ϰ� �ؽ�ó ��ǥ�� �̿� ���缭 ����
	m_verticesColor[0].position = XMFLOAT3((posX + buttonWidth / 2), (posY + buttonHeight / 2), 0.0f);
	m_verticesColor[1].position = XMFLOAT3((posX - buttonWidth / 2), (posY - buttonHeight / 2), 0.0f);
	m_verticesColor[2].position = XMFLOAT3((posX - buttonWidth / 2), (posY + buttonHeight / 2), 0.0f);

	m_verticesColor[3].position = XMFLOAT3((posX - buttonWidth / 2), (posY - buttonHeight / 2), 0.0f);
	m_verticesColor[4].position = XMFLOAT3((posX + buttonWidth / 2), (posY + buttonHeight / 2), 0.0f);
	m_verticesColor[5].position = XMFLOAT3((posX + buttonWidth / 2), (posY - buttonHeight / 2), 0.0f);

	m_verticesColor[0].color = color;
	m_verticesColor[1].color = color; 
	m_verticesColor[2].color = color; 

	m_verticesColor[3].color = color;
	m_verticesColor[4].color = color;
	m_verticesColor[5].color = color;

	//�ε��� �ʱ�ȭ
	int indexCount = 6;
	for (int i = 0; i < indexCount; i++)
	{
		m_indices[i] = i;
	}

	//���� ����
	result = InitializeBuffers(pDevice);
	if (FAILED(result))
	{
		return false;
	}

	m_stride = sizeof(VertexColor);
	m_offset = 0;

	return true;
}


HRESULT BoxClass::InitializeBuffers(ID3D11Device* pDevice)
{
	HRESULT result;
	D3D11_SUBRESOURCE_DATA initData;
	UINT byteSize = 0;

	if (m_verticesColor)
	{
		byteSize = sizeof(VertexColor) * m_indexCount;
		initData.pSysMem = m_verticesColor;
	}
	else if (m_verticesTex)
	{
		byteSize = sizeof(VertexTextureCoord) * m_indexCount;
		initData.pSysMem = m_verticesTex;
	}
	else
	{
		return E_FAIL;
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = byteSize;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	result = pDevice->CreateBuffer(&vertexBufferDesc, &initData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return result;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	initData.pSysMem = m_indices;

	result = pDevice->CreateBuffer(&indexBufferDesc, &initData, &m_indexBuffer);
	if (FAILED(result))
	{
		return result;
	}

	return S_OK;
}

void BoxClass::Render(ID3D11DeviceContext* pDeviceContext)
{

	//���� �ؽ�ó�� ���� ��쿡 �ȼ� ���̴��� �ؽ�ó ����
	if (m_normalTexture)
	{
		pDeviceContext->PSSetShaderResources(0, 1, &m_normalTexture);
	}

	pDeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &m_stride, &m_offset);
	pDeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->DrawIndexed(m_indexCount, 0, 0);

	return;
}


void BoxClass::Shutdown()
{
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

	if (m_normalTexture)
	{
		m_normalTexture->Release();
		m_normalTexture = nullptr;
	}
	
	if (m_verticesTex)
	{
		delete[] m_verticesTex;
		m_verticesTex = nullptr;
	}
	
	if (m_verticesColor)
	{
		delete[] m_verticesColor;
		m_verticesColor = nullptr;
	}

	delete[] m_indices;

	return;
}