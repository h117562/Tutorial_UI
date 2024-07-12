#ifndef _APPLICATION_CLASS_H_
#define _APPLICATION_CLASS_H_

#include "D3DClass.h"
#include "ShaderManager.h"
#include "CameraClass.h"
#include "InputClass.h"
#include "FrameTimer.h"
#include "InterfaceManager.h"
#include "TextClass.h"

class ApplicationClass
{
public:
	ApplicationClass();
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, bool V_SYNC, bool FullScreen, float ScreenDepth, float ScreenNear, InputClass*);
	void Shutdown();
	bool Frame(HWND, InputClass*, FrameTimer*);

private:
	void Render(InputClass*);
private:
	D3DClass* m_Direct3D;
	CameraClass* m_CameraClass;
	InterfaceManager* m_uiManager;
	TextClass* m_TextClass;
	ShaderManager* m_ShaderManager;
	XMMATRIX model1_world;
	XMMATRIX model2_world;
};
#endif