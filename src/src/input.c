#include "input.h"
#include "wasm4.h"

void Input_Initialize(struct Input *i)
{
	i->m_inputtype=INPUT_MOUSE;
	
	i->m_lastmousex=0;
	i->m_lastmousey=0;
	i->m_lastmousebuttons=0;
	i->m_mousex=0;
	i->m_mousey=0;
	i->m_mousebuttons=0;
	
	i->m_lastgamepad=0;
	i->m_gamepad=0;
}

void Input_Update(struct Input *i)
{
	i->m_lastmousex=i->m_mousex;
	i->m_lastmousey=i->m_mousey;
	i->m_lastmousebuttons=i->m_mousebuttons;
	i->m_mousex=*MOUSE_X;
	i->m_mousey=*MOUSE_Y;
	i->m_mousebuttons=*MOUSE_BUTTONS;
	
	i->m_lastgamepad=i->m_gamepad;
	i->m_gamepad=*GAMEPAD1;
	
	if(i->m_lastmousebuttons!=i->m_mousebuttons)
	{
		i->m_inputtype=INPUT_MOUSE;
	}
	else if(i->m_lastgamepad!=i->m_gamepad)
	{
		i->m_inputtype=INPUT_GAMEPAD;
	}
}

bool Input_MouseButtonClick(struct Input *i, const int button)
{
	if(button<0 || button>3)
	{
		return false;
	}
	return (((i->m_lastmousebuttons >> (button-1)) & 0x01) !=1 && ((i->m_mousebuttons >> (button-1)) & 0x01)==1);
}

bool Input_MouseButtonDown(struct Input *i, const int button)
{
	if(button<0 || button>3)
	{
		return false;
	}
	return ((i->m_mousebuttons >> (button-1)) & 0x01)==1;
}

bool Input_GamepadButtonPress(struct Input *i, const int button)
{
	if(button<0 || button>2)
	{
		return false;
	}
	switch(button)
	{
	case 1:
		return (i->m_lastgamepad & BUTTON_1) != BUTTON_1 && (i->m_gamepad & BUTTON_1) == BUTTON_1;
		break;
	case 2:
		return (i->m_lastgamepad & BUTTON_2) != BUTTON_2 && (i->m_gamepad & BUTTON_2) == BUTTON_2;
		break;
	default:
		return false;
	}
}

bool Input_GamepadButtonDown(struct Input *i, const int button)
{
	if(button<0 || button>3)
	{
		return false;
	}
	switch(button)
	{
	case 1:
		return (i->m_gamepad & BUTTON_1) == BUTTON_1;
		break;
	case 2:
		return (i->m_gamepad & BUTTON_2) == BUTTON_2;
		break;
	default:
		return false;
	}	
}

bool Input_GamepadLeftDown(struct Input *i)
{
	return (i->m_gamepad & BUTTON_LEFT)==BUTTON_LEFT;
}

bool Input_GamepadRightDown(struct Input *i)
{
	return (i->m_gamepad & BUTTON_RIGHT)==BUTTON_RIGHT;
}

bool Input_GamepadUpDown(struct Input *i)
{
	return (i->m_gamepad & BUTTON_UP)==BUTTON_UP;
}
