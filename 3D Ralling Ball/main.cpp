

#include<iostream>
#include "game.h"
using namespace gameplay;
//static ITimer		localTimer(TIMER_TIMEUNIT_MILLISECOND);
int main()
{
	
	InitGlobal();
	
	/*HANDLE   hth1;
	unsigned  uiThread1ID;

	hth1 = (HANDLE)_beginthreadex(NULL,         // security
		0,            // stack size
		Iplayer::StaticUpdate,
		&mplayer,           // arg list
		CREATE_SUSPENDED,  // so we can later call ResumeThread()
		&uiThread1ID);*///另一种多线程的方式
	while (1) 
	{
		GameStateSelector();
	}
	getchar();
	return 0;
}




