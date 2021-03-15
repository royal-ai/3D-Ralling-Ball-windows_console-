#pragma once

class Iplayer {
private:
	VECTOR3 mLastPos;
	VECTOR3 mCurrentPos;
	static const float	c_halfPlayerWidth;
	static const float	c_halfPlayerHeight;
	static const float	c_halfPlayerDepth;
	VECTOR3 mPos;
	IMesh       mMeshEnv;
	IPicture    mTexture_Env;
	Material								mMat_Common;
	float startlerpt = 0.0f;//startlerpt  是线性插值
	float endlerpt = 0.0f;
	
public:
	Iplayer();
	void Init();
	void	mFunction_UpdateMovement();
	void update();
	void UpdateandRender();
	void startanimation();
	void whileupdate();
	//static unsigned __stdcall StaticUpdate(void * pThis);//原本是用来弄多线程的，现在已经没用了
	bool endanimation();
	void gameoveranimaiton();//游戏结束动画
	bool endgameoveranimation();//游戏结束动画是否播放结束
};