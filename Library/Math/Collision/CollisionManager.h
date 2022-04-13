#pragma once
#include <forward_list>

class BaseCollider;

class CollisionManager final
{
public: // �V���O���g����
	static CollisionManager* Get();
private:
	CollisionManager() {}
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() {}
	CollisionManager operator=(const CollisionManager&) = delete;

public: // �����o�֐�
	// �R���C�_�[�̒ǉ�
	inline void AddCollider(BaseCollider* collider) { colliders.push_front(collider); }
	// �R���C�_�[�̍폜
	inline void RemoveCollider(BaseCollider* collider) { colliders.remove(collider); }

	// �S�Ă̏Փ˃`�F�b�N
	void CheckAllCollisions();
private:
	// ���̓��m�̏Փˎ�����
	void SphereSphere(BaseCollider* col1, BaseCollider* col2);

private: // �����o�ϐ�
	std::forward_list<BaseCollider*> colliders; //�R���C�_�[�̃��X�g
};
