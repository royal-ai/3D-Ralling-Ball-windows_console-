
class Scene {
public:
	Scene();
	VECTOR3 GetPosition();
	void Render();
	void Update();
	void Init();
	void obstacleUpdateandRender();
	VECTOR2 obstacleProduce(float z);
	void UpdateandRender();
	VECTOR2 getInfObastacle();
	float getObstaclez();
private:
	VECTOR3 mPos;
	VECTOR3 nPos;
	VECTOR3 mLookat;
	IMesh       mMeshNextPlane;
	IMesh		mMeshPlane;
	IPicture		mTexture_Board;
	const float Interval=2500.0f;
	DirectionalLight	mSceneLight;//场
	Material								mMat_Common;
	obstacle numObstacle[20];
	bool boolObstacle[20] = { 0 };
	float posObstacle[4];
	VECTOR2 infObstacle[4];//y是0代表longjump，1代表单pyramid和manjump，2代表doublepyramid，nojump
	//3，代表doublepyramid，singlejump
};
