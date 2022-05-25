#pragma once
#include <DirectXMath.h>

// インクルードファイル
#include "./Math/Vector3.h"
#include "./Math/Matrix4.h"
#include "./Math/Easing.h"

/*定数・構造体*/
namespace Engine
{
namespace Math
{
static const float radian = DirectX::XM_PI / 180.0f;

// 物理計算に必要な変数
struct MathObject
{
	float friction = 0.0f;    //摩擦係数
	float mass = 0.0f;        //質量
	float restitution = 1.0f; //反発係数

	Engine::Math::Vector3 initPos = {}; //プレイヤーの初期座標
	Engine::Math::Vector3 initVel = {}; //プレイヤーの初速度

	Engine::Math::Vector3 pos = initPos;                   //プレイヤーの座標
	Engine::Math::Matrix4 rota = Engine::Math::Identity(); //プレイヤーの回転行列
	Engine::Math::Vector3 vel = {};                        //プレイヤーの速度
	Engine::Math::Vector3 accel = {};                      //プレイヤーの加速度
};
} //Math
} //Engine

/*関数*/
namespace Engine
{
namespace Math
{
bool operator==(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b);
bool operator!=(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b);

// 衝突時の物理計算
int CollisionPhysics(MathObject* obj1, MathObject* obj2);
} //Math
} //Engine
