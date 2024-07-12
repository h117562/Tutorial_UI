#include "ButtonClass.h"


ButtonClass::ButtonClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_vertices = 0;
	m_indices = 0;

	m_stride = sizeof(Vertex);
	m_offset = 0;

	m_enableHighLight = false;
	m_previousButtonState = false;
}

ButtonClass::ButtonClass(const ButtonClass& other)
{

}

ButtonClass::~ButtonClass()
{

}

bool ButtonClass::Initialize(ID3D11Device* pDevice, const WCHAR* filepath1, const WCHAR* filepath2, float posX, float posY, float buttonWidth, float buttonHeight)
{
	HRESULT result;

	m_vertices = new Vertex[6];
	m_indices = new UINT[6];

	//�簢�� ��ư�� ������ �ʱ�ȭ�ϰ� �ؽ�ó ��ǥ�� �̿� ���缭 ����
	m_vertices[0].position = XMFLOAT3((posX + buttonWidth / 2), (posY + buttonHeight / 2), 0.0f);
	m_vertices[1].position = XMFLOAT3((posX - buttonWidth / 2), (posY - buttonHeight / 2), 0.0f);
	m_vertices[2].position = XMFLOAT3((posX - buttonWidth / 2), (posY + buttonHeight / 2), 0.0f);

	m_vertices[3].position = XMFLOAT3((posX - buttonWidth / 2), (posY - buttonHeight / 2), 0.0f);
	m_vertices[4].position = XMFLOAT3((posX + buttonWidth / 2), (posY + buttonHeight / 2), 0.0f);
	m_vertices[5].position = XMFLOAT3((posX + buttonWidth / 2), (posY - buttonHeight / 2), 0.0f);

	m_vertices[0].textureCoord = XMFLOAT2(1.0f, 0.0f); // ������ ���
	m_vertices[1].textureCoord = XMFLOAT2(0.0f, 1.0f); // ���� �ϴ�
	m_vertices[2].textureCoord = XMFLOAT2(0.0f, 0.0f); // ���� ���

	m_vertices[3].textureCoord = XMFLOAT2(0.0f, 1.0f); // ���� �ϴ�
	m_vertices[4].textureCoord = XMFLOAT2(1.0f, 0.0f); // ������ ���
	m_vertices[5].textureCoord = XMFLOAT2(1.0f, 1.0f); // ������ �ϴ�

	//�ε��� �ʱ�ȭ
	int indexCount = 6;
	for (int i = 0; i < indexCount; i++)
	{
		m_indices[i] = i;
	}

	//�ؽ�ó ��θ� ���� �̹����� ���̴� ���ҽ� ��� ��ȯ
	result = D3DX11CreateShaderResourceViewFromFile(pDevice, filepath1, 0, 0, &m_normalTexture, 0);
	if (FAILED(result))
	{
		m_normalTexture = NULL;
	}

	//���̶���Ʈ �ؽ�ó�� �Բ� ��ȯ
	result = D3DX11CreateShaderResourceViewFromFile(pDevice, filepath2, 0, 0, &m_hightlightTexture, 0);
	if (FAILED(result))
	{
		m_hightlightTexture = NULL;
	}

	//���� ����
	result = InitializeBuffers(pDevice);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


HRESULT ButtonClass::InitializeBuffers(ID3D11Device* pDevice)
{
	HRESULT result;

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_indexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = m_vertices;

	result = pDevice->CreateBuffer(&vertexBufferDesc, &initData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return result;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
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

void ButtonClass::Render(ID3D11DeviceContext* pDeviceContext)
{

	//���̶���Ʈ Ȱ�� ���ο� ���� �˸´� �ؽ�ó�� ����
	if (!m_enableHighLight)
	{
		pDeviceContext->PSSetShaderResources(0, 1, &m_normalTexture);
	}
	else
	{
		pDeviceContext->PSSetShaderResources(0, 1, &m_hightlightTexture);
	}
	
	pDeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &m_stride, &m_offset);
	pDeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->DrawIndexed(m_indexCount, 0, 0);

	return;
}

//��ư�� �����ٰ� ������ Ȯ���ϴ� �Լ�
bool ButtonClass::CheckButtonPressed(D3DClass* pD3DClass, CameraClass* pCameraClass, InputClass* pInputClass)
{
	bool result;

	//���콺�� ��ư �������� �ִ��� Ȯ��
	result = CollisionCheck(pD3DClass, pCameraClass, pInputClass);
	if (result)
	{
		bool state = pInputClass->GetKey(LBUTTON);//���콺 ���� ��ư üũ

		//���� ��ư ���¿� �ٸ� ��쿡
		if (m_previousButtonState != state)
		{
			m_enableHighLight ^= true; //bool �� ����
			m_previousButtonState = state;

			//��ư ���� ������ ������ ���� ���
			if (!state)
			{
				m_enableHighLight = false;
				return true;
			}
		}
	}
	else//���� ������ ������ �ʱ�ȭ
	{
		m_previousButtonState = false;
		m_enableHighLight = false;
	}
	
	return false;
}

//��ư�� �ǽð����� ������ �ִ��� Ȯ���ϴ� �Լ�
bool ButtonClass::CheckButtonHold(D3DClass* pD3DClass, CameraClass* pCameraClass, InputClass* pInputClass)
{
	bool result;

	//���콺�� ��ư �������� �ִ��� Ȯ��
	result = CollisionCheck(pD3DClass, pCameraClass, pInputClass);
	if (result)
	{
		bool state = pInputClass->GetKey(LBUTTON);//���콺 ���� ��ư üũ

		//��ư�� Ȧ�� ���� ��
		if (state)
		{
			m_enableHighLight = state;
			return true;
		}
		else//Ȧ�� ���� �ƴ� ��
		{
			m_enableHighLight = state;
			return false;
		}
	}
	else//���� ������ ������ �ʱ�ȭ
	{
		m_previousButtonState = false;
		m_enableHighLight = false;
	}

	return false;
}

//����ĳ������ �̿��Ͽ� ���콺�� ��ư���� �ö� �ִ��� Ȯ���ϱ� ���� �Լ�
bool ButtonClass::CollisionCheck(D3DClass* pD3DClass, CameraClass* pCameraClass, InputClass* pInputClass)
{
	bool result;

	XMMATRIX viewMatrix, projectionMatrix, viewProjectionMatrix;
	XMMATRIX inverseViewProjection;
	XMFLOAT2 mousePoint, screenSize;
	XMVECTOR rayOrigin, rayDirection;
	
	pInputClass->GetMousePosition(mousePoint);//���콺 ��ġ�� �޾ƿ�
	pD3DClass->GetScreenSize(screenSize);//���α׷� ����� �޾ƿ�

	mousePoint = XMFLOAT2(
		((2.0f * mousePoint.x) / screenSize.x) - 1.0f,
		((-2.0f * mousePoint.y) / screenSize.y) + 1.0f
	);//���콺 ��ġ�� ����ȭ

	//UI�� ��� 0,0,-10 ��ġ�� �⺻ �� ��Ʈ�������� ������ �Ǵ� ������ ���길 �ϸ� ��
	pCameraClass->GetBaseViewMatrix(viewMatrix);//�⺻ ���� �� ��Ʈ����
	pD3DClass->GetOrthoMatrix(projectionMatrix);//���� ���� ��Ʈ����

	viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);//��, ���� ���� ��� ��
	inverseViewProjection = XMMatrixInverse(NULL, viewProjectionMatrix);//������� ����

	rayOrigin = XMVectorSet(mousePoint.x, mousePoint.y, 0.0f, 1.0f);//���� ����
	rayDirection = XMVectorSet(mousePoint.x, mousePoint.y, 1.0f, 1.0f);//���� ��

	rayOrigin = XMVector3Transform(rayOrigin, inverseViewProjection);//��.���� ����ķ� ���� ��ȯ
	rayDirection = XMVector3Transform(rayDirection, inverseViewProjection);//��.���� ����ķ� ���� ��ȯ

	rayDirection = XMVectorSubtract(rayDirection, rayOrigin);//���� ������ ������ ���� ������ ����
	rayDirection = XMVector3Normalize(rayDirection);//���� ������ ����ȭ

	float dist = 0.0f;//�浹 �Ÿ��� �޴� ����
	XMVECTOR v1, v2, v3;

	//XMFLOAT3�� XMVECTOR�� ��ȯ
	v1 = XMLoadFloat3(&m_vertices[0].position);
	v2 = XMLoadFloat3(&m_vertices[1].position);
	v3 = XMLoadFloat3(&m_vertices[2].position);

	//�浹 üũ �Լ�
	result = TriangleTests::Intersects(rayOrigin, rayDirection, v1, v2, v3, dist);
	if (result)
	{
		return true;
	}

	//XMFLOAT3�� XMVECTOR�� ��ȯ
	v1 = XMLoadFloat3(&m_vertices[3].position);
	v2 = XMLoadFloat3(&m_vertices[4].position);
	v3 = XMLoadFloat3(&m_vertices[5].position);

	//�浹 üũ �Լ�
	result = TriangleTests::Intersects(rayOrigin, rayDirection, v1, v2, v3, dist);
	if (result)
	{
		return true;
	}

	return false;
}

void ButtonClass::Shutdown()
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

	if (m_hightlightTexture)
	{
		m_hightlightTexture->Release();
		m_hightlightTexture = nullptr;
	}

	delete[] m_vertices;
	delete[] m_indices;
}