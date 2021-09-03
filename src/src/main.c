#include "wasm4.h"
#include "game.h"
#include "wasmrand.h"
#include "global.h"

void start()
{
	_srand(1234);
	
	PALETTE[0]=0x00;		// black
	PALETTE[1]=0x909090;	// dark gray
	PALETTE[2]=0xf0f0f0;	// light gray
	PALETTE[3]=0x00ff21;	// green

	Game_Initialize(&game);
}

void update()
{
	Game_Update(&game,1);
	Game_Draw(&game);
}
