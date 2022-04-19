#pragma once
#include "./Header/BaseScene.h"
#include "./Math/EngineMath.h"

class TestScene : public BaseScene
{
public: // エイリアス
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using Matrix4 = EngineMath::Matrix4;
	using Vector3 = EngineMath::Vector3;

public: // 定数
	static const float sphereRadius; //球体オブジェクトの半径

	static const float gravity; //重力

public: // メンバ関数
	TestScene(SceneChenger* sceneChenger);

	void Init() override;
	void Update() override;
	void Draw() override;

private: // メンバ変数
	int background; //背景画像

	int playerObject;    //プレイヤーの描画オブジェクト

	Vector3 playerPos;      //プレイヤーの座標
	Matrix4 playerRota;     //プレイヤーの回転行列
	Vector3 playerVel;      //プレイヤーの速度
	Vector3 playerAccel;    //プレイヤーの加速度
	Vector3 playerAddForce; //プレイヤーに加わる力

	Vector3 playerInitPos;  //プレイヤーの初期座標
	Vector3 playerInitVel;  //プレイヤーの初速度
};
