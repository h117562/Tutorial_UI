#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

#include <DirectXMath.h>

//키 매크로
#define LBUTTON 0x01
#define RBUTTON 0x02
#define KEY_A 0x41
#define KEY_S 0x53
#define KEY_D 0x44
#define KEY_W 0x57
#define KEY_ESC 0x1B
#define KEY_F3 0x72
#define KEY_F5 0x74

class InputClass
{
public:
	InputClass();
	~InputClass();

	bool GetKey(const unsigned int keyCode);
	void KeyPressed(const unsigned int keyCode);
	void KeyReleased(const unsigned int keyCode);
	void GetMousePosition(DirectX::XMFLOAT2& mousePos);
	void SetMousePosition(const unsigned int posX, const unsigned int posY);

private:
	bool m_key_A;
	bool m_key_S;
	bool m_key_D;
	bool m_key_W;
	bool m_key_ESC;
	bool m_key_F3;
	bool m_key_F5;

	bool m_leftBtn, m_rightBtn;
	DirectX::XMFLOAT2 m_position;
};

#endif
