#include "InterfaceManager.h"

InterfaceManager::InterfaceManager()
{
	m_debugUI = 0;
	m_modelInspector = 0;

	m_enableDebugUI = true;
	m_enableModelInspector = true;
}

InterfaceManager::InterfaceManager(const InterfaceManager&)
{

}

InterfaceManager::~InterfaceManager()
{

}

bool InterfaceManager::Initialize(D3DClass* pD3Dclass)
{
	bool result;

	//각종 디버깅 정보들을 화면에 렌더링하는 UI 클래스
	m_debugUI = new DebugInfoUI;
	if (!m_debugUI)
	{
		return false;
	}

	result = m_debugUI->Initialize(pD3Dclass);
	if (!result)
	{
		return false;
	}

	//모델을 로드하거나 모델의 정보를 볼 수 있게 렌더링하는 UI 클래스
	m_modelInspector = new ModelInspector;
	if (!m_modelInspector)
	{
		return false;
	}

	result = m_modelInspector->Initialize(pD3Dclass->GetDevice());
	if (!result)
	{
		return false;
	}

	return true;
}


void InterfaceManager::Frame(D3DClass* pD3DClass, HWND hwnd, ShaderManager* pShaderManager, TextClass* pTextClass, CameraClass* pCameraClass, InputClass* pInputClass)
{
	bool state;

	//F5 버튼을 눌러 UI를 온오프
	state = pInputClass->GetKey(KEY_F5);
	if (state != m_toggleF5)
	{
		m_toggleF5 = state;
		if (!state) {
			m_enableDebugUI ^= true; //bool 형 반전
		}
	}

	if (m_enableDebugUI)
	{
		m_debugUI->Render(pTextClass, pCameraClass);
	}

	//F3 버튼을 눌러 UI를 온오프
	state = pInputClass->GetKey(KEY_F3);
	if (state != m_toggleF3)
	{
		m_toggleF3 = state;
		if (!state) {
			m_enableModelInspector ^= true; //bool 형 반전
		}
	}

	if (m_enableModelInspector)
	{
		m_modelInspector->Frame(pD3DClass, hwnd, pShaderManager, pTextClass, pCameraClass, pInputClass);
	}

	return;
}

//ModelInpector UI가 활성화 되어있는지 여부를 리턴
bool InterfaceManager::IsModelInspectorEnabled()
{
	return m_enableModelInspector;
}


void InterfaceManager::Shutdown()
{

	if (m_debugUI)
	{
		m_debugUI->Shutdown();
		delete m_debugUI;
		m_debugUI = 0;
	}

	if (m_modelInspector)
	{
		m_modelInspector->Shutdown();
		delete m_modelInspector;
		m_modelInspector = 0;
	}

	return;
}