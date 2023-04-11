#pragma once

#include "DXCore.h"
#include "Vertex.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

using namespace std;

class Mesh
{
private:
	// Vert and index buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	// Index count
	unsigned int indexCount;

public:
	// Ctor
	Mesh(
		Vertex* verts, // Vertices for our mesh
		int vertCount, // Number of vertices
		unsigned int* indices, // Indices of our buffer
		size_t indexCount, // Number of indices
		Microsoft::WRL::ComPtr<ID3D11Device> device // Creates buffers
	);
	Mesh(const std::wstring& objFile, Microsoft::WRL::ComPtr<ID3D11Device> device);
	~Mesh();
	
	// Get methods
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	unsigned int GetIndexCount();
	
	// Handles drawing the mesh
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

	// Sets up buffers
	void CreateBuffers(Vertex* vertArray, size_t numVerts, unsigned int* indexArray, size_t numIndices, Microsoft::WRL::ComPtr<ID3D11Device> device);

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
};

