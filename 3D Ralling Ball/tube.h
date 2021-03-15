#pragma once
class tube
{
private:
	VECTOR3 mPos;
	IMesh		mMesh;
	IPicture		mTexture_Common;
	IPicture        mTexture_Hit;
	Material								mMat_Common;
	Material								mMat_Red;
public:
	tube();
	void Init(float x,float z);
	void Render();
	tube operator=(tube a);
	VECTOR3 GetPosition();
	void setPosition(float x, float z);
	void disappear();
};
