#include"game.h"

using namespace gameplay;
Scene::Scene() {
	mMat_Common.ambient = { 1.0f,1.0f,1.0f };
	mMat_Common.diffuse = { 0.8f,0.8f,0.8f };
	mMat_Common.specular = { 0.0f,0.0f,0.0f };
}
void Scene::Init() {
	mMeshPlane.LoadFile_OBJ("picture/plane1.obj");
	mTexture_Board.LoadPPM("picture/chicken.ppm");
	mMeshNextPlane.LoadFile_OBJ("picture/plane1.obj");
	mPos = VECTOR3(0, 0, 0);
	nPos= VECTOR3(0, 0, 6000.0f);
	mMeshPlane.SetMaterial(mMat_Common);
	mMeshPlane.SetTexture(&mTexture_Board);
	mMeshPlane.SetPosition(mPos.x, mPos.y, mPos.z);
	mMeshNextPlane.SetMaterial(mMat_Common);
	mMeshNextPlane.SetTexture(&mTexture_Board);
	//boolObstacle[20] = { 0 };
	for (int i = 0; i < 20; i++)
	{
		boolObstacle[i] = 0;
	}
	mMeshNextPlane.SetPosition(nPos.x, nPos.y, nPos.z);//两个板的初始化
	for (int i = 0; i < 20; i++)//障碍物的初始化
	{
		if (i >= 0 && i < 4)
		{
			numObstacle[i].Init(longjump);
		}
		if (i >= 4 && i < 12)
		{   
			numObstacle[i].Init(pyramid);
		}
		if (i >= 12 && i < 20)
		{
			numObstacle[i].Init(jump);
		}

	}
	for (int i = 0; i < 4; i++) //产生障碍物和初始化其位置，类型信息
	{
		posObstacle[i] = -3000.0f + (i+1)*Interval;//四轮障碍物的初始化
	    VECTOR2 inf=	obstacleProduce(posObstacle[i]);
		//在相应的z轴位置上初始化障碍物，并且把相应的障碍物信息传递给inf坐标
		infObstacle[i] = inf;
	}

}
VECTOR2 Scene::obstacleProduce(float z)
{
	int countjump = 1;//count is the number of jump be set 
	int countpyramid = 1;//count is the number of jump be set 
	VECTOR2 inf;
	int numpyramid, numjump, posx;
	srand(time(NULL));
	numpyramid = rand() % 3;//the number of pyramid
	numjump = rand() % 2;//the number of jump
	posx = rand() % 3;//0 is left ,1 is middle, 2 is right
	switch (numpyramid) {
	case 0://longjump
		for (int i = 0; i < 4; i++) {
			if (boolObstacle[i] == 0) //为零则可以调用
			{
				numObstacle[i].setPosition(0.0f, z);
				boolObstacle[i] = 1;
				inf = { float( posx),0.0f };
				break;
			}
		}
		break;
	case 1://single pyramid
		
		for (int i = 4; i < 12; i++) //pyramid
		{
			if (boolObstacle[i] == 0) {
				numObstacle[i].setPosition((posx-1)*800.0f, z);
				boolObstacle[i] = 1;
				inf = { float(posx),1.0f };
				break;
			}
		}
		for (int i = 12; i < 20; i++) //doublejump
		{
			if (boolObstacle[i] == 0&&countjump==1) 
			{
				if (posx == 0) {
					numObstacle[i].setPosition(0.0f, z);
					
				}
				else if (posx == 1) {
					numObstacle[i].setPosition(-800.0f, z);
					
				}
				else {
					numObstacle[i].setPosition(-800.0f, z);
					
				}
				boolObstacle[i]=1;
				countjump++;
			}
			if (boolObstacle[i] == 0 && countjump == 2)
			{
				if (posx == 0)
				{
					numObstacle[i].setPosition(800.0f, z);

				}
				else if (posx == 1)
				{
					numObstacle[i].setPosition(800.0f, z);

				}
				else
				{
					numObstacle[i].setPosition(0.0f, z);

				}
				boolObstacle[i] = 1;
				break;
			}
		}
		break;
	case 2://double pyramid
		for (int i = 4; i < 12; i++) //doublepyramid
		{
			if (boolObstacle[i] == 0 && countpyramid == 1)
			{
				if (posx == 0) {
					numObstacle[i].setPosition(0.0f, z);

				}
				else if (posx == 1) {
					numObstacle[i].setPosition(-800.0f, z);

				}
				else {
					numObstacle[i].setPosition(-800.0f, z);

				}
				boolObstacle[i] = 1;
				countpyramid++;
			}
			if (boolObstacle[i] == 0 && countpyramid == 2)
			{
				if (posx == 0)
				{
					numObstacle[i].setPosition(800.0f, z);

				}
				else if (posx == 1)
				{
					numObstacle[i].setPosition(800.0f, z);

				}
				else
				{
					numObstacle[i].setPosition(0.0f, z);

				}
				boolObstacle[i] = 1;
				break;
			}
		}
		if (numjump == 1)//single jump
		{
			for (int i = 12; i < 20; i++) //pyramid
			{
				if (boolObstacle[i] == 0) {
					numObstacle[i].setPosition((posx - 1)*800.0f, z);
					boolObstacle[i] = 1;
					inf = { float(posx),3.0f };
					break;
					
				}
			}
		}
		else {
			inf = { float(posx),2.0f };
		}
		break;
	}
	return inf;
}
/*void Scene::UpdateandRender()
{
}*/
VECTOR2 Scene::getInfObastacle()
{
	return infObstacle[0];
}
float Scene::getObstaclez()
{
	return posObstacle[0];
}
void Scene::Render() 
{
	
	gRenderer.RenderMesh(mMeshPlane);
	gRenderer.RenderMesh(mMeshNextPlane);
	obstacleUpdateandRender();
}
VECTOR3 Scene::GetPosition()
{
	return mPos;
}
void Scene::Update()
{
	if (nPos.z > mPos.z)
	{
		mPos.z += 12000.0f;
		mMeshPlane.SetPosition(mPos.x, mPos.y, mPos.z);
	}
	else {
		nPos.z += 12000.0f;
		mMeshNextPlane.SetPosition(nPos.x, nPos.y, nPos.z);
	}
}
void Scene::obstacleUpdateandRender() 
{
	if (gPlayer.endanimation() == true && gBall.deadCondition() == false)
	{
		if (gCamera.GetPosition().z > posObstacle[0])
		{
			for (int i = 0; i < 20; i++)
			{
				if (numObstacle[i].GetPosition().z == posObstacle[0])
				{
					boolObstacle[i] = 0;
				}
			}
			for (int i = 0; i < 3; i++)
			{
				infObstacle[i] = infObstacle[i + 1];//障碍物信息的传递
				posObstacle[i] += Interval;//障碍物位置的传递
			}
			posObstacle[3] += Interval;
			infObstacle[3] = obstacleProduce(posObstacle[3]);//生成一个新的障碍物，并放在视野外
		}
	}
	//****render****
	for (int i = 0; i < 20; i++)
	{
		if (boolObstacle[i] == 1) {
			numObstacle[i].Render();
		}

	}
}