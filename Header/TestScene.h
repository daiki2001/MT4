#pragma once
#include "./Header/BaseScene.h"
#include "./Math/EngineMath.h"

class TestScene : public BaseScene
{
public: // �G�C���A�X
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using Matrix4 = EngineMath::Matrix4;
	using Vector3 = EngineMath::Vector3;

public: // �萔
	static const float sphereRadius; //���̃I�u�W�F�N�g�̔��a

	static const float gravity; //�d��

public: // �����o�֐�
	TestScene(SceneChenger* sceneChenger);

	void Init() override;
	void Update() override;
	void Draw() override;

private: // �����o�ϐ�
	int background; //�w�i�摜

	int playerObject;    //�v���C���[�̕`��I�u�W�F�N�g

	Vector3 playerPos;      //�v���C���[�̍��W
	Matrix4 playerRota;     //�v���C���[�̉�]�s��
	Vector3 playerVel;      //�v���C���[�̑��x
	Vector3 playerAccel;    //�v���C���[�̉����x
	Vector3 playerAddForce; //�v���C���[�ɉ�����

	Vector3 playerInitPos;  //�v���C���[�̏������W
	Vector3 playerInitVel;  //�v���C���[�̏����x
};
