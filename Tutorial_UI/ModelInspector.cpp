#include "ModelInspector.h"

ModelInspector::ModelInspector()
{
	m_fileLoadBtn = 0;
	m_rotateRightBtn = 0;
	m_rotateLeftBtn = 0;
	m_zoomInBtn = 0;
	m_zoomOutBtn = 0;
	m_panel = 0;

	m_FbxLoader = 0;
}

ModelInspector::~ModelInspector()
{

}

bool ModelInspector::Initialize(ID3D11Device* pDevice)
{
	bool result;

	m_rotateRightBtn = new ButtonClass;
	if (!m_rotateRightBtn)
	{
		return false;
	}

	m_rotateLeftBtn = new ButtonClass;
	if (!m_rotateLeftBtn)
	{
		return false;
	}

	m_fileLoadBtn = new ButtonClass;
	if (!m_fileLoadBtn)
	{
		return false;
	}

	m_zoomInBtn = new ButtonClass;
	if (!m_zoomInBtn)
	{
		return false;
	}

	m_zoomOutBtn = new ButtonClass;
	if (!m_zoomOutBtn)
	{
		return false;
	}

	result = m_rotateRightBtn->Initialize(pDevice, L"..\\Data\\UI\\RotateRight.png", L"..\\Data\\UI\\RotateRightH.png", 460.0f, -160.0f, 100.0f, 100.0f);
	if (!result)
	{
		return false;
	}

	result = m_rotateLeftBtn->Initialize(pDevice, L"..\\Data\\UI\\RotateLeft.png", L"..\\Data\\UI\\RotateLeftH.png", 340.0f, -160.0f, 100.0f, 100.0f);
	if (!result)
	{
		return false;
	}

	result = m_fileLoadBtn->Initialize(pDevice, L"..\\Data\\UI\\Find.png", L"..\\Data\\UI\\FindH.png", 340.0f, -40.0f, 100.0f, 100.0f);
	if (!result)
	{
		return false;
	}

	result = m_zoomInBtn->Initialize(pDevice, L"..\\Data\\UI\\Star.png", L"..\\Data\\UI\\StarH.png", 460.0f, -280.0f, 100.0f, 100.0f);
	if (!result)
	{
		return false;
	}

	result = m_zoomOutBtn->Initialize(pDevice, L"..\\Data\\UI\\Star.png", L"..\\Data\\UI\\StarH.png", 340.0f, -280.0f, 100.0f, 100.0f);
	if (!result)
	{
		return false;
	}

	m_panel = new BoxClass;
	if (!m_panel)
	{
		return false;
	}

	result = m_panel->Initialize(pDevice, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.7f), 400.0f, -200.0f, 300.0f, 500.0f);
	if (!result)
	{
		return false;
	}

	
}

void ModelInspector::Frame(D3DClass* pD3DClass, HWND hwnd, ShaderManager* pShaderManager, TextClass* pTextClass, CameraClass* pCameraClass, InputClass* pInputClass)
{
	bool result;

	result = m_fileLoadBtn->CheckButtonPressed(pD3DClass, pCameraClass, pInputClass);
	if (result)
	{
		LoadNewModel(pD3DClass->GetDevice(), hwnd);
	}

	result = m_rotateLeftBtn->CheckButtonHold(pD3DClass, pCameraClass, pInputClass);
	if (result)
	{
		if (m_FbxLoader)
		{
			m_FbxLoader->RotateLeftModel();
		}
	}

	result = m_rotateRightBtn->CheckButtonHold(pD3DClass, pCameraClass, pInputClass);
	if (result)
	{
		if (m_FbxLoader)
		{
			m_FbxLoader->RotateRightModel();
		}
	}

	result = m_zoomInBtn->CheckButtonHold(pD3DClass, pCameraClass, pInputClass);
	if (result)
	{
		if (m_FbxLoader)
		{
			m_FbxLoader->ZoomInModel();
		}
	}

	result = m_zoomOutBtn->CheckButtonHold(pD3DClass, pCameraClass, pInputClass);
	if (result)
	{
		if (m_FbxLoader)
		{
			m_FbxLoader->ZoomOutModel();
		}
	}

	Render(pD3DClass, pShaderManager, pTextClass, pCameraClass, pInputClass);

	return;
}

void ModelInspector::Render(D3DClass* pD3DClass, ShaderManager* pShaderManager, TextClass* pTextClass, CameraClass* pCameraClass, InputClass* pInputClass)
{
	bool result;
	XMMATRIX worldMatrix, viewMatrix, baseViewMatrix, projectionMatrix,orthoMatrix;

	pCameraClass->GetBaseViewMatrix(baseViewMatrix);//0,0,-10 위치에서 기본 시점 기준 뷰 매트릭스를 가져와 UI 렌더링에 사용
	pCameraClass->GetViewMatrix(viewMatrix);//현재 카메라의 위치와 회전 방향을 업데이트한 뷰 매트릭스를 가져와 모델 렌더링에 사용
	pD3DClass->GetWorldMatrix(worldMatrix);//기본 월드 매트릭스
	pD3DClass->GetProjectionMatrix(projectionMatrix);//투영 행렬
	pD3DClass->GetOrthoMatrix(orthoMatrix);//원근감이 없는 직교 투영 행렬 UI 렌더링에 사용

	//불러온 모델이 있을 경우에만 렌더링
	if (m_FbxLoader)
	{
		pShaderManager->GetTextureShader()->Render(pD3DClass->GetDeviceContext(), m_FbxLoader->GetWorldMatrix(), viewMatrix, projectionMatrix);
		m_FbxLoader->Render(pD3DClass->GetDeviceContext());
	}

	//UI는 2D로 렌더링 하기 때문에 Z축 버퍼를 꺼도 됨
	//알파 블랜딩은 사용할 버튼의 이미지가 알파값을 필요로 하기 때문에
	pD3DClass->TurnZBufferOff();
	pD3DClass->EnableAlphaBlending();

	pShaderManager->GetColorShader()->Render(pD3DClass->GetDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix);
	m_panel->Render(pD3DClass->GetDeviceContext());

	pShaderManager->GetTextureShader()->Render(pD3DClass->GetDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix);
	m_rotateRightBtn->Render(pD3DClass->GetDeviceContext());

	pShaderManager->GetTextureShader()->Render(pD3DClass->GetDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix);
	m_rotateLeftBtn->Render(pD3DClass->GetDeviceContext());

	pShaderManager->GetTextureShader()->Render(pD3DClass->GetDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix);
	m_fileLoadBtn->Render(pD3DClass->GetDeviceContext());

	pShaderManager->GetTextureShader()->Render(pD3DClass->GetDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix);
	m_zoomInBtn->Render(pD3DClass->GetDeviceContext());

	pShaderManager->GetTextureShader()->Render(pD3DClass->GetDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix);
	m_zoomOutBtn->Render(pD3DClass->GetDeviceContext());

	//UI 렌더링이 끝나고 다시 원래대로
	pD3DClass->TurnZBufferOn();
	pD3DClass->DisableAlphaBlending();

}

bool ModelInspector::LoadNewModel(ID3D11Device* pDevice, HWND hwnd)
{
	bool result;

	if (m_FbxLoader)
	{
		m_FbxLoader->Shutdown();
		delete m_FbxLoader;
		m_FbxLoader = 0;
	}

	m_FbxLoader = new FbxLoader;
	if (m_FbxLoader)
	{
		result = m_FbxLoader->LoadFile(pDevice, hwnd);
	}

	if (!result)
	{
		return false;
	}

	return true;
}

void ModelInspector::Shutdown()
{
	if (m_zoomInBtn)
	{
		m_zoomInBtn->Shutdown();
		delete m_zoomInBtn;
		m_zoomInBtn = 0;
	}

	if (m_zoomOutBtn)
	{
		m_zoomOutBtn->Shutdown();
		delete m_zoomOutBtn;
		m_zoomOutBtn = 0;
	}

	if (m_rotateRightBtn)
	{
		m_rotateRightBtn->Shutdown();
		delete m_rotateRightBtn;
		m_rotateRightBtn = 0;
	}

	if (m_rotateLeftBtn)
	{
		m_rotateLeftBtn->Shutdown();
		delete m_rotateLeftBtn;
		m_rotateLeftBtn = 0;
	}

	if (m_fileLoadBtn)
	{
		m_fileLoadBtn->Shutdown();
		delete m_fileLoadBtn;
		m_fileLoadBtn = 0;
	}

	if (m_panel)
	{
		m_panel->Shutdown();
		delete m_panel;
		m_panel = 0;
	}

	if (m_FbxLoader)
	{
		m_FbxLoader->Shutdown();
		delete m_FbxLoader;
		m_FbxLoader = 0;
	}
}