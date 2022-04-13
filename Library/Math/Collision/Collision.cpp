#include "Collision.h"

namespace
{
void ClosestPtPoint2Triangle(const EngineMath::Vector3& point, const Triangle& triangle, EngineMath::Vector3* closest)
{
	using namespace EngineMath;

	// point��p0�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	Vector3 p0_p1 = triangle.p1 - triangle.p0;
	Vector3 p0_p2 = triangle.p2 - triangle.p0;
	Vector3 p0_pt = point - triangle.p0;

	float d1 = p0_p1.Dot(p0_pt);
	float d2 = p0_p2.Dot(p0_pt);

	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		*closest = triangle.p0;
		return;
	}

	// point��p1�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	Vector3 p1_pt = point - triangle.p1;

	float d3 = p0_p1.Dot(p1_pt);
	float d4 = p0_p2.Dot(p1_pt);

	if (d3 >= 0.0f && d4 <= d3)
	{
		*closest = triangle.p1;
		return;
	}

	// point��p0_p1�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p0_p1��ɑ΂���ˉe��Ԃ�
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		*closest = triangle.p0 + v * p0_p1;
		return;
	}

	// point��p2�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	Vector3 p2_pt = point - triangle.p2;

	float d5 = p0_p1.Dot(p2_pt);
	float d6 = p0_p2.Dot(p2_pt);

	if (d6 >= 0.0f && d5 <= d6)
	{
		*closest = triangle.p2;
		return;
	}

	// point��p0_p2�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p0_p2��ɑ΂���ˉe��Ԃ�
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		*closest = triangle.p0 + w * p0_p2;
		return;
	}

	// point��p1_p2�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p1_p2��ɑ΂���ˉe��Ԃ�
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		return;
	}

	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;
}
}

namespace Collision
{
bool IsSphereToSphereCollision(const Sphere& sphere1, const Sphere& sphere2, Vector3* inter)
{
	static Vector3 distance{}; //���S�_�Ԃ̋���
	static bool result = false;

	distance = sphere2.center - sphere1.center;

	result = distance.Length() < (sphere1.radius + sphere2.radius);

	if (result)
	{
		if (inter != nullptr)
		{
			// A�̔��a��0�̎����W��B�̒��S B�̔��a��0�̎����W��A�̒��S �ƂȂ�悤�⊮
			float t = sphere2.radius / (sphere1.radius + sphere2.radius);
			*inter = EngineMath::Lerp(sphere1.center, sphere2.center, t);
		}
	}

	return result;
}

bool IsSphereToPlaneCollision(const Sphere& sphere, const Plane& plane)
{
	static float distance{}; //���̂̒��S���W�ƕ��ʂ̋���
	distance = sphere.center.Dot(plane.normal) - plane.distance;
	if (fabsf(distance) > sphere.radius)
	{
		return false;
	}

	return true;
}

bool IsSphereToTriangleCollision(const Sphere& sphere, const Triangle& triangle)
{
	Vector3 p;
	ClosestPtPoint2Triangle(sphere.center, triangle, &p);
	Vector3 v = p - sphere.center;

	if (v.LengthSquared() > sphere.radius * sphere.radius)
	{
		return false;
	}

	return true;
}

bool IsRayToSphereCollision(const Ray& ray, const Sphere& sphere, float* distance, Vector3* inter)
{
	Vector3 m = ray.start - sphere.center;
	float b = m.Dot(ray.dir);
	float c = m.Dot(m) - sphere.radius * sphere.radius;

	// ���C�̎n�_�����̂ɓ������Ė����āA���C�����̂ɑ΂��ė���Ă����������w���Ă���ꍇ�͓�����Ȃ�
	if ((c > 0.0f) && (b > 0.0f))
	{
		return false;
	}

	float discr = b * b - c;
	// ���ʎ����}�C�i�X�Ȃ�A���C�͋��̂�����Ă���
	if (discr < 0.0f)
	{
		return false;
	}

	// ��������ꏊt�����߂�
	float t = -b - sqrtf(discr);
	// t���}�C�i�X�̏ꍇ�A�n�_�����̂̓����ɂ���
	if (t < 0)
	{
		t = 0.0f;
	}

	// ��������������
	if (distance != nullptr)
	{
		*distance = t;
	}
	// ��_�����߂�
	if (inter != nullptr)
	{
		*inter = ray.start + t * ray.dir;
	}

	return true;
}

bool IsRayToPlaneCollision(const Ray& ray, const Plane& plane, float* distance, Vector3* inter)
{
	// �ʖ@���ƃ��C�̕����x�N�g���̓���
	float d1 = plane.normal.Dot(ray.dir);
	// �ʖ@���ƃ��C�̎n�_���W�̓���
	float d2 = plane.normal.Dot(ray.start);
	// �n�_�ƕ��ʂ̋���(���ʂ̖@������)
	float dist = d2 - plane.distance;
	// �n�_�ƕ��ʂ̋���(���C����)
	float t = dist / -d1;
	// ��_���n�_�����ɂ��邩�瓖����Ȃ�
	if (t < 0)
	{
		return false;
	}

	// ��������������
	if (distance != nullptr)
	{
		*distance = t;
	}
	// ��_�����߂�
	if (inter != nullptr)
	{
		*inter = ray.start + t * ray.dir;
	}

	return true;
}

bool IsRayToTriangleCollision(const Ray& ray, const Triangle& triangle, float* distance, Vector3* inter)
{
	// �O�p�`������Ă��镽�ʂ��Z�o
	Plane plane;
	Vector3 interPlane;
	plane.normal = triangle.normal;
	plane.distance = triangle.normal.Dot(triangle.p0);
	// ���C�ƕ��ʂ��������Ė��������瓖�����Ė���
	if (IsRayToPlaneCollision(ray, plane, distance, &interPlane) == false)
	{
		return false;
	}

	const float epsilon = 1.0e-5f; //�덷�z���p�̔����Ȓl
	Vector3 m;

	// ��p0_p1
	Vector3 pt_p0 = triangle.p0 - interPlane;
	Vector3 p0_p1 = triangle.p1 - triangle.p0;
	m = pt_p0.Cross(p0_p1);

	if (m.Dot(triangle.normal) < -epsilon)
	{
		return false;
	}

	// ��p1_p2
	Vector3 pt_p1 = triangle.p1 - interPlane;
	Vector3 p1_p2 = triangle.p2 - triangle.p1;
	m = pt_p1.Cross(p1_p2);

	if (m.Dot(triangle.normal) < -epsilon)
	{
		return false;
	}

	// ��p2_p0
	Vector3 pt_p2 = triangle.p2 - interPlane;
	Vector3 p2_p0 = triangle.p0 - triangle.p2;
	m = pt_p2.Cross(p2_p0);

	if (m.Dot(triangle.normal) < -epsilon)
	{
		return false;
	}

	if (inter != nullptr)
	{
		*inter = interPlane;
	}

	return true;
}

bool IsOBBToOBBCollision(const Vector3& pos1, const XMMATRIX& rotation1, const Vector3& scale1, const Vector3& pos2, const XMMATRIX& rotation2, const Vector3& scale2)
{
	using namespace DirectX;

	// ��_�Ԃ̋���
	static Vector3 distance{};
	// ���S����ʂ܂ł̒���
	static Vector3 length1{}, length2{};
	// �������x�N�g��
	static Vector3 direction1_XN{}, direction1_YN{}, direction1_ZN{}, direction2_XN{}, direction2_YN{}, direction2_ZN{};
	// �����x�N�g��
	static Vector3 direction1_X{}, direction1_Y{}, direction1_Z{}, direction2_X{}, direction2_Y{}, direction2_Z{};

	/*OBB�̓����蔻��̉�����*/

	length1 = scale1 / 2.0f;
	XMStoreFloat3(&direction1_XN, XMVector4Normalize(rotation1.r[0]));
	XMStoreFloat3(&direction1_YN, XMVector4Normalize(rotation1.r[1]));
	XMStoreFloat3(&direction1_ZN, XMVector4Normalize(rotation1.r[2]));
	direction1_X = direction1_XN * length1.x;
	direction1_Y = direction1_YN * length1.y;
	direction1_Z = direction1_ZN * length1.z;

	length2 = scale2 / 2.0f;
	XMStoreFloat3(&direction2_XN, XMVector4Normalize(rotation2.r[0]));
	XMStoreFloat3(&direction2_YN, XMVector4Normalize(rotation2.r[1]));
	XMStoreFloat3(&direction2_ZN, XMVector4Normalize(rotation2.r[2]));
	direction2_X = direction2_XN * length2.x;
	direction2_Y = direction2_YN * length2.y;
	direction2_Z = direction2_ZN * length2.z;

	/*OBB�̓����蔻��*/

	distance = pos2 - pos1;

	static float r1{}, r2{}; //�����e����
	static float l{}; //���ԓ_�Ԃ̋���

	// �������x�N�g���Fdirection1_XN
	r1 = direction1_X.Length();
	r2 = fabsf(direction1_XN.Dot(direction2_X)) + fabsf(direction1_XN.Dot(direction2_Y)) + fabsf(direction1_XN.Dot(direction2_Z));
	l = fabsf(direction1_XN.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	// �������x�N�g���Fdirection1_YN
	r1 = direction1_Y.Length();
	r2 = fabsf(direction1_YN.Dot(direction2_X)) + fabsf(direction1_YN.Dot(direction2_Y)) + fabsf(direction1_YN.Dot(direction2_Z));
	l = fabsf(direction1_YN.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	// �������x�N�g���Fdirection1_ZN
	r1 = direction1_Z.Length();
	r2 = fabsf(direction1_ZN.Dot(direction2_X)) + fabsf(direction1_ZN.Dot(direction2_Y)) + fabsf(direction1_ZN.Dot(direction2_Z));
	l = fabsf(direction1_ZN.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	// �������x�N�g���Fdirection2_XN
	r1 = direction2_X.Length();
	r2 = fabsf(direction2_XN.Dot(direction1_X)) + fabsf(direction2_XN.Dot(direction1_Y)) + fabsf(direction2_XN.Dot(direction1_Z));
	l = fabsf(direction2_XN.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	// �������x�N�g���Fdirection2_YN
	r1 = direction2_Y.Length();
	r2 = fabsf(direction2_YN.Dot(direction1_X)) + fabsf(direction2_YN.Dot(direction1_Y)) + fabsf(direction2_YN.Dot(direction1_Z));
	l = fabsf(direction2_YN.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	// �������x�N�g���Fdirection2_ZN
	r1 = direction2_Z.Length();
	r2 = fabsf(direction2_ZN.Dot(direction1_X)) + fabsf(direction2_ZN.Dot(direction1_Y)) + fabsf(direction2_ZN.Dot(direction1_Z));
	l = fabsf(direction2_ZN.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	static Vector3 crossDirection{}; //�O�ϕ�����

	// �������x�N�g���FCross(direction1_X, direction2_X)
	crossDirection = direction1_X.Cross(direction2_X);
	r1 = fabsf(crossDirection.Dot(direction1_Y)) + fabsf(crossDirection.Dot(direction1_Z));
	r2 = fabsf(crossDirection.Dot(direction2_Y)) + fabsf(crossDirection.Dot(direction2_Z));
	l = fabsf(crossDirection.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	// �������x�N�g���FCross(direction1_X, direction2_Y)
	crossDirection = direction1_X.Cross(direction2_Y);
	r1 = fabsf(crossDirection.Dot(direction1_Y)) + fabsf(crossDirection.Dot(direction1_Z));
	r2 = fabsf(crossDirection.Dot(direction2_X)) + fabsf(crossDirection.Dot(direction2_Z));
	l = fabsf(crossDirection.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	// �������x�N�g���FCross(direction1_X, direction2_Z)
	crossDirection = direction1_X.Cross(direction2_Z);
	r1 = fabsf(crossDirection.Dot(direction1_Y)) + fabsf(crossDirection.Dot(direction1_Z));
	r2 = fabsf(crossDirection.Dot(direction2_X)) + fabsf(crossDirection.Dot(direction2_Y));
	l = fabsf(crossDirection.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	// �������x�N�g���FCross(direction1_Y, direction2_X)
	crossDirection = direction1_Y.Cross(direction2_X);
	r1 = fabsf(crossDirection.Dot(direction1_X)) + fabsf(crossDirection.Dot(direction1_Z));
	r2 = fabsf(crossDirection.Dot(direction2_Y)) + fabsf(crossDirection.Dot(direction2_Z));
	l = fabsf(crossDirection.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	// �������x�N�g���FCross(direction1_Y, direction2_Y)
	crossDirection = direction1_Y.Cross(direction2_Y);
	r1 = fabsf(crossDirection.Dot(direction1_X)) + fabsf(crossDirection.Dot(direction1_Z));
	r2 = fabsf(crossDirection.Dot(direction2_X)) + fabsf(crossDirection.Dot(direction2_Z));
	l = fabsf(crossDirection.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	// �������x�N�g���FCross(direction1_Y, direction2_Z)
	crossDirection = direction1_Y.Cross(direction2_Z);
	r1 = fabsf(crossDirection.Dot(direction1_X)) + fabsf(crossDirection.Dot(direction1_Z));
	r2 = fabsf(crossDirection.Dot(direction2_X)) + fabsf(crossDirection.Dot(direction2_Y));
	l = fabsf(crossDirection.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	// �������x�N�g���FCross(direction1_Z, direction2_X)
	crossDirection = direction1_Z.Cross(direction2_X);
	r1 = fabsf(crossDirection.Dot(direction1_X)) + fabsf(crossDirection.Dot(direction1_Y));
	r2 = fabsf(crossDirection.Dot(direction2_Y)) + fabsf(crossDirection.Dot(direction2_Z));
	l = fabsf(crossDirection.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	// �������x�N�g���FCross(direction1_Z, direction2_Y)
	crossDirection = direction1_Z.Cross(direction2_Y);
	r1 = fabsf(crossDirection.Dot(direction1_X)) + fabsf(crossDirection.Dot(direction1_Y));
	r2 = fabsf(crossDirection.Dot(direction2_X)) + fabsf(crossDirection.Dot(direction2_Z));
	l = fabsf(crossDirection.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	// �������x�N�g���FCross(direction1_Z, direction2_Z)
	crossDirection = direction1_Z.Cross(direction2_Z);
	r1 = fabsf(crossDirection.Dot(direction1_X)) + fabsf(crossDirection.Dot(direction1_Y));
	r2 = fabsf(crossDirection.Dot(direction2_X)) + fabsf(crossDirection.Dot(direction2_Y));
	l = fabsf(crossDirection.Dot(distance));
	if ((r1 + r2) < l)
	{
		return false;
	}

	return true;
}

float sphereSwept(const Vector3& pos1, const Vector3& speed1, const float& r1, const Vector3& pos2, const Vector3& speed2, const float& r2)
{
	static const float noHit = -1.0f;

	static Vector3 endPos1{};       //t = 1.0f�n�_��pos1�̒��S�ʒu
	static Vector3 endPos2{};       //t = 1.0f�n�_��pos2�̒��S�ʒu
	static Vector3 startDistance{}; //t = 0.0f�n�_�̒��S�_�Ԃ̋���
	static Vector3 endDistance{};   //t = 1.0f�n�_�̒��S�_�Ԃ̋���
	static Vector3 distance{};      //�񎲊Ԃ̋���

	// t = 0.0f�n�_�̒��S�_�Ԃ̋����̌v�Z
	startDistance = pos2 - pos1;

	// t = 0.0f�n�_�̏Փ˔���
	if (distance.Length() < (r1 + r2))
	{
		return 0.0f;
	}

	// t = 1.0f�n�_��pos1�̒��S�ʒu�̌v�Z
	endPos1 = pos1 + speed1;
	// t = 1.0f�n�_��pos2�̒��S�ʒu�̌v�Z
	endPos2 = pos2 + speed2;
	// t = 1.0f�n�_�̒��S�_�Ԃ̋����̌v�Z
	endDistance = endPos2 - endPos1;
	// t = 1.0f�n�_�̒��S�_�Ԃ̋��� - t = 0.0f�n�_�̒��S�_�Ԃ̋���
	distance = endDistance - startDistance;

	static float t = 0.0f; //�Փ˂�������
	static float p = 0.0f; //distance�̒����̓��
	static float q = 0.0f; //startDistance��distance�̓���
	static float r = 0.0f; //startDistance�̒����̓��

	// distance�̒����̓��̌v�Z
	p = distance.LengthSquared();
	if (p == 0)
	{
		// distance�̒�����0�Ȃ�Փ˂��Ă��Ȃ�
		return noHit;
	}

	// startDistance�̒����̓��̌v�Z
	r = startDistance.LengthSquared();
	// startDistance��distance�̓��ς̌v�Z
	q = startDistance.Dot(distance);

	float keep = (q * q) - p * (r - ((r1 + r2) * (r1 + r2))); //���[�g���̌v�Z
	if (keep < 0)
	{
		// ���[�g�����}�C�i�X�̏ꍇ�͏Փ˂��Ȃ�
		return noHit;
	}
	else
	{
		// �Փˎ��Ԃ̌v�Z
		t = (-q - sqrtf(keep)) / p;
		// ���[�g�����v���X�ŋ��߂�t��0~1�̊Ԃɖ����ꍇ�͎���ς��Čv�Z������
		if (keep > 0 && (t < 0.0f || t > 1.0f))
		{
			t = (-q + sqrtf(keep)) / p;
		}
	}

	if (t >= 0.0f && t <= 1.0f)
	{
		return t;
	}
	else
	{
		return noHit;
	}
}

bool IsPredictCollisionBall(const Vector3& pos1, const Vector3& speed1, const float& r1, const Vector3& pos2, const Vector3& speed2, const float& r2)
{
	// ��_�Ԃ̋���
	static Vector3 distance{};
	// ���S�_�Ԃ̋���
	distance = pos1 - pos2;

	return distance.Length() < ((r1 + speed1.Length()) + (r2 + speed2.Length()));
}
} // namespace Collision
