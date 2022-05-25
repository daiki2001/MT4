#pragma once
#include "./Header/BaseScene.h"
#include "./Math/EngineMath.h"

class TestScene : public BaseScene
{
public: // �G�C���A�X
	using MathObject = Engine::Math::MathObject;

public: // �萔
	static const float gravity;       //�d��
	static const float airResistance; //��C��R

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
