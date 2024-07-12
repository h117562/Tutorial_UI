#ifndef _INTERFACE_MANAGER_H_
#define _INTERFACE_MANAGER_H_

#include "DebugInfoUI.h"
#include "ModelInspector.h"
#include "InputClass.h"

class InterfaceManager
{
public:
	InterfaceManager();
	InterfaceManager(const InterfaceManager&);
	~InterfaceManager();

	bool Initialize(D3DClass*);
	void Frame(D3DClass*, HWND hwnd, ShaderManager*, TextClass*, CameraClass* ,InputClass*);
	void Shutdown();

	bool IsModelInspectorEnabled();

private:
	DebugInfoUI* m_debugUI;
	ModelInspector* m_modelInspector;

	bool m_toggleF3, m_toggleF5;
	bool m_enableDebugUI, m_enableModelInspector;
};

#endif