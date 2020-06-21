#pragma once

#include <d3d11.h>

#include <DirectXMath.h>

#define SAVERELEASE(x) if(x) { x->Release(); x = nullptr; }

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

	void DrawTriangle(
		DirectX::XMFLOAT3 a,
		DirectX::XMFLOAT3 b,
		DirectX::XMFLOAT3 c,
		DirectX::XMFLOAT4 color);

	void DrawSphere(
		DirectX::XMFLOAT3 m,
		float r,
		DirectX::XMFLOAT4 color);

private:
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_d3d11Device = nullptr;
	ID3D11DeviceContext* m_d3d11DeviceContext = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;

};