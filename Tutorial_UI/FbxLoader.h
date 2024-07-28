#ifndef _FBXLOADER_H_
#define _FBXLOADER_H_

#pragma comment(lib, "libfbxsdk-mt.lib")

#include "Mesh.h"
#include <windows.h>
#include <commdlg.h>
#include <tchar.h>


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
	void processNode(FbxNode*, ID3D11Device*);
	void processMesh(FbxNode*, ID3D11Device*);
	bool GetTextureFromMaterial(FbxSurfaceMaterial*, ID3D11Device*, Mesh&);
	void OpenFileDialog(HWND);

private:
	std::vector<Mesh> m_meshes;
	char* m_filepath;
	std::string m_previousTexturePath;
	DirectX::XMMATRIX m_worldMatrix;//모델의 위치, 회전 방향을 포함한 행렬
};

#endif