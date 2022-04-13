#pragma once
#include <DirectXMath.h>

// インクルードファイル
#include "./Math/Vector3.h"
#include "./Math/Matrix4.h"
#include "./Math/Easing.h"

/*定数*/
namespace EngineMath
{
static const float radian = DirectX::XM_PI / 180.0f;
} //EngineMath

/*オペレーター演算子オーバーロード*/
namespace EngineMath
{
bool operator==(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b);
bool operator!=(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b);
} //EngineMath
