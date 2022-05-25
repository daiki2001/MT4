#pragma once
#include "./Header/BaseScene.h"
#include "./Math/EngineMath.h"

class TestScene : public BaseScene
{
public: // エイリアス
	using MathObject = Engine::Math::MathObject;

public: // 定数
	static const float gravity;       //重力
	static const float airResistance; //空気抵抗

public: // メンバ関数
	TestScene(SceneChenger* sceneChenger);

	void Init() override;
	void Update() override;
	void Draw() override;

private: // メンバ変数
	int background; //背景画像

	MathObject playerObject; //プレイヤーのオブジェクト
	int playerDrawObject;    //プレイヤーの描画オブジェクト

	MathObject cubeObject; //立方体のオブジェクト
	int cubeDrawObject;    //立方体の描画オブジェクト

	bool isHit; //衝突の有無
};
