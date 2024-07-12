#ifndef _FBXLOADER_H_
#define _FBXLOADER_H_

#pragma comment(lib, "libfbxsdk-mt.lib")

#include <windows.h>
#include <commdlg.h>
#include <tchar.h>

#include "Mesh.h"

class FbxLoader
{
public:
	FbxLoader();
	FbxLoader(const FbxLoader&);
	~FbxLoader();

	bool LoadFile(ID3D11Device*, HWND);
	void Shutdown();
	void Render(ID3D11DeviceContext* pDeviceContext);

	void RotateLeftModel();
	void RotateRightModel();
	void ZoomInModel();
	void ZoomOutModel();

	DirectX::XMMATRIX GetWorldMatrix();
private:
	void processNode(FbxNode*, ID3D11Device* pDevice);
	void processMesh(FbxNode*, ID3D11Device* pDevice);
	bool GetTextureFromMaterial(FbxSurfaceMaterial*, ID3D11Device* pDevice, Mesh& mesh);
	void OpenFileDialog(HWND);

private:
	std::vector<Mesh> m_meshes;
	char* m_filepath;

	DirectX::XMMATRIX m_worldMatrix;//모델의 위치, 회전 방향을 포함한 행렬
};

#endif