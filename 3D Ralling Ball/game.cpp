#include"game.h"
using namespace gameplay;

namespace gameplay {
	static ITimer		localTimer(TIMER_TIMEUNIT_MILLISECOND);
	IRenderer			gRenderer;
	UINT					gFPS;
	float					gTimeElapsed;
	ICamera				gCamera;
	int						gRootGameState;
	const float              speed = 150.0f;
	int highscore = 0;
	Scene               gScene;
	Sphere               gBall;
	Iplayer             gPlayer;
	IPicture            menuPlay;
	IPicture            menuHelp;
	IPicture            menuExit;
	IPicture            Help;
	IPicture            pauseReturn;
	IPicture            pauseExit;
	IPicture            again;
	IPicture            back;
	IPicture            helpExit;
	IPicture            recordWord;
	//IPicture            record;
	IPicture            numscore[10];
	std::string         titleStr;
	bool               newstate = 1;
	int GameState = 31;
	HANDLE hIn;
	HANDLE hOut;
	INPUT_RECORD keyRec, mouseRec;
	DWORD res;
	void drawscore(int score, int x, int y)
	{
		int num1 = score % 10;
		gRenderer.DrawPicture(numscore[num1], x + 70, y , x+105, y+50);
		score /= 10;
		int num2 = score % 10;
		gRenderer.DrawPicture(numscore[num2], x + 35, y , x+70, y+50);
		score /= 10;
		int num3 = score % 10;
		gRenderer.DrawPicture(numscore[num3], x, y, x+35, y+50);
	}
	void InitGlobal()
	{
		hIn = GetStdHandle(STD_INPUT_HANDLE);
		gRenderer.Init(450, 250);
		gRenderer.SetWindowTitle("3D Rolling Ball");
		gRenderer.SetCamera(gCamera);
		gPlayer.Init();
		gScene.Init();
		gBall.Init();
		//record.LoadPPM("picture/0.ppm");
		menuExit.LoadPPM("picture/menuExit2.ppm");
		menuPlay.LoadPPM("picture/menuplay3.ppm");
		menuHelp.LoadPPM("picture/menuHelp3.ppm");
		pauseReturn.LoadPPM("picture/pausemenu.ppm");
		pauseExit.LoadPPM("picture/pauseexit.ppm");
		Help.LoadPPM("picture/help.ppm");
		again.LoadPPM("picture/again.ppm");
		back.LoadPPM("picture/back.ppm");
		helpExit.LoadPPM("picture/helpexit.ppm");
		recordWord.LoadPPM("picture/recordword.ppm");
		numscore[0].LoadPPM("picture/0.ppm");
		numscore[1].LoadPPM("picture/1.ppm");
		numscore[2].LoadPPM("picture/2.ppm");
		numscore[3].LoadPPM("picture/3.ppm");
		numscore[4].LoadPPM("picture/4.ppm");
		numscore[5].LoadPPM("picture/5.ppm");
		numscore[6].LoadPPM("picture/6.ppm");
		numscore[7].LoadPPM("picture/7.ppm");
		numscore[8].LoadPPM("picture/8.ppm");
		numscore[9].LoadPPM("picture/9.ppm");
	}
	void GameStateSelector()
	{
		COORD pos;
		DWORD cursorstate,res;
		if (GameState != GS_playing) //加快响应速度，为了避免不必要的初始赋值，降低游戏进行过程种的延迟
		{
			GetNumberOfConsoleInputEvents(hIn, &res);
			_CONSOLE_CURSOR_INFO cci;//读取输入控制台的信息量数目
			hOut = GetStdHandle(STD_OUTPUT_HANDLE);//获取句柄
			GetConsoleCursorInfo(hOut, &cci);
			SetConsoleMode(hIn, ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
			if (res != 0) //当存在输入时，这句话很重要
			{
				//允许鼠标输入模式，但是删除好像也没啥影响。。。。。
				ReadConsoleInput(hIn, &mouseRec, 1, &res);
				//static POINT currentCursorPos = { 0,0 };
				//::GetCursorPos(&currentCursorPos);//获取鼠标位置
				pos = mouseRec.Event.MouseEvent.dwMousePosition;
				cursorstate = mouseRec.Event.MouseEvent.dwButtonState;//获取鼠标状态
			}
		}
		switch (GameState) 
		{
		case GS_menuplay:
			
			mciSendString("play d:\\menumusic.mp3", NULL, 0, NULL);//开始音乐！！
           if (newstate == 1) {
				gRenderer.Clear();
				gRenderer.DrawPicture(menuPlay, 0, 0, GS_bufferwidth, GS_buffetheight);
				gRenderer.DrawPicture(recordWord, 5, 5, 80, 55);
				drawscore(highscore, 80, 5);
				gRenderer.Present();
				newstate = 0;
			}
			if ((pos.X >170&&pos.X
				<290&&pos.Y>170&&pos.Y<200))//这是help的位置
			{
				GameState = GS_menuhelp; newstate = 1;
			}
			if (pos.X > 170 && pos.X
				< 290 && pos.Y>200 && pos.Y < 250)
				GameState = GS_menuexit; newstate = 1;
			if (mouseRec.EventType == MOUSE_EVENT&&cursorstate==0x0001&& pos.X > 170 && pos.X
				< 290 && pos.Y>120 && pos.Y < 170)
			{
				GameState = GS_playing; newstate = 1;
			}
			
			
			
			break;
		case GS_menuhelp:
			if (newstate == 1) {
				gRenderer.Clear();
				gRenderer.DrawPicture(menuHelp, 0, 0, GS_bufferwidth, GS_buffetheight);
				gRenderer.DrawPicture(recordWord, 5, 5, 80, 55);
				drawscore(highscore, 80, 5);
				gRenderer.Present();
				newstate = 0;
			}
			if ( pos.X > 170 && pos.X
				< 290 && pos.Y>200 && pos.Y < 250)
			{
				GameState = GS_menuexit; newstate = 1;
			}
			if (pos.X > 170 && pos.X
				< 290 && pos.Y>120 && pos.Y < 170)
			{
				GameState = GS_menuplay; newstate = 1;
			}
			if (mouseRec.EventType == MOUSE_EVENT && cursorstate == 0x0001 && (pos.X > 170 && pos.X
				< 290 && pos.Y>170 && pos.Y < 200))
			{
				GameState = GS_help; newstate = 1;
			}
			
			break;
		case GS_menuexit:
			if (newstate == 1) {
				gRenderer.Clear();
				gRenderer.DrawPicture(menuExit, 0, 0, GS_bufferwidth, GS_buffetheight);
				gRenderer.DrawPicture(recordWord, 5, 5, 80, 55);
				drawscore(highscore, 80, 5);
				gRenderer.Present();
				newstate = 0;
			}
			if (pos.X > 170 && pos.X
				< 290 && pos.Y>120 && pos.Y < 170)
			{
				GameState = GS_menuplay; newstate = 1;
			}
			if ( pos.X > 170 && pos.X
				< 290 && pos.Y>170 && pos.Y < 200)
			{
				GameState = GS_menuhelp; newstate = 1;
			}
			if (mouseRec.EventType == MOUSE_EVENT && cursorstate == 0x0001 && pos.X > 170 && pos.X
				< 290 && pos.Y>200 && pos.Y < 250)
			{
				exit(0);
			}
			
			break;
		case GS_help:
			if (newstate == 1) {
				gRenderer.Clear();
				gRenderer.DrawPicture(Help, 0, 0, GS_bufferwidth, GS_buffetheight);
				gRenderer.Present();
				newstate = 0;
			}
			if (IS_KEY_DOWN('A'))
			{
				GameState = GS_menuplay; newstate = 1;
			}
			if (pos.X > 0 && pos.X
				< 100 && pos.Y>200 && pos.Y < 250)
			{
				GameState = GS_helpExit; newstate = 1;
			}
			break;
		case GS_helpExit:
			if (newstate == 1) {
				gRenderer.Clear();
				gRenderer.DrawPicture(helpExit, 0, 0, GS_bufferwidth, GS_buffetheight);
				gRenderer.Present();
				newstate = 0;
			}
			if (mouseRec.EventType == MOUSE_EVENT && cursorstate == 0x0001 && pos.X > 0 && pos.X
				< 100 && pos.Y>200 && pos.Y < 250)
			{
				GameState =GS_menuhelp; newstate = 1;
			}
			break;

			break;
		case GS_playing:
			maingame();
			newstate = 1;
			if (IS_KEY_DOWN('S'))
			{
				GameState = GS_pasuereturn; newstate = 1;
			}
			
			break;
		case GS_pasuereturn:
			mciSendString("stop d:\\gamemusic.mp3", NULL, 0, NULL);
			if (newstate == 1) {
				gRenderer.Clear();
				gRenderer.DrawPicture(pauseReturn, 0, 0, GS_bufferwidth, GS_buffetheight);
				drawscore(gBall.getscores(), 190, 53);
				gRenderer.Present();
				newstate = 0;
			}
			if ((pos.X > 170 && pos.X
				< 290 && pos.Y>170 && pos.Y < 200))
			{
				GameState = GS_pasueexit; newstate = 1;
			}
			if (mouseRec.EventType == MOUSE_EVENT && cursorstate == 0x0001 && (pos.X > 170 && pos.X
				< 290 && pos.Y>120 && pos.Y < 150))
			{
				GameState = GS_playing;
			}
			
			break;
		case GS_pasueexit:
			if (newstate == 1) {
				gRenderer.Clear();
				gRenderer.DrawPicture(pauseExit, 0, 0, GS_bufferwidth, GS_buffetheight);
				drawscore(gBall.getscores(), 190, 53);
				gRenderer.Present();
				newstate = 0;
			}
			if ((pos.X > 170 && pos.X
				< 290 && pos.Y>120 && pos.Y < 150))
			{
				GameState = GS_pasuereturn; newstate = 1;
			}
			if (mouseRec.EventType == MOUSE_EVENT && cursorstate == 0x0001 && (pos.X > 170 && pos.X
				< 290 && pos.Y>170 && pos.Y < 200))
			{
				GameState = GS_menuplay; newstate = 1;
				InitGlobal();
			}
			
			break;
		case GS_deadMenuAgain:
			mciSendString("stop d:\\gamemusic.mp3", NULL, 0, NULL);
			if (newstate == 1) {
				gRenderer.Clear();
				gRenderer.DrawPicture(again, 0, 0, GS_bufferwidth, GS_buffetheight);
				drawscore(gBall.getscores(), 190, 53);
				gRenderer.Present();
				newstate = 0;
			}
			if ((pos.X > 170 && pos.X
				< 290 && pos.Y>170 && pos.Y < 200))
			{
				GameState = GS_deadMenuBack; newstate = 1;
			}
			if (mouseRec.EventType == MOUSE_EVENT && cursorstate == 0x0001 && (pos.X > 170 && pos.X
				< 290 && pos.Y>120 && pos.Y < 150))
			{
				maingame();
				newstate = 1;
			}
		
			break;
		case GS_deadMenuBack:
			if (newstate == 1) {
				gRenderer.Clear();
				gRenderer.DrawPicture(back, 0, 0, GS_bufferwidth, GS_buffetheight);
				drawscore(gBall.getscores(), 190, 53);
				gRenderer.Present();
				newstate = 0;
			}
			if ((pos.X > 170 && pos.X
				< 290 && pos.Y>120 && pos.Y < 150))
			{
				GameState = GS_deadMenuAgain; newstate = 1;
			}
			if (mouseRec.EventType == MOUSE_EVENT && cursorstate == 0x0001 && (pos.X > 170 && pos.X
				< 290 && pos.Y>170 && pos.Y < 200))
			{
				InitGlobal();//这里有漏洞，其实应该设立一个again的参数，判断是否时再一次的进入
				GameState = GS_menuplay; newstate = 1;
			}
		}
	}
	void maingame() {
		mciSendString("stop d:\\menumusic.mp3", NULL, 0, NULL);
		mciSendString("play d:\\gamemusic.mp3", NULL, 0, NULL);
		localTimer.NextTick();//计算一帧的时间，和FPS
		gTimeElapsed = Clamp(localTimer.GetInterval(), 0.0f, 100.0f);
		titleStr = "Rolling ball!! Record：" + std::to_string(highscore);
		gRenderer.SetWindowTitle(titleStr.c_str());
		gRenderer.Clear();
		std::thread tplayer(&Iplayer::UpdateandRender, &gPlayer);
		tplayer.join();//这里尝试着用多线程去缩短反应时间

		//std::thread tSphere(&Sphere::UpdateandRender, &gBall);
		//tSphere.join();
		std::thread tScene(&Scene::Render, &gScene);
		tScene.join();
		if (gPlayer.endanimation())
		{
			std::thread tBall(&Sphere::UpdateandRender, &gBall);
			tBall.join();
		}//结束动画
		//gBall.UpdateandRender();
		drawscore(gBall.getscores(), 5, 5);//画分数
		gRenderer.Present();
	}
	
}