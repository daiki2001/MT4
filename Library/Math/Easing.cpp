#include "Easing.h"

namespace Engine
{
namespace Math
{
// ���`���
const Vector3 Lerp(const Vector3& start, const Vector3& end, const float t)
{
	// float y = t;
	// return start * (1.0f - y) + end * y;
	return start * (1.0f - t) + end * t;
}

// easeIn���
const Vector3 easeIn(const Vector3& start, const Vector3& end, const float t)
{
	float y = t * t;
	return start * (1.0f - y) + end * y;
}

// easeOut���
const Vector3 easeOut(const Vector3& start, const Vector3& end, const float t)
{
	float y = t * (2 - t);
	return start * (1.0f - y) + end * y;
}

// easeInOut���
const Vector3 easeInOut(const Vector3& start, const Vector3& end, const float t)
{
	float y = t * t * (3 - 2 * t);
	return start * (1.0f - y) + end * y;
}
} //Math
} //Engine
