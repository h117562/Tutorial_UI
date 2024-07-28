#include "InputClass.h"

InputClass::InputClass()
{
	m_key_A = false;
	m_key_S = false;
	m_key_D = false;
	m_key_W = false;
	m_key_ESC = false;
	m_key_F3 = false;
	m_key_F5 = false;

	m_leftBtn = false;
	m_rightBtn = false;

	m_position = DirectX::XMFLOAT2(0.0f, 0.0f);
}

InputClass::~InputClass()
{

}

bool InputClass::GetKey(const unsigned int keyCode)
{
	switch (keyCode)
	{
	case LBUTTON:
	{
		if (m_leftBtn) return true;
		break;
	}
	case RBUTTON:
	{
		if (m_rightBtn) return true;
		break;
	}
	case KEY_ESC:
	{
		if (m_key_ESC) return true;
		break;
	}
	case KEY_A:
	{
		if (m_key_A) return true;
		break;
	}
	case KEY_S:
	{
		if (m_key_S) return true;
		break;
	}
	case KEY_D:
	{
		if (m_key_D) return true;
		break;
	}
	case KEY_W:
	{
		if (m_key_W) return true;
		break;
	}
	case KEY_F3:
	{
		if (m_key_F3) return true;
		break;
	}
	case KEY_F5:
	{
		if (m_key_F5) return true;
		break;
	}
	}

	return false;
};

void InputClass::KeyPressed(const unsigned int keyCode)
{
	switch (keyCode)
	{
	case LBUTTON:
	{
		m_leftBtn = true;
		break;
	}
	case RBUTTON:
	{
		m_rightBtn = true;
		break;
	}
	case KEY_ESC:
	{
		m_key_ESC = true;
		break;
	}
	case KEY_A:
	{
		m_key_A = true;
		break;
	}
	case KEY_S:
	{
		m_key_S = true;
		break;
	}
	case KEY_D:
	{
		m_key_D = true;
		break;
	}
	case KEY_W:
	{
		m_key_W = true;
		break;
	}
	case KEY_F3:
	{
		m_key_F3 = true;
		break;
	}
	case KEY_F5:
	{
		m_key_F5 = true;
		break;
	}
	}
};

void InputClass::KeyReleased(const unsigned int keyCode)
{
	switch (keyCode)
	{
	case LBUTTON:
	{
		m_leftBtn = false;
		break;
	}
	case RBUTTON:
	{
		m_rightBtn = false;
		break;
	}
	case KEY_ESC:
	{
		m_key_ESC = false;
		break;
	}
	case KEY_A:
	{
		m_key_A = false;
		break;
	}
	case KEY_S:
	{
		m_key_S = false;
		break;
	}
	case KEY_D:
	{
		m_key_D = false;
		break;
	}
	case KEY_W:
	{
		m_key_W = false;
		break;
	}
	case KEY_F3:
	{
		m_key_F3 = false;
		break;
	}
	case KEY_F5:
	{
		m_key_F5 = false;
		break;
	}
	}
};

void InputClass::GetMousePosition(DirectX::XMFLOAT2& mousePos)
{
	mousePos = m_position;

	return;
}

void InputClass::SetMousePosition(const unsigned int posX, const  unsigned int posY)
{
	m_position.x = posX;
	m_position.y = posY;
}
