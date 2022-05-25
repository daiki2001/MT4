#include "EngineMath.h"
#include "./Header/Error.h"

namespace Engine
	{
namespace Math
{
bool operator==(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

bool operator!=(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b)
{
	return !(a == b);
}

int CollisionPhysics(MathObject* obj1, MathObject* obj2)
{
	if (obj1->mass <= 0.0f || obj2->mass <= 0.0f)
	{
		return EF;
	}

	Vector3 keepVel1 = { 0.0f, 0.0f, 0.0f };
	Vector3 keepVel2 = { 0.0f, 0.0f, 0.0f };
	keepVel1 = (obj1->vel * (obj1->mass - obj2->mass) + (2 * obj2->vel * obj2->mass)) / (obj1->mass + obj2->mass);
	keepVel2 = (obj2->vel * (obj2->mass - obj1->mass) + (2 * obj1->vel * obj1->mass)) / (obj1->mass + obj2->mass);

	float restitution = (obj1->restitution + obj2->restitution) / 2;
	keepVel1 *= restitution;
	keepVel2 *= restitution;

	obj1->vel = keepVel1;
	obj2->vel = keepVel2;

	return 0;
}
} //Math
} //Engine