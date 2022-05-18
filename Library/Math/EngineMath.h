#pragma once
#include <DirectXMath.h>

// インクルードファイル
#include "./Math/Vector3.h"
#include "./Math/Matrix4.h"
#include "./Math/Easing.h"

/*定数*/
namespace Engine
{
namespace Math
{
static const float radian = DirectX::XM_PI / 180.0f;
} //Math
} //Engine

/*関数*/
namespace Engine
{
namespace Math
{
bool operator==(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b);
bool operator!=(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b);

// 運動量保存則
int ConservationOfMomentum(Vector3* vel1, const float mass1, Vector3* vel2, const float mass2);
} //Math
} //Engine
