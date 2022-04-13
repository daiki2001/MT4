#pragma once
#include "./Header/BaseScene.h"
#include "./Math/EngineMath.h"
#include "./Math/Collision/Collision.h"
#include "Player.h"

class TestScene : public BaseScene
{
public: // �G�C���A�X
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using Vector3 = EngineMath::Vector3;

public: // �萔
	static const float sphereRadius; //���̃I�u�W�F�N�g�̔��a
	static const float planeRadius;  //���ʃI�u�W�F�N�g�̑傫��
	static const Vector3 tPos1;      //�O�p�`�I�u�W�F�N�g�̒��_1
	static const Vector3 tPos2;      //�O�p�`�I�u�W�F�N�g�̒��_2
	static const Vector3 tPos3;      //�O�p�`�I�u�W�F�N�g�̒��_3

	static const XMFLOAT4 color[3];

	static const float rayLength; //���C�̒���

	static const float triangleAngle;
	static const float triangleScale;

public: // �����o�֐�
	TestScene(SceneChenger* sceneChenger);

	void Init() override;
	void Update() override;
	void Draw() override;

private: // �����o�ϐ�
	int background; //�w�i�摜

	// �I�u�W�F�N�g�̐���
	int sphere;   //���̃I�u�W�F�N�g
	int plane;    //���ʃI�u�W�F�N�g
	int triangle; //�O�p�`�I�u�W�F�N�g
	int ray;      //���C�`��p�I�u�W�F�N�g

	Player* player;
	Vector3 playerPos;

	Vector3 rayPos;
	Vector3 rayDir;
	Vector3 rayAngle;
	XMMATRIX rayRot;
	float rayDistance;

	Vector3 spherePos;

	Vector3 planePos;
	XMMATRIX planeRot;
	float planeScale;
	Vector3 planeToRayHitPos;

	Vector3 trianglePos;
	XMMATRIX triangleRot;
	XMMATRIX triangleInvRot;
	XMMATRIX triangleMatrix;

	Sphere playerCollision;
	Sphere sphereCollision;
	Plane planeCollision;
	Triangle triangleCollision;
	Ray rayCollision;

	bool isCameraMove;
	bool isRay;
};
