#ifndef _BUTTON_CLASS_H_
#define _BUTTON_CLASS_H_

#include <d3dx11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "InputClass.h"
#include "CameraClass.h"
#include "D3DClass.h"

using namespace DirectX;

class ButtonClass
{
public:
	ButtonClass();
	ButtonClass(const ButtonClass&);
	~ButtonClass();

	bool Initialize(ID3D11Device*, const WCHAR* normalImageDirectory, const WCHAR* highlightImageDirectory, float positionX, float positionY, float buttonWidth, float buttonHeight);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	bool CheckButtonPressed(D3DClass*, CameraClass*, InputClass*);
	bool CheckButtonHold(D3DClass*, CameraClass*, InputClass*);

private:
	HRESULT InitializeBuffers(ID3D11Device*);
	bool CollisionCheck(D3DClass*, CameraClass*, InputClass*);

private:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 textureCoord;
	};

	Vertex* m_vertices;
	UINT* m_indices;
	int m_indexCount = 6;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	ID3D11ShaderResourceView* m_normalTexture;
	ID3D11ShaderResourceView* m_hightlightTexture;

	UINT m_stride;
	UINT m_offset;

	XMMATRIX m_modelMatrix;

	bool m_enableHighLight;
	bool m_previousButtonState;
};

#endif