#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

struct xfwVertex
{
	xfwVertex(): pos(0.0f, 0.0f, 0.0f) {}
	xfwVertex(float x, float y, float z) : pos(x, y, z) {}

	DirectX::XMFLOAT3 pos;
};

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
UINT numElements = ARRAYSIZE(layout);