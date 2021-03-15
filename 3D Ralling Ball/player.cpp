#include "game.h"
//已看
using namespace gameplay;

const float Iplayer::c_halfPlayerWidth = 25.0f;
const float Iplayer::c_halfPlayerHeight = 35.0f;
const float Iplayer::c_halfPlayerDepth = 25.0f;

Iplayer::Iplayer()
{
	
}
void Iplayer::Init()
{
	
	//------------------PLAYER EYE--------------------
	gCamera.SetPosition(0.0f,800.0f,-3700.0f);// 
	gCamera.SetLookAt(0, 0, 0);
	gCamera.SetViewAngle(Math::CONST_PI / 2.0f, 2.5f);
	gCamera.SetViewFrustumPlane(1.0f, 5000.0f);
	mMeshEnv.LoadFile_OBJ("picture/background9.obj");
	mTexture_Env.LoadPPM("picture/chicken.ppm");
	mPos = VECTOR3(0, 0, 0);
	mMeshEnv.SetMaterial(mMat_Common);
	mMeshEnv.SetTexture(&mTexture_Env);
	mMeshEnv.SetPosition(mPos.x, mPos.y, mPos.z);
	std::fstream gethighscore("test.txt");
	gethighscore >> highscore;
	startlerpt = 0.0f;
	endlerpt = 0.0f;
}
void	Iplayer::mFunction_UpdateMovement()//摄像机的移动
{
	//--------------------------keyboard------------------------------
	VECTOR3 moveVector = { 0,0,0 };
	moveVector.z +=speed;
	//in case that camera moves faster if 3 directions has projection of speed
	
	gCamera.fps_MoveRight(moveVector.x);
	gCamera.fps_MoveForward(moveVector.z);
	gCamera.fps_MoveUp(moveVector.y);

	//restrict player movement to a Box
	VECTOR3 camPos = gCamera.GetPosition();
	/*camPos = { Clamp(camPos.x,-c_halfMovementRestrictBoxWidth,c_halfMovementRestrictBoxWidth),
		Clamp(camPos.y,-c_halfMovementRestrictBoxWidth,c_halfMovementRestrictBoxWidth),
		Clamp(camPos.z,-c_halfMovementRestrictBoxWidth,c_halfMovementRestrictBoxWidth) };
	gCamera.SetPosition(camPos);*/

	//update Position
	mLastPos = mCurrentPos;
	mCurrentPos = camPos;


	//-------------------------------cursor movement----------------------------------
	
}
void Iplayer::update()
{
	Iplayer::mFunction_UpdateMovement();
}

void Iplayer::whileupdate()
{
	while (1)
	{
		update();
	}
}

 void Iplayer::UpdateandRender() {
	 if (!endanimation())
	 {
		 startanimation();
	 }
	 else
	 {
		 if (!gBall.deadCondition())
		 {
			 update();
		 }
		 else
			 gameoveranimaiton();
	 }
	 if (endgameoveranimation())
	 {
		 std::ofstream recordscore("test.txt");
		 int i = gBall.getscores();
		 if (i >= highscore) {
			 highscore = i;//更新最高分数
			 
		 }
		 recordscore << highscore;
	 }
	 gRenderer.RenderMesh(mMeshEnv);
 }
 void Iplayer::gameoveranimaiton()
 {
	 VECTOR3 lastpos = gCamera.GetPosition();
	 VECTOR3 targetpos = { 0.0f,800.0f,-3100.0f };
	 if (endlerpt == 0.05f)//防止突然，留有延迟
	 {
		 mciSendString("stop d:\\gamemusic.mp3", NULL, 0, NULL);
		 mciSendString("play d:\\overmusic.mp3", NULL, 0, NULL);
		 Sleep(2000);
	 }
	 if (endlerpt < 1.0f)
	 {
		 endlerpt += 0.05;
		 VECTOR3 movevector = Lerp(lastpos, targetpos, endlerpt);
		 gCamera.SetPosition(movevector);
	 }
 }
 bool Iplayer::endgameoveranimation()
 {
	 if (endlerpt < 1.0f)
	 {
		 return false;
	 }
	 else return true;
 }
 bool Iplayer::endanimation()//是否结束开场动画
 {
	 if (startlerpt < 1.0f)
	 {
		 return false;
	 }
	 else return true;
 }
 void Iplayer::startanimation()//开场动画
 {
	 if (startlerpt < 1.0f)//
	 {
		 startlerpt += 0.03;
		 VECTOR3 startvector = { 0.0f,2500.0f,-15000.0f };
		 VECTOR3 targetvector = { 0.0f,800.0f,-3700.0f };
		 VECTOR3 moveVector = Lerp(startvector, targetvector, startlerpt);
		 gCamera.SetPosition(moveVector);
		 gRenderer.RenderMesh(mMeshEnv);
	 }
 }