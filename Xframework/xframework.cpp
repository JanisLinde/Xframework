#include "xframework.h"

#include <fstream>

#include <d3dcompiler.h>
#include "xfwShaderCode.h"

#include "xfwUtilities.h"

xframework::xframework()
{
	// Camera
	m_cameraPosition = DirectX::XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);
	m_cameraTarget = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_cameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_cameraView = DirectX::XMMatrixLookAtLH(m_cameraPosition, m_cameraTarget, m_cameraUp);
}

xframework::~xframework()
{
}

bool xframework::Initialize(HINSTANCE hInstance, HWND hwnd, int width, int height)
{
	m_width = width;
	m_height = height;

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

	// Depth stencil 
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	m_d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &m_depthStencilBuffer);
	m_d3d11Device->CreateDepthStencilView(m_depthStencilBuffer, NULL, &m_depthStencilView);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_d3d11DeviceContext->RSSetViewports(1, &viewport);
	m_d3d11DeviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Initialize constant buffer (per object)
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObj);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = m_d3d11Device->CreateBuffer(&cbbd, NULL, &m_perObjBuffer);

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

	// Input
	m_input = std::make_unique<xfwInput>();
	if (m_input->Initialize(hInstance, hwnd, width, height))
		return false;

	return true;
}

void xframework::Shutdown()
{
	SAFERELEASE(m_swapChain)
	SAFERELEASE(m_d3d11Device)
	SAFERELEASE(m_d3d11DeviceContext)
	SAFERELEASE(m_renderTargetView)
	SAFERELEASE(m_depthStencilBuffer)
	SAFERELEASE(m_depthStencilView)
	SAFERELEASE(m_perObjBuffer)

	SAFERELEASE(m_inputLayout)
	SAFERELEASE(m_vertexShader)
	SAFERELEASE(m_pixelShader)
}

void xframework::Update()
{
	m_input->Update();

	float speed = 0.0025f;

	if (m_input->IsKeyPressed(DIK_A))
	{
		m_cameraMoveLeftRight -= speed;
	}
	if (m_input->IsKeyPressed(DIK_D))
	{
		m_cameraMoveLeftRight += speed;
	}
	if (m_input->IsKeyPressed(DIK_W))
	{
		m_cameraMoveBackForward += speed;
	}
	if (m_input->IsKeyPressed(DIK_S))
	{
		m_cameraMoveBackForward -= speed;
	}

	int x, y;
	m_input->GetMouseMove(x, y);
	m_cameraYaw += x * 0.001f;
	m_cameraPitch += y * 0.001f;

	// Update camera
	m_cameraRotation = XMMatrixRotationRollPitchYaw(m_cameraPitch, m_cameraYaw, 0.0f);
	m_cameraTarget = XMVector3TransformCoord(m_cameraDefaultForward, m_cameraRotation);
	m_cameraTarget = XMVector3Normalize(m_cameraTarget);

	m_cameraRight = XMVector3TransformCoord(m_cameraDefaultRight, m_cameraRotation);
	m_cameraForward = XMVector3TransformCoord(m_cameraDefaultForward, m_cameraRotation);
	m_cameraUp = XMVector3Cross(m_cameraForward, m_cameraRight);

	m_cameraPosition += m_cameraMoveLeftRight * m_cameraRight;
	m_cameraPosition += m_cameraMoveBackForward * m_cameraForward;

	m_cameraMoveLeftRight = 0.0f;
	m_cameraMoveBackForward = 0.0f;

	m_cameraTarget = m_cameraPosition + m_cameraTarget;

	m_cameraView = XMMatrixLookAtLH(m_cameraPosition, m_cameraTarget, m_cameraUp);
}

void xframework::BeginScene(float r, float g, float b)
{
	float bgColor[4] = { r, g, b, 1.0f };
	m_d3d11DeviceContext->ClearRenderTargetView(m_renderTargetView, bgColor);
	m_d3d11DeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0);

	// Update camera
	DirectX::XMMATRIX camProjection = DirectX::XMMatrixPerspectiveFovLH(0.4f * 3.14f, (float)m_width / m_height, 1.0f, 1000.0f);
	m_world = DirectX::XMMatrixIdentity();
	m_wvp = m_world * m_cameraView * camProjection;

	cbPerObj.WVP = DirectX::XMMatrixTranspose(m_wvp);
	m_d3d11DeviceContext->UpdateSubresource(m_perObjBuffer, 0, NULL, &cbPerObj, 0, 0);
	m_d3d11DeviceContext->VSSetConstantBuffers(0, 1, &m_perObjBuffer);
}

void xframework::EndScene()
{
	m_swapChain->Present(0, 0);
}

void xframework::SetCameraLocation(float x, float y, float z)
{
	m_cameraPosition = DirectX::XMVectorSet(x, y, z, 0.0f);
}

void xframework::DrawPoint(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color)
{
	DrawPoint(pos.x, pos.y, pos.z, color.x, color.y, color.z);
}

void xframework::DrawPoint(float x, float y, float z, float r, float g, float b)
{
	HRESULT hr;
	ID3D11Buffer* vertexBuffer = nullptr;

	xfwVertex v[] =
	{
		xfwVertex(x, y, z, r, g, b, 1.0f)
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(xfwVertex) * 1;
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
	m_d3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_d3d11DeviceContext->VSSetShader(m_vertexShader, 0, 0);
	m_d3d11DeviceContext->PSSetShader(m_pixelShader, 0, 0);
	m_d3d11DeviceContext->Draw(1, 0);


	SAFERELEASE(vertexBuffer);
}

void xframework::DrawLine(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT4 color)
{

}

void xframework::DrawLine(float ax, float ay, float az, float bx, float by, float bz,
	float r, float g, float b)
{
	HRESULT hr;
	ID3D11Buffer* vertexBuffer = nullptr;

	xfwVertex v[] =
	{
		xfwVertex(ax, ay, az, r, g, b, 1.0f),
		xfwVertex(bx, by, bz, r, g, b, 1.0f)
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(xfwVertex) * 2;
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
	m_d3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	m_d3d11DeviceContext->VSSetShader(m_vertexShader, 0, 0);
	m_d3d11DeviceContext->PSSetShader(m_pixelShader, 0, 0);
	m_d3d11DeviceContext->Draw(2, 0);


	SAFERELEASE(vertexBuffer);
}

void xframework::DrawTriangle(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 c, DirectX::XMFLOAT4 color)
{
	DrawTriangle(a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z, color.x, color.y, color.z);
}

void xframework::DrawTriangle(float ax, float ay, float az, float bx, float by, float bz,
	float cx, float cy, float cz, float r, float g, float b)
{
	HRESULT hr;
	ID3D11Buffer* vertexBuffer = nullptr;

	xfwVertex v[] =
	{
		xfwVertex(ax, ay, az, r, g, b, 1.0f),
		xfwVertex(bx, by, bz, r, g, b, 1.0f),
		xfwVertex(cx, cy, cz, r, g, b, 1.0f)
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

bool xframework::InputIsKeyPressed(unsigned int key)
{
	if (m_input)
		return m_input->IsKeyPressed(key);
	else
		return false;
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
