#pragma once
#include "./Header/BaseScene.h"
#include "./Math/EngineMath.h"

// 物理計算に必要な変数
struct MathObject
{
	float friction = 0.0f; //摩擦係数
	float mass = 0.0f;     //プレイヤーの質量

	Engine::Math::Vector3 initPos = {}; //プレイヤーの初期座標
	Engine::Math::Vector3 initVel = {}; //プレイヤーの初速度

	Engine::Math::Vector3 pos = initPos;                 //プレイヤーの座標
	Engine::Math::Matrix4 rota = Engine::Math::Identity(); //プレイヤーの回転行列
	Engine::Math::Vector3 vel = {};                      //プレイヤーの速度
	Engine::Math::Vector3 accel = {};                    //プレイヤーの加速度
};

class TestScene : public BaseScene
{
public: // エイリアス
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using Matrix4 = Engine::Math::Matrix4;
	using Vector3 = Engine::Math::Vector3;

public: // 定数
	static const float sphereRadius; //球体オブジェクトの半径

	static const float gravity;       //重力
	static const float airResistance; //空気抵抗
	static const float friction;      //摩擦係数

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
