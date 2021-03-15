#pragma once
//ÒÑ¿´
enum obs_type {
	pyramid = 1,
	jump = 2,
	longjump=3
};
class obstacle 
{
private:
	VECTOR3 mPos;
	IMesh		mMesh;
	IPicture		mTexture_Common;
	IPicture        mTexture_Hit;
	Material								mMat_Common;
	Material								mMat_Red;
	int randtype;
	int randx;
public:
	obstacle();
	int gettype();
	void Init(int type,float x=0.0f,float z=0.0f);
	void Render();
	VECTOR3 GetPosition();
	
	float getposx();
	float getposz();
	void setPosition(float x,float z);
};
