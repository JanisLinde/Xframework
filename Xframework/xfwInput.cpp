#include "xfwInput.h"

#include "xfwUtilities.h"

xfwInput::xfwInput()
{
}

xfwInput::~xfwInput()
{
}

bool xfwInput::Initialize(HINSTANCE hInstance, HWND hWnd, int width, int height)
{
	m_width = width;
	m_height = height;

	HRESULT result;

	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
		return false;

	// Keyboard
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
		return false;

	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return false;

	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return false;

	result = m_keyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		return false;

	result = m_keyboard->Acquire();
	if (FAILED(result))
		return false;

	// Mouse
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
		return false;

	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
		return false;

	result = m_mouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
		return false;

	result = m_mouse->Acquire();
	if (FAILED(result))
		return false;

	return true;
}

void xfwInput::Shutdown()
{
	SAFERELEASE(m_keyboard)
	SAFERELEASE(m_mouse)
	SAFERELEASE(m_directInput)
}

void xfwInput::Update()
{
	HRESULT result;

	// Tastatur einlesen
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
	}

	m_lastMouseState = m_currMouseState;
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_currMouseState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
	}

	m_mouseX += m_currMouseState.lX;
	m_mouseY += m_currMouseState.lY;

	// Randcheck
	if (m_mouseX < 0)
		m_mouseX = 0;
	if (m_mouseY < 0)
		m_mouseY = 0;
	if (m_mouseX > m_width)
		m_mouseX = m_width;
	if (m_mouseY > m_height)
		m_mouseY = m_height;
}

bool xfwInput::IsKeyPressed(unsigned int key)
{
	if (m_keyboardState[key] & 0x80)
		return true;
	else
		return false;
}

void xfwInput::GetMouseLocation(int& posX, int& posY)
{
	posX = m_mouseX;
	posY = m_mouseY;
}

void xfwInput::GetMouseMove(int& x, int& y)
{
	if ((m_currMouseState.lX != m_lastMouseState.lX) || (m_currMouseState.lY != m_lastMouseState.lY))
	{
		x = m_currMouseState.lX;
		y = m_currMouseState.lY;
	}
	else
	{
		x = m_lastMouseState.lX;
		y = m_lastMouseState.lY;
	}
}

void xfwInput::SetMouseLocation(const int posX, const int posY)
{
	m_mouseX = posX;
	m_mouseY = posY;
}

bool xfwInput::IsMouseKeyDown(unsigned char key)
{
	return m_currMouseState.rgbButtons[key] & 0x80;
}