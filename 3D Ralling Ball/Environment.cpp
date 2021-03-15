#include"game.h"
using namespace gameplay;
void environment::Init()
{

	//------------------Init Art Resources------------------
	mDefaultMaterial.ambient = { 1.0f,1.0f,1.0f };
	mDefaultMaterial.diffuse = { 1.0f,1.0f,1.0f };
	mDefaultMaterial.specular = { 0.0f,0.0f,0.0f };
	mDefaultMaterial.specularSmoothLevel = 20;


	
	mSkyTexture.LoadPPM("picture/chicken.ppm");

		//sky dome
	mSkyModel.LoadFile_OBJ("picture/background9.obj");//创建一个球
	//mSkyModel.CreateCylinder(1000.0f, 25.0f, 40,40);
	mSkyModel.SetPosition(0, 0.0f,6000.0f);

	mSkyModel.SetMaterial(mDefaultMaterial);
	mSkyModel.SetTexture(&mSkyTexture);

	//-----------------Lights----------------------
	mSceneLight.mAmbientColor = { 1.0f,1.0f,1.0f };
	mSceneLight.mDiffuseColor = { 1.0f,1.0f,1.0f };
	mSceneLight.mDiffuseIntensity = 1.0f;
	mSceneLight.mDirection = { -1.0f,-1.0f,-1.0f };
	mSceneLight.mIsEnabled = TRUE;
	mSceneLight.mSpecularColor = { 1.0f,1.0f,1.0f };
	mSceneLight.mSpecularIntensity = 1.2f;
	//环境光
	gRenderer.SetLight(0, mSceneLight);

	//-----------------Init Cursor--------------------
	
}

void environment::Update()
{
	VECTOR3 pos = gCamera.GetPosition();
	mSkyModel.SetPosition(pos.x, pos.y, pos.z);//环境随着摄像机的变化而变化
	//因为是跑酷游戏所以随着摄像机的变化而变化
}

void environment::Render()
{
	gRenderer.RenderMesh(mSkyModel);
	
}