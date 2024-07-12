#ifndef _MODEL_INSPECTOR_H_
#define _MODEL_INSPECTOR_H_

#include "FbxLoader.h"
#include "TextClass.h"
#include "BoxClass.h"
#include "ButtonClass.h"
#include "ShaderManager.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "InputClass.h"

class ModelInspector
{
public:
	ModelInspector();
	~ModelInspector();

	bool Initialize(ID3D11Device*);
	void Frame(D3DClass*, HWND hwnd, ShaderManager*, TextClass*, CameraClass*, InputClass*);
	void Shutdown();

private:
	void Render(D3DClass*, ShaderManager*, TextClass*, CameraClass*, InputClass*);
	bool LoadNewModel(ID3D11Device*, HWND);

private:
	ButtonClass* m_fileLoadBtn;
	ButtonClass* m_rotateRightBtn;
	ButtonClass* m_rotateLeftBtn;

	ButtonClass* m_zoomInBtn;
	ButtonClass* m_zoomOutBtn;

	FbxLoader* m_FbxLoader;
	BoxClass* m_panel;
};

#endif
