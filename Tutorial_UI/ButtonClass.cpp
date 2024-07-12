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

	//사각형 버튼의 정점을 초기화하고 텍스처 좌표도 이에 맞춰서 매핑
	m_vertices[0].position = XMFLOAT3((posX + buttonWidth / 2), (posY + buttonHeight / 2), 0.0f);
	m_vertices[1].position = XMFLOAT3((posX - buttonWidth / 2), (posY - buttonHeight / 2), 0.0f);
	m_vertices[2].position = XMFLOAT3((posX - buttonWidth / 2), (posY + buttonHeight / 2), 0.0f);

	m_vertices[3].position = XMFLOAT3((posX - buttonWidth / 2), (posY - buttonHeight / 2), 0.0f);
	m_vertices[4].position = XMFLOAT3((posX + buttonWidth / 2), (posY + buttonHeight / 2), 0.0f);
	m_vertices[5].position = XMFLOAT3((posX + buttonWidth / 2), (posY - buttonHeight / 2), 0.0f);

	m_vertices[0].textureCoord = XMFLOAT2(1.0f, 0.0f); // 오른쪽 상단
	m_vertices[1].textureCoord = XMFLOAT2(0.0f, 1.0f); // 왼쪽 하단
	m_vertices[2].textureCoord = XMFLOAT2(0.0f, 0.0f); // 왼쪽 상단

	m_vertices[3].textureCoord = XMFLOAT2(0.0f, 1.0f); // 왼쪽 하단
	m_vertices[4].textureCoord = XMFLOAT2(1.0f, 0.0f); // 오른쪽 상단
	m_vertices[5].textureCoord = XMFLOAT2(1.0f, 1.0f); // 오른쪽 하단

	//인덱스 초기화
	int indexCount = 6;
	for (int i = 0; i < indexCount; i++)
	{
		m_indices[i] = i;
	}

	//텍스처 경로를 통해 이미지를 쉐이더 리소스 뷰로 변환
	result = D3DX11CreateShaderResourceViewFromFile(pDevice, filepath1, 0, 0, &m_normalTexture, 0);
	if (FAILED(result))
	{
		m_normalTexture = NULL;
	}

	//하이라이트 텍스처도 함께 변환
	result = D3DX11CreateShaderResourceViewFromFile(pDevice, filepath2, 0, 0, &m_hightlightTexture, 0);
	if (FAILED(result))
	{
		m_hightlightTexture = NULL;
	}

	//버퍼 생성
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

	//하이라이트 활성 여부에 따라 알맞는 텍스처을 세팅
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

//버튼을 눌렀다가 땟는지 확인하는 함수
bool ButtonClass::CheckButtonPressed(D3DClass* pD3DClass, CameraClass* pCameraClass, InputClass* pInputClass)
{
	bool result;

	//마우스가 버튼 영역내에 있는지 확인
	result = CollisionCheck(pD3DClass, pCameraClass, pInputClass);
	if (result)
	{
		bool state = pInputClass->GetKey(LBUTTON);//마우스 왼쪽 버튼 체크

		//이전 버튼 상태와 다를 경우에
		if (m_previousButtonState != state)
		{
			m_enableHighLight ^= true; //bool 형 반전
			m_previousButtonState = state;

			//버튼 영역 내에서 눌렀다 땟을 경우
			if (!state)
			{
				m_enableHighLight = false;
				return true;
			}
		}
	}
	else//영역 밖으로 나가면 초기화
	{
		m_previousButtonState = false;
		m_enableHighLight = false;
	}
	
	return false;
}

//버튼을 실시간으로 누르고 있는지 확인하는 함수
bool ButtonClass::CheckButtonHold(D3DClass* pD3DClass, CameraClass* pCameraClass, InputClass* pInputClass)
{
	bool result;

	//마우스가 버튼 영역내에 있는지 확인
	result = CollisionCheck(pD3DClass, pCameraClass, pInputClass);
	if (result)
	{
		bool state = pInputClass->GetKey(LBUTTON);//마우스 왼쪽 버튼 체크

		//버튼을 홀드 중일 때
		if (state)
		{
			m_enableHighLight = state;
			return true;
		}
		else//홀드 중이 아닐 때
		{
			m_enableHighLight = state;
			return false;
		}
	}
	else//영역 밖으로 나가면 초기화
	{
		m_previousButtonState = false;
		m_enableHighLight = false;
	}

	return false;
}

//레이캐스팅을 이용하여 마우스가 버튼위에 올라가 있는지 확인하기 위한 함수
bool ButtonClass::CollisionCheck(D3DClass* pD3DClass, CameraClass* pCameraClass, InputClass* pInputClass)
{
	bool result;

	XMMATRIX viewMatrix, projectionMatrix, viewProjectionMatrix;
	XMMATRIX inverseViewProjection;
	XMFLOAT2 mousePoint, screenSize;
	XMVECTOR rayOrigin, rayDirection;
	
	pInputClass->GetMousePosition(mousePoint);//마우스 위치를 받아옴
	pD3DClass->GetScreenSize(screenSize);//프로그램 사이즈를 받아옴

	mousePoint = XMFLOAT2(
		((2.0f * mousePoint.x) / screenSize.x) - 1.0f,
		((-2.0f * mousePoint.y) / screenSize.y) + 1.0f
	);//마우스 위치를 정규화

	//UI의 경우 0,0,-10 위치의 기본 뷰 매트릭스에서 렌더링 되니 간단한 연산만 하면 됨
	pCameraClass->GetBaseViewMatrix(viewMatrix);//기본 시점 뷰 매트릭스
	pD3DClass->GetOrthoMatrix(projectionMatrix);//직교 투영 매트릭스

	viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);//뷰, 투영 서로 행렬 곱
	inverseViewProjection = XMMatrixInverse(NULL, viewProjectionMatrix);//역행렬을 구함

	rayOrigin = XMVectorSet(mousePoint.x, mousePoint.y, 0.0f, 1.0f);//레이 시작
	rayDirection = XMVectorSet(mousePoint.x, mousePoint.y, 1.0f, 1.0f);//레이 끝

	rayOrigin = XMVector3Transform(rayOrigin, inverseViewProjection);//뷰.투영 역행렬로 벡터 변환
	rayDirection = XMVector3Transform(rayDirection, inverseViewProjection);//뷰.투영 역행렬로 벡터 변환

	rayDirection = XMVectorSubtract(rayDirection, rayOrigin);//레이 끝에서 시작을 빼서 방향을 구함
	rayDirection = XMVector3Normalize(rayDirection);//레이 방향을 정규화

	float dist = 0.0f;//충돌 거리를 받는 변수
	XMVECTOR v1, v2, v3;

	//XMFLOAT3을 XMVECTOR로 변환
	v1 = XMLoadFloat3(&m_vertices[0].position);
	v2 = XMLoadFloat3(&m_vertices[1].position);
	v3 = XMLoadFloat3(&m_vertices[2].position);

	//충돌 체크 함수
	result = TriangleTests::Intersects(rayOrigin, rayDirection, v1, v2, v3, dist);
	if (result)
	{
		return true;
	}

	//XMFLOAT3을 XMVECTOR로 변환
	v1 = XMLoadFloat3(&m_vertices[3].position);
	v2 = XMLoadFloat3(&m_vertices[4].position);
	v3 = XMLoadFloat3(&m_vertices[5].position);

	//충돌 체크 함수
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