#pragma once

#include <dinput.h>

static const int MOUSE_LEFT = 0;
static const int MOUSE_RIGHT = 1;
static const int MOUSE_MIDDLE = 2;

class __declspec(dllexport) xfwInput
{
public:
	xfwInput();
	~xfwInput();

	bool Initialize(HINSTANCE hInstance, HWND hWnd, int width, int height);
	void Shutdown();

	void Update();

	bool IsKeyPressed(unsigned int key);
	void GetMouseLocation(int& posX, int& posY);
	void GetMouseMove(int& x, int& y);

	void SetMouseLocation(const int posX, const int posY);

	bool IsMouseKeyDown(unsigned char key);

private:
	int m_width = 0;
	int m_height = 0;

	IDirectInput8* m_directInput = nullptr;
	IDirectInputDevice8* m_keyboard = nullptr;
	IDirectInputDevice8* m_mouse = nullptr;

	DIMOUSESTATE m_currMouseState;
	DIMOUSESTATE m_lastMouseState;

	unsigned char m_keyboardState[256];
	int m_mouseX = 0;
	int m_mouseY = 0;
};