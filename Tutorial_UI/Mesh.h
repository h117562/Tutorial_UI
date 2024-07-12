#include <vector>
#include <string>
#include <fbxsdk.h>
#include <d3dx11.h>
#include <DirectXMath.h>

#include "GraphicsStructures.h"

class Mesh
{
public:
	Mesh();

	bool InitializeBuffer(ID3D11Device* pDevice);
	void Render(ID3D11DeviceContext* pDeviceContext);
	void Shutdown();

	void SetResource(ID3D11Device* pDevice, std::string filePath);
public:
	std::vector<VertexTextureCoord> m_vertices;
	std::vector<UINT> m_indices;
	
private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	ID3D11ShaderResourceView* m_diffuseTexture;

	UINT stride;
	UINT offset;
};