#pragma once
#include "CollisionPrimitive.h"
#include "./Math/EngineMath.h"

namespace Collision
{
using XMMATRIX = DirectX::XMMATRIX;
using Vector3 = Engine::Math::Vector3;

// ���̓��m�̓����蔻��
bool IsSphereToSphereCollision(const Sphere& sphere1, const Sphere& sphere2, Vector3* inter = nullptr);

// ���̂ƕ��ʂ̓����蔻��
bool IsSphereToPlaneCollision(const Sphere& sphere, const Plane& plane);

// ���̂ƎO�p�`�̓����蔻��
bool IsSphereToTriangleCollision(const Sphere& sphere, const Triangle& triangle);

// ���C�Ƌ��̂̓����蔻��
bool IsRayToSphereCollision(const Ray& ray, const Sphere& sphere, float* distance = nullptr, Vector3* inter = nullptr);

// ���C�ƕ��ʂ̓����蔻��
bool IsRayToPlaneCollision(const Ray& ray, const Plane& plane, float* distance = nullptr, Vector3* inter = nullptr);

// ���C�ƎO�p�`�̓����蔻��
bool IsRayToTriangleCollision(const Ray& ray, const Triangle& triangle, float* distance = nullptr, Vector3* inter = nullptr);

// �����̓��m�̓����蔻��
bool IsOBBToOBBCollision(const Vector3& pos1, const XMMATRIX& rotation1, const Vector3& scale1, const Vector3& pos2, const XMMATRIX& rotation2, const Vector3& scale2);

// ���̃X�C�[�v
// �������Ă�����Փ˂������Ԃ�0~1�̊ԂŕԂ��A�������Ă��Ȃ�������-1��Ԃ�
float sphereSwept(const Vector3& pos1, const Vector3& speed1, const float& r1, const Vector3& pos2, const Vector3& speed2, const float& r2);

// �Փ˗\��
// ���̏ꍇ�́AVector3(����, ����, ���s).Length() / 2������Ύg���܂�
bool IsPredictCollisionBall(const Vector3& pos1, const Vector3& speed1, const float& r1, const Vector3& pos2, const Vector3& speed2, const float& r2);
} //Collision
