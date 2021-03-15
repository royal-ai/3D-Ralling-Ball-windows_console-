#include"game.h"
using namespace gameplay;
obstacle::obstacle() {
	mMat_Common.ambient = { 1.0f,1.0f,1.0f };
	mMat_Common.diffuse = { 1.0f,1.0f,1.0f };
	mMat_Common.specular = { 0.f,0.f,0.f };

	mMat_Red.ambient = { 0.9f,0,0 };
	mMat_Red.diffuse = { 0.3f,0,0 };
	mMat_Common.specular = { 0.5f,0.5f,0.5f };
}
void obstacle::Init(int type, float x,float z)//传入障碍物的类型，x位置和z位置
{
	switch (type)
	{
	case 1:
		mMesh.LoadFile_OBJ("picture/pyramid(2).obj");
		mTexture_Common.LoadPPM("picture/checkerboard.ppm");
		mPos = VECTOR3(x, 100.0f, z);
		break;
	case 2:
		mMesh.LoadFile_OBJ("picture/jump(1).obj");
		mTexture_Common.LoadPPM("picture/cai.ppm");
		mPos = VECTOR3(x, 100.0f, z);
		break;
	case 3:
		mMesh.LoadFile_OBJ("picture/longjump.obj");
		mTexture_Common.LoadPPM("picture/checkerboard.ppm");
		mPos = VECTOR3(0.0f, 100.0f, z);
	}
	mMesh.SetMaterial(mMat_Common);
	mMesh.SetTexture(&mTexture_Common);
	mMesh.SetPosition(mPos.x, mPos.y, mPos.z);
}



void obstacle::Render()//展示障碍物
{
	gRenderer.RenderMesh(mMesh);
}
VECTOR3 obstacle::GetPosition()
{
	return mPos;
}
int obstacle::gettype()
{
	return randtype;
}
float obstacle::getposx()
{
	return mPos.x;
}
float obstacle::getposz()
{
	return mPos.z;
}
void obstacle::setPosition(float x, float z)

{
	mPos.x = x;
	mPos.z = z;
	mMesh.SetPosition(mPos.x, mPos.y, mPos.z);
}