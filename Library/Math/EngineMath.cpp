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

int ConservationOfMomentum(Vector3* vel1, const float mass1, Vector3* vel2, const float mass2)
{
	if (mass1 <= 0.0f || mass2 <= 0.0f)
	{
		return EF;
	}

	//float rate = mass1 / mass2;
	Vector3 keepVel1 = *vel1;
	Vector3 keepVel2 = *vel2;

	*vel1 += (keepVel2 * mass2 - keepVel1 * mass1) / mass1;
	*vel2 += (keepVel1 * mass1 - keepVel2 * mass2) / mass2;

	return 0;
}
} //Math
} //Engine