#pragma once
#include "Vector3.h"

// ��Ԋ֐�
namespace Engine
{
namespace Math
{
// ���`���(1���֐����)
const Vector3 Lerp(const Vector3& start, const Vector3& end, const float t);

// easeIn���
const Vector3 easeIn(const Vector3& start, const Vector3& end, const float t);
// easeOut���
const Vector3 easeOut(const Vector3& start, const Vector3& end, const float t);
// easeInOut���
const Vector3 easeInOut(const Vector3& start, const Vector3& end, const float t);
} //Math
} //Engine
