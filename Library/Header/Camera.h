#pragma once
#include "./Math/EngineMath.h"

class Camera
{
private: // �G�C���A�X
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
	using Vector3 = Engine::Math::Vector3;

public: // �ÓI�����o�ϐ�
	static float targetRadius; //�����_����̋���
	static float longitude;    //�o�x
	static float latitude;     //�ܓx

	static Vector3 pos;    //�J�����̈ʒu
	static Vector3 target; //�J�����̒����_
	static Vector3 upVec;        //��x�N�g��

	/*�����o�֐�*/
public:
	Camera() {}
	~Camera() {}

	// �L�����N�^�p���s��̐���
	static DirectX::XMMATRIX CreateCamera(const XMVECTOR& pos, const XMVECTOR& target, const XMVECTOR& upVector);
	// �L�����N�^�p�������s��̐���
	static DirectX::XMMATRIX CreateCameraFix(const XMVECTOR& pos, const XMVECTOR& target, const XMVECTOR& upVector);
};
