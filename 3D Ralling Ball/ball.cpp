#include"game.h"

using namespace gameplay;
Sphere::Sphere() {
	mMat_Common.ambient = { 1.0f,1.0f,1.0f };
	mMat_Common.diffuse = { 0.8f,0.8f,0.8f };
	mMat_Common.specular = { 1.0f,1.0f,1.0f };
}
void Sphere::Init( ) {
	mMesh.LoadFile_OBJ("picture/ball4.obj");
	mTexture_Common.LoadPPM("picture/cai.ppm");
	mPos = VECTOR3(0.0f,100.0f, -3100.0f);
	cpos = mPos;
	jump = 0;
	totaltime = 0;
	fronttubepos = 0;
	record = 0;
	 scores = 0;
	mTexture_End.LoadPPM("picture/star.ppm");
	mMesh.SetMaterial(mMat_Common);
	mMesh.SetTexture(&mTexture_Common);
	mMesh.SetPosition(mPos.x, mPos.y, mPos.z);
	//--------light----------------
	mSceneLight.mAmbientColor = { 1.0f,1.0f,1.0f };
	mSceneLight.mDiffuseColor = { 1.0f,1.0f,1.0f };
	mSceneLight.mDiffuseIntensity = 1.0f;
	mSceneLight.mDirection = { -1.0f,-1.0f,0.0f };
	mSceneLight.mIsEnabled = TRUE;
	mSceneLight.mSpecularColor = { 1.0f,1.0f,1.0f };
	mSceneLight.mSpecularIntensity = 2.0f;

	gRenderer.SetLight(0, mSceneLight);//设置光照，有用的
	createtube();
}
void Sphere::Render() {
	tuberender();
	gRenderer.RenderMesh(mMesh);
}
VECTOR3 Sphere::GetPosition()
{
	return mPos;
}
void Sphere::Update()
{
	translationandrotation();
}
void Sphere::UpdateandRender()
{
	if (deadCondition() == false)
	{
		tubeupdate();
		Update();//球的前进
		Render();
	}
	else {
		mMesh.SetTexture(&mTexture_End);
		gRenderer.RenderMesh(mMesh);
		if(gPlayer.endgameoveranimation())GameState = GS_deadMenuAgain;
	}
}
bool Sphere::deadCondition()
{
	VECTOR2 inf = gScene.getInfObastacle();
	float infz = gScene.getObstaclez();
	if (mPos.z > (infz - 75.0f)&&(mPos.z < infz + 75.0f))
	{
		if (inf.y == 0.0f)//longjump
		{
			if (mPos.y > 400.0f)return false;
			else return true;
		}
		if (inf.y == 1.0f)//singlepyramid和doublejump
		{
			if (mPos.x == ((inf.x-1)*800.0f))
			{
				return true;
			}
			else {
				if (mPos.y > 400.0f)return false;
				else return true;
			}
		}
		if (inf.y == 2.0f)
		{
			if (mPos.x == ((inf.x-1)*800.0f))return false;
			else return true;
		}
		else {
			if (mPos.x == ((inf.x-1)*800.0f)&&mPos.y > 400.0f)
				return false;
			else return true;
			
		}
	}
	return false;
}
void Sphere::translationandrotation()
{
	totaltime += gTimeElapsed;
	float xangle;
	xangle = gTimeElapsed * 0.005;
	mMesh.RotateX_Pitch(xangle);
	//-----旋转-----
	mPos.z += speed;
	cpos.z = mPos.z;
	//cpos 是下一次球的位置
	if (jump == 1&&mPos.y==600.0f)
	{
		cpos.y -= 500.0f;//
		jump = 0;
	}
	
	if (IS_KEY_DOWN('A')&&(mPos.x==0.0f||mPos.x==800.0f))
	{
		cpos.x -= 800.0f;
	}
	if (IS_KEY_DOWN('D') && (mPos.x == 0.0f || mPos.x == -800.0f))
	{
		cpos.x += 800.0f;
	}
	if (IS_KEY_DOWN('W')&&jump==0&&mPos.y==100.0f)
	{
		cpos.y += 500.0f;
		jump = 1;
	}
	if (fmod((mPos.z+3100.0f),6000.0f)==0)
	{
		gScene.Update();//板的移动
	}
	cpos.x = Clamp(cpos.x, -800.0f, 800.0f);
	VECTOR3 pos=slowmove(mPos, cpos, 400);
	mPos.x = Clamp(mPos.x,-800.0f ,800.0f);
    mMesh.SetPosition(pos.x, pos.y, pos.z);
}

VECTOR3 Sphere::slowmove(VECTOR3 begin, VECTOR3 end, float time)//time是球从起点到终点的时间，单位是毫秒 
{
	if (!(begin == end))
	{
		VECTOR3 pos;
		//--重置-----
		if (totaltime > time) { totaltime = 0; mPos = pos = end; }
		
		else if (totaltime > 0.9*time)//这里是防止偏向过大
			{
				totaltime = 0;
				mPos=pos = end;
			}
		else {
				pos.x = Lerp(begin.x, end.x, totaltime / time);
				pos.y = Lerp(begin.y, end.y, totaltime / time);
				pos.z = end.z;//z方向上不要插值
			}
		
		return pos;
	}
	return begin;
}
void Sphere::createtube()
{
	for (int i = 0; i < 3; i++)
	{
		int randx = rand() % 3;
		srand(time(NULL));
		for (int j = 0; j < 3; j++)
		{
			float tubez = Lerp(gScene.getObstaclez() + i * 1600.0f, gScene.getObstaclez() + (i + 1)*1600.0f, (j + 1) / 4.0f);
			eating[i][j].Init((randx - 1)*800.0f, tubez);
		}
	}
}
void Sphere::tuberender()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			eating[i][j].Render();
		}
	}
}


void Sphere::tubeupdate()
{
	int randx = rand() % 3;
	srand(time(NULL));
	if (eattube(fronttubepos, record) == true)
	{
		eating[fronttubepos][record].disappear();
		scores++;
		if (record == 2)record = 0;
		else record++;
	}
	if (mPos.z >= eating[fronttubepos][2].GetPosition().z)
	{
		for (int j = 0; j < 3; j++)
		{
			float tubez = Lerp(gScene.getObstaclez() + 2 * 1600.0f, 
				gScene.getObstaclez() + 3 * 1600.0f, (j + 1) / 4.0f);
			eating[fronttubepos][j].setPosition((randx - 1)*800.0f, tubez);
		}
		if (fronttubepos == 2)fronttubepos = 0;
		else fronttubepos++;
	}
}

int Sphere::getscores()
{
	return scores;
}

bool Sphere::eattube(int x, int y)//x为组数，y为这个组中的第几个
{
	
	if (mPos.x == eating[x][y].GetPosition().x&&
		mPos.y == eating[x][y].GetPosition().y&&eating[x][y].GetPosition().z-mPos.z<400.0f)
	{
		mciSendString("play d:\\eating.mp3", NULL, 0, NULL);
		
		return true;
	}
	else return false;
}