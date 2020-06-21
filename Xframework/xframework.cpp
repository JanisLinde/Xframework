#include "xframework.h"

xframework::xframework()
{
}

xframework::~xframework()
{
}

bool xframework::Initialize(HWND hwnd, int width, int height)
{
	HRESULT hr;

	//Describe our Buffer
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = width;
	bufferDesc.Height = height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_d3d11Device, NULL, &m_d3d11DeviceContext);
	
	ID3D11Texture2D* BackBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
	hr = m_d3d11Device->CreateRenderTargetView(BackBuffer, NULL, &m_renderTargetView);
	BackBuffer->Release();

	m_d3d11DeviceContext->OMSetRenderTargets(1, &m_renderTargetView, NULL);

	return true;
}

void xframework::Shutdown()
{
	SAVERELEASE(m_swapChain)
	SAVERELEASE(m_d3d11Device)
	SAVERELEASE(m_d3d11DeviceContext)
	SAVERELEASE(m_renderTargetView)
}

void xframework::BeginScene(float r, float g, float b)
{
	float bgColor[4] = { r, g, b, 1.0f };
	m_d3d11DeviceContext->ClearRenderTargetView(m_renderTargetView, bgColor);
}

void xframework::EndScene()
{
	m_swapChain->Present(0, 0);
}

void xframework::SetCameraLocation(float x, float y, float z)
{
}

void xframework::DrawTriangle(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 c, DirectX::XMFLOAT4 color)
{
}

void xframework::DrawSphere(DirectX::XMFLOAT3 m, float r, DirectX::XMFLOAT4 color)
{
}
