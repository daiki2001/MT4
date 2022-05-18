#include "./Header/Camera.h"

float Camera::targetRadius = 150.0f;
float Camera::longitude = Engine::Math::radian * (-90.0f);
float Camera::latitude = Engine::Math::radian * (0.0f);

Engine::Math::Vector3 Camera::pos = {};
Engine::Math::Vector3 Camera::target = { 0.0f, 50.0f, 0.0f };
Engine::Math::Vector3 Camera::upVec = { 0.0f, 1.0f, 0.0f };

DirectX::XMMATRIX Camera::CreateCamera(const XMVECTOR& pos, const XMVECTOR& target, const XMVECTOR& upVector)
{
	using namespace DirectX;

	XMVECTOR x, y, z;
	XMMATRIX mat = XMMatrixIdentity();

	z = target - pos;
	z = XMVector3Normalize(z);
	x = XMVector3Cross(XMVector3Normalize(upVector), z);
	x = XMVector3Normalize(x);
	y = XMVector3Cross(z, x);
	y = XMVector3Normalize(y);

	mat.r[0] = x;
	mat.r[1] = y;
	mat.r[2] = z;
	mat.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	mat = XMMatrixTranspose(mat);

	// ���_���W��-1���|�������W
	XMVECTOR reverseEyePosition = XMVectorNegate(pos);
	// �J�����̈ʒu���烏�[���h���_�ւ̃x�N�g���i�J�������W�n�j
	XMVECTOR tX = XMVector3Dot(x, reverseEyePosition); //X����
	XMVECTOR tY = XMVector3Dot(y, reverseEyePosition); //Y����
	XMVECTOR tZ = XMVector3Dot(z, reverseEyePosition); //Z����
	// ��̃x�N�g���ɂ܂Ƃ߂�
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
	// �r���[�s��ɕ��s�ړ�������ݒ�
	mat.r[3] = translation;

	return mat;
}

DirectX::XMMATRIX Camera::CreateCameraFix(const XMVECTOR& pos, const XMVECTOR& target, const XMVECTOR& upVector)
{
	using namespace DirectX;

	XMVECTOR x, y, z, d;
	XMMATRIX mat = XMMatrixIdentity();

	y = XMVector3Normalize(upVector);
	d = target - pos;
	d = XMVector3Normalize(d);
	x = XMVector3Cross(y, d);
	x = XMVector3Normalize(x);
	z = XMVector3Cross(x, y);
	z = XMVector3Normalize(z);

	mat.r[0] = x;
	mat.r[1] = y;
	mat.r[2] = z;
	mat.r[3] = -pos;

	mat.r[0].m128_f32[3] = 0;
	mat.r[1].m128_f32[3] = 0;
	mat.r[2].m128_f32[3] = 0;
	mat.r[3].m128_f32[3] = 1;

	return mat;
}
