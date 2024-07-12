#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_CameraClass = 0;
	m_uiManager = 0;
	m_TextClass = 0;
	m_ShaderManager = 0;

	model1_world = XMMatrixIdentity() * XMMatrixScaling(0.1f, 0.1f, 0.1f);
	model2_world = XMMatrixTranslation(5.0f, 5.0f, 0.0f) *XMMatrixScaling(5.0f, 5.0f, 5.0f);
}

ApplicationClass::~ApplicationClass()
{

}

bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, bool vsyncEnabled, bool fullScreen, float screenDepth, float screenNear, InputClass* pInputClass)
{
	bool result;
	
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	result = m_Direct3D->Initialize(vsyncEnabled, hwnd, fullScreen, screenDepth, screenNear);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	m_ShaderManager = new ShaderManager;
	if (!m_ShaderManager)
	{
		return false;
	}

	result = m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ShaderManager.", L"Error", MB_OK);
		return false;
	}

	m_TextClass = new TextClass;
	if (!m_TextClass)
	{
		return false;
	}

	result = m_TextClass->Initialize(m_Direct3D->GetSwapChain());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TextClass.", L"Error", MB_OK);
		return false;
	}

	m_uiManager = new InterfaceManager;
	if (!m_uiManager)
	{
		return false;
	}

	result = m_uiManager->Initialize(m_Direct3D);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the UI Manager Class.", L"Error", MB_OK);
		return false;
	}

	m_CameraClass = new CameraClass;
	if (!m_CameraClass)
	{
		return false;
	}

	m_CameraClass->SetPosition(XMFLOAT3(0.0f, 10.0f, -50.0f));

	return result;
}

void ApplicationClass::Shutdown()
{

	if (m_uiManager)
	{
		m_uiManager->Shutdown();
		delete m_uiManager;
		m_uiManager = 0;
	}

	if (m_TextClass)
	{
		m_TextClass->Shutdown();
		delete m_TextClass;
		m_TextClass = 0;
	}

	if (m_CameraClass)
	{
		delete m_CameraClass;
		m_CameraClass = 0;
	}

	if (m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}
	
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}

bool ApplicationClass::Frame(HWND hwnd, InputClass* pInputClass, FrameTimer* pFrameTimer)
{
	bool result;

	//UI가 비활성화 되어있을때만 카메라 이동 가능하게
	result = m_uiManager->IsModelInspectorEnabled();
	if (true)//test
	{
		bool keyDown;
		float fps = pFrameTimer->GetTime();

		m_CameraClass->SetFrameTime(1);

		keyDown = pInputClass->GetKey(KEY_A);
		m_CameraClass->MoveLeft(keyDown);

		keyDown = pInputClass->GetKey(KEY_S);
		m_CameraClass->MoveBackward(keyDown);

		keyDown = pInputClass->GetKey(KEY_D);
		m_CameraClass->MoveRight(keyDown);

		keyDown = pInputClass->GetKey(KEY_W);
		m_CameraClass->MoveForward(keyDown);


	/*	XMFLOAT2 mousePos, screenSize;

		m_Direct3D->GetScreenSize(screenSize);
		pInputClass->GetMousePosition(mousePos);

		screenSize.x /= 2;
		screenSize.y /= 2;

		mousePos.x -= screenSize.x;
		mousePos.y -= screenSize.y;

		m_CameraClass->UpdateRotation(mousePos);

		RECT rect;
		GetWindowRect(hwnd, &rect);

		int centerX = (rect.left + rect.right) / 2;
		int centerY = (rect.top + rect.bottom) / 2;

		SetCursorPos(centerX, centerY);*/
	}

	m_CameraClass->Render();

	//백 버퍼 초기화
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.2f, 1.0f);

	//model1_world = model1_world * XMMatrixRotationY(0.05f);//회전
	//m_ShaderManager->GetTextureShader()->Render(m_Direct3D->GetDeviceContext(), model1_world, viewMatrix, projectionMatrix);
	//m_FbxLoader->Render(m_Direct3D->GetDeviceContext());

	m_TextClass->BeginDraw();
	m_uiManager->Frame(m_Direct3D,hwnd, m_ShaderManager, m_TextClass, m_CameraClass, pInputClass);

	m_TextClass->EndDraw();
	m_Direct3D->EndScene();

	return true;
}
