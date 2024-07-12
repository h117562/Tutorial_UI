#ifndef _BOX_CLASS_H_
#define _BOX_CLASS_H_

#include <d3dx11.h>
#include <DirectXMath.h>

#include "GraphicsStructures.h"

using namespace DirectX;

class BoxClass
{
public:
	BoxClass();
	BoxClass(const BoxClass&);
	~BoxClass();

	bool Initialize(ID3D11Device*, const WCHAR* normalImageDirectory, float positionX, float positionY, float boxWidth, float boxHeight);
	bool Initialize(ID3D11Device*, XMFLOAT4 color, float positionX, float positionY, float boxWidth, float boxHeight);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

private:
	HRESULT InitializeBuffers(ID3D11Device*);

private:
	VertexTextureCoord* m_verticesTex;
	VertexColor* m_verticesColor;

	UINT* m_indices;
	int m_indexCount = 6;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	ID3D11ShaderResourceView* m_normalTexture;

	UINT m_stride;
	UINT m_offset;
};


#endif
