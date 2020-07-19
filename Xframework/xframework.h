#pragma once

#include "xfwVertex.h"

#define SAFERELEASE(x) if(x) { x->Release(); x = nullptr; }

// Constant-Buffers
struct cbPerObj
{
	DirectX::XMMATRIX WVP;
};


class __declspec(dllexport) xframework
{
public:
	xframework();
	~xframework();

	bool Initialize(HWND hwnd, int width, int height);
	void Shutdown();

	void BeginScene(float r, float g, float b);
	void EndScene();

	void SetCameraLocation(float x, float y, float z);

	void DrawPoint(
		DirectX::XMFLOAT3 pos,
		DirectX::XMFLOAT4 color
	);

	void DrawPoint(
		float x, float y, float z,
		float r, float g, float b
	);

	void DrawLine(
		float ax, float ay, float az,
		float bx, float by, float bz,
		float r, float g, float b
	);

	void DrawLine(
		DirectX::XMFLOAT3 a,
		DirectX::XMFLOAT3 b,
		DirectX::XMFLOAT4 color
	);

	void DrawTriangle(
		DirectX::XMFLOAT3 a,
		DirectX::XMFLOAT3 b,
		DirectX::XMFLOAT3 c,
		DirectX::XMFLOAT4 color);

	void DrawTriangle(
		float ax, float ay, float az,
		float bx, float by, float bz,
		float cx, float cy, float cz,
		float r, float g, float b
	);

private:
	void OutputShaderError(ID3DBlob* errorMsg);

private:
	int m_width = 0;
	int m_height = 0;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_d3d11Device = nullptr;
	ID3D11DeviceContext* m_d3d11DeviceContext = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;

	// Shader
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;

	// Constant buffer per object
	ID3D11Buffer* m_perObjBuffer = nullptr;
	cbPerObj cbPerObj;

	DirectX::XMMATRIX m_wvp;
	DirectX::XMMATRIX m_world;

	// Camera
	DirectX::XMMATRIX m_cameraView;
	DirectX::XMVECTOR m_cameraPosition;
	DirectX::XMVECTOR m_cameraTarget;
	DirectX::XMVECTOR m_cameraUp;

};