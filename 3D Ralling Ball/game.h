#pragma once
#include"MyConsoleEngine.h"
#include"tube.h"
#include"ball.h"
#include"player.h"
#include"Environment.h"
#include"obstacle.h"   
#include"Scene.h"    
namespace gameplay 
{
	extern IRenderer			gRenderer;
	extern UINT					gFPS;
	extern float					gTimeElapsed;
	extern ICamera				gCamera;
	extern int						gRootGameState;
	extern const float                speed;//球和摄像机的速度
	extern Iplayer              gPlayer;
	extern Scene               gScene;
	extern Sphere               gBall;
	extern IPicture            menuPlay;
	extern IPicture            menuHelp;
	extern IPicture            menuExit;
	extern IPicture            Help;
	extern IPicture            pauseReturn;
	extern IPicture            pauseExit;
	extern IPicture            record;
	extern IPicture            again;
	extern IPicture            back;
	extern IPicture            numscore[10];
	extern IPicture            helpExit;
	extern IPicture            recordWord;
	extern std::string titleStr;
	const int GS_bufferwidth = 450;
	const int GS_buffetheight = 250;
	extern int GameState;
	const int GS_menuplay = 31;
	const int GS_menuhelp = 32;
	const int GS_menuexit = 33;
	const int GS_help = 34;
	const int GS_pasuereturn = 35;
	const int GS_pasueexit = 36;
	const int GS_playing = 37;
	const int GS_deadMenuAgain=38;
	const int GS_deadMenuBack=39;
	const int GS_helpExit = 40;
	extern int highscore;
	void InitGlobal();
	void GameStateSelector();
	void maingame();
	void drawscore(int score, int x, int y);
};