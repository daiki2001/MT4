#pragma once
#include "CollisionTypes.h"
#include "./Header/DirectDrawing.h"
#include "./Math/Collision/CollisionInfo.h"

class BaseCollider
{
private: // �G�C���A�X
	using ShapeType = Collision::ShapeType;

public: // �����o�֐�
	BaseCollider() {}
	virtual ~BaseCollider() {}

	// �X�V
	virtual void Update() = 0;
	// �`��^�C�v�̎擾
	inline ShapeType GetShapeType() const { return shapeType; }

	// �I�u�W�F�N�g�̃f�[�^�̐ݒ�
	inline void SetObject(Object* objectData) { this->objectData = objectData; }
	// �I�u�W�F�N�g�̃f�[�^�̎擾
	inline Object* GetObject() const { return objectData; }
	// �Փˎ��R�[���o�b�N�֐�
	inline void OnCollision(const CollisionInfo& info) { objectData->OnCollision(info); }

protected: // �����o�ϐ�
	Object* objectData = nullptr;                            //�I�u�W�F�N�g�̃f�[�^
	ShapeType shapeType = ShapeType::SHAPE_UNKNOWN; //�`��^�C�v
};
