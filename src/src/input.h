#pragma once

#include <stdint.h>
#include <stdbool.h>

enum GameInput
{
INPUT_MOUSE=0,
INPUT_GAMEPAD=1
};

struct Input
{
	uint8_t m_inputtype;
	
	short m_lastmousex;
	short m_lastmousey;
	uint8_t m_lastmousebuttons;
	short m_mousex;
	short m_mousey;
	uint8_t m_mousebuttons;
	
	uint8_t m_lastgamepad;
	uint8_t m_gamepad;
};

void Input_Initialize(struct Input *i);
void Input_Update(struct Input *i);
bool Input_MouseButtonClick(struct Input *i, const int button);
bool Input_MouseButtonDown(struct Input *i, const int button);
bool Input_GamepadButtonPress(struct Input *i, const int button);
bool Input_GamepadButtonDown(struct Input *i, const int button);
bool Input_GamepadLeftDown(struct Input *i);
bool Input_GamepadRightDown(struct Input *i);
bool Input_GamepadUpDown(struct Input *i);