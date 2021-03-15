#include"game.h"
using namespace gameplay;
tube::tube() {
	mMat_Common.ambient = { 1.0f,1.0f,1.0f };
	mMat_Common.diffuse = { 1.0f,1.0f,1.0f };
	mMat_Common.specular = { 0.f,0.f,0.f };

	mMat_Red.ambient = { 0.9f,0,0 };
	mMat_Red.diffuse = { 0.3f,0,0 };
	mMat_Common.specular = { 0.5f,0.5f,0.5f };
}
void tube::Init(float x,float z) {

		mMesh.LoadFile_OBJ("picture/tube.obj");
		mTexture_Common.LoadPPM("picture/star.ppm");
		mPos = VECTOR3(x, 100.0f,z);
	   mMesh.SetMaterial(mMat_Common);
	   mMesh.SetTexture(&mTexture_Common);
	   mMesh.SetPosition(mPos.x, mPos.y, mPos.z);
}
void tube::Render()
{
	gRenderer.RenderMesh(mMesh);
}

tube tube::operator=(tube a)
{
	mPos.x = a.mPos.x;
	mPos.y = a.mPos.y;
	mPos.z = a.mPos.z;
	return *this;
}
VECTOR3 tube::GetPosition()
{
	return mPos;
}
void tube::setPosition(float x, float z)
{
	mPos.x = x;
	mPos.z = z;
	mMesh.SetPosition(mPos.x, mPos.y, mPos.z);
}

void tube::disappear()
{
	mMesh.SetPosition(0.0f, 100000.0f, +10000.0f);
}
