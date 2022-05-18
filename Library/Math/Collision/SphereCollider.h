#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"
#include "./Math/EngineMath.h"

class SphereCollider : public BaseCollider
{
private: // �G�C���A�X
	using Vector3 = Engine::Math::Vector3;

public: // �����o�֐�
	SphereCollider(const Vector3& offset = { 0.0f, 0.0f, 0.0f }, const float& radius = 1.0f);

	// �X�V
	void Update() override;

	// ���a�̐ݒ�
	inline void SetRadius(const float& radius) { this->radius = radius; }

public: // �����o�ϐ�
	Sphere sphere;
private:
	Vector3 offset; //�I�u�W�F�N�g���S����̃I�t�Z�b�g
	float radius; //���a
};
