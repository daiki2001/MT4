#pragma once
#include <DirectXMath.h>

namespace Engine
{
namespace Math
{
class Vector3 : public DirectX::XMFLOAT3
{
public:
	// �R���X�g���N�^
	Vector3();                          //��x�N�g���Ƃ��Đ���
	Vector3(float x, float y, float z); //x�����Ay�����Az�������w�肵�Đ���
	Vector3(XMFLOAT3 v);                //DirectX::XMFLOAT3�ƌ݊������������邽��

	// �����o�֐�
	float Length() const;                  //�m����(����)�����߂�
	float LengthSquared() const;           //�m����(����)�����߂�
	Vector3 Normalize() const;             //���K������
	float Dot(const Vector3& v) const;     //���ς����߂�
	Vector3 Cross(const Vector3& v) const; //�O�ς����߂�

	// �P�����Z�q�I�[�o�[���[�h
	Vector3 operator+() const;
	Vector3 operator-() const;

	// ������Z�q�I�[�o�[���[�h
	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);

	// ��r���Z�q�I�[�o�[���[�h
	bool operator==(const Vector3& v);
	bool operator!=(const Vector3& v);

	inline static Vector3 Zero() { return { 0.0f, 0.0f, 0.0f }; }
};

// 2�����Z�q�I�[�o�[���[�h
// �������Ȉ���(�����̌^�Ə���)�̃p�^�[���ɑΉ����邽�߁A�ȉ��̂悤�ɏ������Ă���
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);
} //Math
} //Engine
