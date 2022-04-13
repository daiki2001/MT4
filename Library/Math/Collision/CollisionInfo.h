#pragma once
#include "./Math/EngineMath.h"

class BaseCollider;

struct CollisionInfo
{
private: // �G�C���A�X
	using Vector3 = EngineMath::Vector3;

public:
	CollisionInfo(BaseCollider* collider, const Vector3& inter)
	{
		this->collider = collider;
		this->inter = inter;
	}

	BaseCollider* collider = nullptr; //�Փˑ���̃R���C�_�[
	Vector3 inter;                    //�Փ˓_
};