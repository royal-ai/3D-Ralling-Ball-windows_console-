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
	float startlerpt = 0.0f;//startlerpt  �����Բ�ֵ
	float endlerpt = 0.0f;
	
public:
	Iplayer();
	void Init();
	void	mFunction_UpdateMovement();
	void update();
	void UpdateandRender();
	void startanimation();
	void whileupdate();
	//static unsigned __stdcall StaticUpdate(void * pThis);//ԭ��������Ū���̵߳ģ������Ѿ�û����
	bool endanimation();
	void gameoveranimaiton();//��Ϸ��������
	bool endgameoveranimation();//��Ϸ���������Ƿ񲥷Ž���
};