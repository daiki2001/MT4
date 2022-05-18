#pragma once
#include "./Header/BaseScene.h"
#include "./Math/EngineMath.h"

// �����v�Z�ɕK�v�ȕϐ�
struct MathObject
{
	float friction = 0.0f; //���C�W��
	float mass = 0.0f;     //�v���C���[�̎���

	Engine::Math::Vector3 initPos = {}; //�v���C���[�̏������W
	Engine::Math::Vector3 initVel = {}; //�v���C���[�̏����x

	Engine::Math::Vector3 pos = initPos;                 //�v���C���[�̍��W
	Engine::Math::Matrix4 rota = Engine::Math::Identity(); //�v���C���[�̉�]�s��
	Engine::Math::Vector3 vel = {};                      //�v���C���[�̑��x
	Engine::Math::Vector3 accel = {};                    //�v���C���[�̉����x
};

class TestScene : public BaseScene
{
public: // �G�C���A�X
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using Matrix4 = Engine::Math::Matrix4;
	using Vector3 = Engine::Math::Vector3;

public: // �萔
	static const float sphereRadius; //���̃I�u�W�F�N�g�̔��a

	static const float gravity;       //�d��
	static const float airResistance; //��C��R
	static const float friction;      //���C�W��

public: // �����o�֐�
	TestScene(SceneChenger* sceneChenger);

	void Init() override;
	void Update() override;
	void Draw() override;

private: // �����o�ϐ�
	int background; //�w�i�摜

	MathObject playerObject; //�v���C���[�̃I�u�W�F�N�g
	int playerDrawObject;    //�v���C���[�̕`��I�u�W�F�N�g

	MathObject cubeObject; //�����̂̃I�u�W�F�N�g
	int cubeDrawObject;    //�����̂̕`��I�u�W�F�N�g

	bool isHit; //�Փ˂̗L��
};
