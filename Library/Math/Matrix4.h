#pragma once
#include <DirectXMath.h>
#include "Vector3.h"

namespace EngineMath
{
// DirectX::XMMATRIX �^�� Matrix4 �^�Ƃ��Ĉ��� ���݊����̊m��&�g���̂���
using Matrix4 = DirectX::XMMATRIX;

// �P�ʍs������߂�
Matrix4 Identity();

// �t�s������߂�
Matrix4 Inverse(const Matrix4& m);

// �g��k���s��̐ݒ�
Matrix4 scale(const Vector3& s);

// ��]�s��̐ݒ�
Matrix4 rotateX(float angle);
Matrix4 rotateY(float angle);
Matrix4 rotateZ(float angle);

// ���s�ړ��s��̍쐬
Matrix4 translate(const Vector3& t);

// ���W�ϊ��i�x�N�g���ƍs��̊|���Z������j
Vector3 transform(const Vector3& v, const Matrix4& m);

// 2�����Z�q�I�[�o�[���[�h
Vector3 operator * (const Vector3& v, const Matrix4& m);
} //EngineMath
