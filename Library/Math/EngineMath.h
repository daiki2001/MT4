#pragma once
#include <DirectXMath.h>

// �C���N���[�h�t�@�C��
#include "./Math/Vector3.h"
#include "./Math/Matrix4.h"
#include "./Math/Easing.h"

/*�萔*/
namespace EngineMath
{
static const float radian = DirectX::XM_PI / 180.0f;
} //EngineMath

/*�I�y���[�^�[���Z�q�I�[�o�[���[�h*/
namespace EngineMath
{
bool operator==(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b);
bool operator!=(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b);
} //EngineMath
