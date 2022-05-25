#pragma once
#include <DirectXMath.h>

// �C���N���[�h�t�@�C��
#include "./Math/Vector3.h"
#include "./Math/Matrix4.h"
#include "./Math/Easing.h"

/*�萔�E�\����*/
namespace Engine
{
namespace Math
{
static const float radian = DirectX::XM_PI / 180.0f;

// �����v�Z�ɕK�v�ȕϐ�
struct MathObject
{
	float friction = 0.0f;    //���C�W��
	float mass = 0.0f;        //����
	float restitution = 1.0f; //�����W��

	Engine::Math::Vector3 initPos = {}; //�v���C���[�̏������W
	Engine::Math::Vector3 initVel = {}; //�v���C���[�̏����x

	Engine::Math::Vector3 pos = initPos;                   //�v���C���[�̍��W
	Engine::Math::Matrix4 rota = Engine::Math::Identity(); //�v���C���[�̉�]�s��
	Engine::Math::Vector3 vel = {};                        //�v���C���[�̑��x
	Engine::Math::Vector3 accel = {};                      //�v���C���[�̉����x
};
} //Math
} //Engine

/*�֐�*/
namespace Engine
{
namespace Math
{
bool operator==(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b);
bool operator!=(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b);

// �Փˎ��̕����v�Z
int CollisionPhysics(MathObject* obj1, MathObject* obj2);
} //Math
} //Engine
