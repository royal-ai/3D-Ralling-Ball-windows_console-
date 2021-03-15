#pragma once
namespace gameplay
{
	class Sphere {
	public:
		Sphere();
		VECTOR3 GetPosition();
		void Render();
		void Update();
		void Init();
		VECTOR3 slowmove(VECTOR3 begin, VECTOR3 end, float speed);
		void UpdateandRender();
		
		bool deadCondition();
		void createtube();
		void tuberender();
		void tubeupdate();
		int getscores();
		bool eattube(int x, int y);
	private:
		VECTOR3 mPos;
		VECTOR3 cpos;
		VECTOR3 mLookat;
		IMesh		mMesh;
		IPicture		mTexture_Common;
		DirectionalLight	mSceneLight;//³¡
		Material								mMat_Common;
		Material								mMat_Red;
		IPicture		mTexture_End;//ÉùÃ÷
		bool jump=0;
		bool down = 0;
		float totaltime=0;
		void translationandrotation();
		tube eating[3][3];
		int fronttubepos = 0;
		int record = 0;
		int scores = 0;
	};
}