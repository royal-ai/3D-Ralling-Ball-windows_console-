#pragma once
class environment {
public:
	void Init();
	void Update();
	void Render();
private:
	Material				mDefaultMaterial;
	IPicture					mSkyTexture;
	IMesh					mSkyModel;
	DirectionalLight	mSceneLight;//≥°æ∞π‚œﬂ
};