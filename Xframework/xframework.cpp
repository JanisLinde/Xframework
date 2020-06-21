#include "xframework.h"

#include <fstream>

#include <d3dcompiler.h>
#include "xfwShaderCode.h"

xframework::xframework()
{
}

xframework::~xframework()
{
}

bool xframework::Initialize(HWND hwnd, int width, int height)
{
	HRESULT hr;
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
	
	ID3D11Texture2D* backBuffer = nullptr;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	hr = m_d3d11Device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
	backBuffer->Release();

	m_d3d11DeviceContext->OMSetRenderTargets(1, &m_renderTargetView, NULL);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	m_d3d11DeviceContext->RSSetViewports(1, &viewport);

	// Initialize shader
	ID3DBlob* vsBuffer;
	ID3DBlob* psBuffer;
	ID3DBlob* errorMsg;
	hr = D3DCompile(shaderCode, strlen(shaderCode), 0, 0, 0, "VS", "vs_5_0", 0, 0, &vsBuffer, &errorMsg);

	if (FAILED(hr))
	{
		if (errorMsg)
			OutputShaderError(errorMsg);
		return false;
	}
	hr = m_d3d11Device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, &m_vertexShader);

	hr = D3DCompile(shaderCode, strlen(shaderCode), 0, 0, 0, "PS", "ps_5_0", 0, 0, &psBuffer, &errorMsg);
	if (FAILED(hr))
	{
		if (errorMsg)
			OutputShaderError(errorMsg);
		return false;
	}
	hr = m_d3d11Device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, &m_pixelShader);

	// Input layout
	hr = m_d3d11Device->CreateInputLayout(layout, numElements, vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &m_inputLayout);

	SAFERELEASE(vsBuffer);
	SAFERELEASE(psBuffer);
	SAFERELEASE(errorMsg);

	return true;
}

void xframework::Shutdown()
{
	SAFERELEASE(m_swapChain)
	SAFERELEASE(m_d3d11Device)
	SAFERELEASE(m_d3d11DeviceContext)
	SAFERELEASE(m_renderTargetView)
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
	HRESULT hr;
	ID3D11Buffer* vertexBuffer = nullptr;

	xfwVertex v[] =
	{
		xfwVertex(a.x, a.y, a.z),
		xfwVertex(b.x, b.y, b.z),
		xfwVertex(c.x, c.y, c.z)
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(xfwVertex) * 3;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = v;
	hr = m_d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Buffer", L"Error", MB_OK);
		return;
	}

	UINT stride = sizeof(xfwVertex);
	UINT offset = 0;
	m_d3d11DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	m_d3d11DeviceContext->IASetInputLayout(m_inputLayout);
	m_d3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3d11DeviceContext->VSSetShader(m_vertexShader, 0, 0);
	m_d3d11DeviceContext->PSSetShader(m_pixelShader, 0, 0);
	m_d3d11DeviceContext->Draw(3, 0);


	SAFERELEASE(vertexBuffer);
}

void xframework::DrawSphere(DirectX::XMFLOAT3 m, float r, DirectX::XMFLOAT4 color)
{
}

void xframework::OutputShaderError(ID3DBlob* errorMsg)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;

	compileErrors = (char*)(errorMsg->GetBufferPointer());
	bufferSize = errorMsg->GetBufferSize();

	fout.open("shader-error.txt");

	for (i = 0; i < bufferSize; i++)
		fout << compileErrors[i];

	fout.close();

	errorMsg->Release();
	errorMsg = 0;
	MessageBox(0, L"Error compiling shader.  Check shader-error.txt for message.", L"Error", MB_OK);

	return;
}
