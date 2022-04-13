#include "./Header/DrawPolygon.h"
#include "./Header/Camera.h"
#include <fstream>
#include <sstream>
#include <string>

#define EoF -1 // End of Function

DrawPolygon::DrawPolygon(const DirectXInit* w) :
	DebugText(w),
	polygonCount(0),
	loopCount(0),
	lightVec{},
	light(100, -100, 100),
	cameraNo(MAIN_CAMERA),
	objModelCount(0),
	verticesCount{},
	material{}
{
	using DirectX::XMLoadFloat3;

	XMFLOAT3 cameraPos = { 0, 0, -100 };
	XMFLOAT3 cameraTarget = { 0, 0, 0 };
	XMFLOAT3 upVector = { 0, 1, 0 };

	matView.push_back(Camera::CreateCamera(
		XMLoadFloat3(&cameraPos),
		XMLoadFloat3(&cameraTarget),
		XMLoadFloat3(&upVector)
	));
}

DrawPolygon::~DrawPolygon()
{
	DataClear();
}

int DrawPolygon::CreateTriangle(
	const XMFLOAT3& vertex1, const DirectX::XMFLOAT2& uv1,
	const XMFLOAT3& vertex2, const DirectX::XMFLOAT2& uv2,
	const XMFLOAT3& vertex3, const DirectX::XMFLOAT2& uv3,
	const bool& isFill)
{
	vertices.push_back(VertexData());

	vertices[vertices.size() - 1].vertices.push_back({ vertex1, {}, uv1 });
	vertices[vertices.size() - 1].vertices.push_back({ vertex2, {}, uv2 });
	vertices[vertices.size() - 1].vertices.push_back({ vertex3, {}, uv3 });

	if (isFill == true)
	{
		vertices[vertices.size() - 1].indices.push_back(0);
		vertices[vertices.size() - 1].indices.push_back(1);
		vertices[vertices.size() - 1].indices.push_back(2);
	}
	else
	{
		vertices[vertices.size() - 1].indices.push_back(0);
		vertices[vertices.size() - 1].indices.push_back(1);
		vertices[vertices.size() - 1].indices.push_back(1);
		vertices[vertices.size() - 1].indices.push_back(2);
		vertices[vertices.size() - 1].indices.push_back(2);
		vertices[vertices.size() - 1].indices.push_back(0);
	}

	return CreateVertexAndIndexBuffer();
}

int DrawPolygon::CreateRect(const float& width, const float& height, const bool& isFill)
{
	vertices.push_back(VertexData());

	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, +height / 2, 0.0f }, {}, { 0.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, +height / 2, 0.0f }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, -height / 2, 0.0f }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, +height / 2, 0.0f }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, -height / 2, 0.0f }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, -height / 2, 0.0f }, {}, { 1.0f, 1.0f } });

	if (isFill == true)
	{
		vertices[vertices.size() - 1].indices.push_back(0);
		vertices[vertices.size() - 1].indices.push_back(1);
		vertices[vertices.size() - 1].indices.push_back(2);
		vertices[vertices.size() - 1].indices.push_back(4);
		vertices[vertices.size() - 1].indices.push_back(3);
		vertices[vertices.size() - 1].indices.push_back(5);
	}
	else
	{
		vertices[vertices.size() - 1].indices.push_back(0);
		vertices[vertices.size() - 1].indices.push_back(1);
		vertices[vertices.size() - 1].indices.push_back(3);
		vertices[vertices.size() - 1].indices.push_back(5);
		vertices[vertices.size() - 1].indices.push_back(5);
		vertices[vertices.size() - 1].indices.push_back(2);
		vertices[vertices.size() - 1].indices.push_back(4);
		vertices[vertices.size() - 1].indices.push_back(0);
	}

	return CreateVertexAndIndexBuffer();
}

int DrawPolygon::CreateCircle(const float& r, const size_t& divNum, const bool& isFill)
{
	vertices.push_back(VertexData());

	Vertex* v = (Vertex*)malloc(sizeof(Vertex) * (divNum + 1));
	size_t indexSize = divNum * (2 + (size_t)isFill);
	unsigned short* index = (unsigned short*)malloc(sizeof(unsigned short) * indexSize);

	Circle({ 0, 0, 0 }, r, divNum, isFill, v, index);

	for (size_t i = 0; i < divNum + 1; i++)
	{
		vertices[vertices.size() - 1].vertices.push_back(v[i]);
	}
	for (size_t i = 0; i < indexSize; i++)
	{
		vertices[vertices.size() - 1].indices.push_back(index[i]);
	}

	free(v);
	free(index);

	for (size_t i = 0; i < vertices[vertices.size() - 1].vertices.size(); i++)
	{
		vertices[vertices.size() - 1].vertices[i].pos.x *= -1;
	}

	return CreateVertexAndIndexBuffer();
}

int DrawPolygon::Create3Dbox(const float& width, const float& height, const float& depth, const bool& isFill)
{
	vertices.push_back(VertexData());

	// �O
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, +height / 2, -depth / 2 }, {}, { 0.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, +height / 2, -depth / 2 }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, -height / 2, -depth / 2 }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, +height / 2, -depth / 2 }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, -height / 2, -depth / 2 }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, -height / 2, -depth / 2 }, {}, { 1.0f, 1.0f } });
	// ���
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, +height / 2, +depth / 2 }, {}, { 0.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, -height / 2, +depth / 2 }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, +height / 2, +depth / 2 }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, -height / 2, +depth / 2 }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, +height / 2, +depth / 2 }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, -height / 2, +depth / 2 }, {}, { 1.0f, 1.0f } });
	// ��
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, -height / 2, +depth / 2 }, {}, { 0.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, +height / 2, +depth / 2 }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, -height / 2, -depth / 2 }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, +height / 2, +depth / 2 }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, -height / 2, -depth / 2 }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, +height / 2, -depth / 2 }, {}, { 1.0f, 1.0f } });
	// �E
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, -height / 2, +depth / 2 }, {}, { 0.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, -height / 2, -depth / 2 }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, +height / 2, +depth / 2 }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, -height / 2, -depth / 2 }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, +height / 2, +depth / 2 }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, +height / 2, -depth / 2 }, {}, { 1.0f, 1.0f } });
	// ��
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, -height / 2, -depth / 2 }, {}, { 0.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, -height / 2, +depth / 2 }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, -height / 2, -depth / 2 }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, -height / 2, +depth / 2 }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, -height / 2, -depth / 2 }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, -height / 2, +depth / 2 }, {}, { 1.0f, 1.0f } });
	// ��
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, +height / 2, -depth / 2 }, {}, { 0.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, +height / 2, -depth / 2 }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, +height / 2, +depth / 2 }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, +height / 2, -depth / 2 }, {}, { 0.0f, 1.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { +width / 2, +height / 2, +depth / 2 }, {}, { 1.0f, 0.0f } });
	vertices[vertices.size() - 1].vertices.push_back({ { -width / 2, +height / 2, +depth / 2 }, {}, { 1.0f, 1.0f } });

	if (isFill == true)
	{
		for (unsigned short i = 0; i < 6; i++)
		{
			vertices[vertices.size() - 1].indices.push_back(0 + 3 * 2 * i);
			vertices[vertices.size() - 1].indices.push_back(1 + 3 * 2 * i);
			vertices[vertices.size() - 1].indices.push_back(2 + 3 * 2 * i);
			vertices[vertices.size() - 1].indices.push_back(4 + 3 * 2 * i);
			vertices[vertices.size() - 1].indices.push_back(3 + 3 * 2 * i);
			vertices[vertices.size() - 1].indices.push_back(5 + 3 * 2 * i);
		}
	}
	else
	{
		for (unsigned short i = 0; i < 6; i++)
		{
			vertices[vertices.size() - 1].indices.push_back(0 + 2 * 4 * i);
			vertices[vertices.size() - 1].indices.push_back(1 + 2 * 4 * i);
			vertices[vertices.size() - 1].indices.push_back(3 + 2 * 4 * i);
			vertices[vertices.size() - 1].indices.push_back(5 + 2 * 4 * i);
			vertices[vertices.size() - 1].indices.push_back(5 + 2 * 4 * i);
			vertices[vertices.size() - 1].indices.push_back(2 + 2 * 4 * i);
			vertices[vertices.size() - 1].indices.push_back(4 + 2 * 4 * i);
			vertices[vertices.size() - 1].indices.push_back(0 + 2 * 4 * i);
		}
	}

	return CreateVertexAndIndexBuffer();
}

int DrawPolygon::CreateCorn(const float& r, const float& h, const size_t& divNum, const bool& isFill)
{
	vertices.push_back(VertexData());

	Vertex* v = (Vertex*)malloc(sizeof(Vertex) * (divNum + 1));
	size_t indexSize = divNum * (2 + (size_t)isFill);
	unsigned short* index = (unsigned short*)malloc(sizeof(unsigned short) * indexSize);

	Circle({ 0, 0, 0 }, r, divNum, isFill, v, index);

	for (size_t i = 0; i < divNum + 1; i++)
	{
		vertices[vertices.size() - 1].vertices.push_back(v[i]);
	}
	for (size_t i = 0; i < indexSize; i++)
	{
		vertices[vertices.size() - 1].indices.push_back(index[i]);
	}

	free(v);
	free(index);

	size_t size = vertices[vertices.size() - 1].vertices.size();

	for (size_t i = 0; i < divNum; i++)
	{
		vertices[vertices.size() - 1].vertices.push_back(vertices[vertices.size() - 1].vertices[i + 0]);
		vertices[vertices.size() - 1].vertices.push_back(vertices[vertices.size() - 1].vertices[i + 1]);
		vertices[vertices.size() - 1].vertices.push_back({
			{
				vertices[vertices.size() - 1].vertices[divNum].pos.x,
				vertices[vertices.size() - 1].vertices[divNum].pos.y,
				vertices[vertices.size() - 1].vertices[divNum].pos.z + h
			},
			vertices[vertices.size() - 1].vertices[divNum].normal,
			vertices[vertices.size() - 1].vertices[divNum].uv,
			});

		if (isFill == true)
		{
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 3 + 0 + size));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 3 + 1 + size));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 3 + 2 + size));
		}
		else
		{
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 3 + 0 + size));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 3 + 1 + size));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 3 + 1 + size));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 3 + 2 + size));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 3 + 2 + size));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 3 + 0 + size));
		}
	}

	vertices[vertices.size() - 1].vertices[divNum * 3 - 2 + size] =
		vertices[vertices.size() - 1].vertices[0];

	return CreateVertexAndIndexBuffer();
}

int DrawPolygon::CreateCylinder(const float& r, const float& h, const size_t& divNum, const bool& isFill)
{
	vertices.push_back(VertexData());

	Vertex* v = (Vertex*)malloc(sizeof(Vertex) * (divNum + 1));
	size_t indexSize = divNum * (2 + (size_t)isFill);
	unsigned short* index = (unsigned short*)malloc(sizeof(unsigned short) * indexSize);

	Circle({ 0, 0, -h /*/ 2.0f*/ }, r, divNum, isFill, v, index);

	for (size_t i = 0; i < divNum + 1; i++)
	{
		vertices[vertices.size() - 1].vertices.push_back(v[i]);
	}
	for (size_t i = 0; i < indexSize; i++)
	{
		vertices[vertices.size() - 1].indices.push_back(index[i]);
	}

	size_t size1 = vertices[vertices.size() - 1].vertices.size();

	Circle({ 0, 0, 0.0f }, r, divNum, isFill, v, index);

	for (size_t i = 0; i < divNum + 1; i++)
	{
		vertices[vertices.size() - 1].vertices.push_back(v[i]);
	}
	for (size_t i = 0; i < indexSize; i++)
	{
		vertices[vertices.size() - 1].indices.push_back(index[i] + (unsigned short)size1);
	}

	free(v);
	free(index);

	size_t size2 = vertices[vertices.size() - 1].vertices.size();

	for (size_t i = 0; i < divNum; i++)
	{
		vertices[vertices.size() - 1].vertices.push_back(vertices[vertices.size() - 1].vertices[i + 0]);
		vertices[vertices.size() - 1].vertices.push_back(vertices[vertices.size() - 1].vertices[i + 1]);
		vertices[vertices.size() - 1].vertices.push_back(vertices[vertices.size() - 1].vertices[i + 0 + size1]);
		vertices[vertices.size() - 1].vertices.push_back(vertices[vertices.size() - 1].vertices[i + 1 + size1]);

		if (isFill == true)
		{
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 4 + 1 + size2));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 4 + 0 + size2));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 4 + 3 + size2));

			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 4 + 3 + size2));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 4 + 0 + size2));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 4 + 2 + size2));
		}
		else
		{
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 4 + 0 + size2));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 4 + 1 + size2));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 4 + 2 + size2));
			vertices[vertices.size() - 1].indices.push_back((unsigned short)(i * 4 + 3 + size2));
		}
	}

	vertices[vertices.size() - 1].vertices[divNum * 4 - 3 + size2] = vertices[vertices.size() - 1].vertices[0];
	vertices[vertices.size() - 1].vertices[divNum * 4 - 1 + size2] = vertices[vertices.size() - 1].vertices[0 + size1];

	for (size_t i = 0; i < size1 - 1; i++)
	{
		vertices[vertices.size() - 1].vertices[i].pos.x *= -1;
	}

	return CreateVertexAndIndexBuffer();
}

int DrawPolygon::CreateSphere(const float& r, const size_t& divNum, const bool& isFill)
{
	using namespace DirectX;

	vertices.push_back(VertexData());

	// �ϐ��ȗ�
	auto& vert = vertices[vertices.size() - 1];

	size_t sizeV = vert.vertices.size();
	size_t oldsizeV = sizeV;
	size_t sizeI = vert.indices.size();
	size_t oldsizeI = sizeI;
	size_t indexSize = divNum * 3;
	size_t divNum2 = divNum - 1;
	Vertex* v = (Vertex*)malloc(sizeof(Vertex) * (divNum + 1));
	unsigned short* index = (unsigned short*)malloc(sizeof(unsigned short) * indexSize);

	for (size_t i = 1; i < divNum2; i++)
	{
		float r2 = r * sinf(XM_PI / divNum2 * i);
		float h = r * cosf(XM_PI / divNum2 * i);

		if (r2 >= r * -0.00000005f && r2 <= r * 0.00000005f)
		{
			r2 = 0;
		}
		if (h >= r * -0.00000005f && h <= r * 0.00000005f)
		{
			h = 0;
		}

		Circle({ 0, 0, h }, r2, divNum, true, v, index);

		oldsizeV = sizeV;
		oldsizeI = sizeI;
		sizeV = vert.vertices.size();
		sizeI = vert.indices.size();

		if (i == 1)
		{
			v[divNum].pos.z = r;

			if (isFill == true)
			{
				for (size_t i = 0; i < divNum; i++)
				{
					vert.vertices.push_back(v[i + 0]);
					vert.vertices.push_back(v[i + 1]);
					vert.vertices.push_back(v[divNum]);

					vert.indices.push_back((unsigned short)(i * 3 + 1 + sizeV));
					vert.indices.push_back((unsigned short)(i * 3 + 0 + sizeV));
					vert.indices.push_back((unsigned short)(i * 3 + 2 + sizeV));
				}
				vert.vertices[(divNum * 3 - 2) + sizeV] = v[0];
			}
			else
			{
				for (size_t j = 0; j < divNum + 1; j++)
				{
					vert.vertices.push_back(v[j]);
				}

				for (size_t j = 0; j < divNum; j++)
				{
					vert.indices.push_back((unsigned short)((j + 0) + sizeV));
					vert.indices.push_back((unsigned short)((j + 1) + sizeV));
					vert.indices.push_back((unsigned short)((j + 1) + sizeV));
					vert.indices.push_back((unsigned short)(divNum + sizeV));
					vert.indices.push_back((unsigned short)(divNum + sizeV));
					vert.indices.push_back((unsigned short)((j + 0) + sizeV));
				}
				vert.indices[(divNum * 6 - 4) + sizeV] = (unsigned short)sizeV;
				vert.indices[(divNum * 6 - 5) + sizeV] = (unsigned short)sizeV;
			}
		}
		else
		{
			if (isFill == true)
			{
				for (size_t j = 0; j < divNum; j++)
				{
					if (i == 2)
					{
						vert.vertices.push_back(v[j + 0]);
						vert.vertices.push_back(v[j + 1]);
						vert.vertices.push_back(vert.vertices[j * 3 + 0 + oldsizeV]);
						vert.vertices.push_back(vert.vertices[j * 3 + 1 + oldsizeV]);
					}
					else
					{
						vert.vertices.push_back(v[j + 0]);
						vert.vertices.push_back(v[j + 1]);
						vert.vertices.push_back(vert.vertices[j * 4 + 0 + oldsizeV]);
						vert.vertices.push_back(vert.vertices[j * 4 + 1 + oldsizeV]);
					}

					vert.indices.push_back((unsigned short)((j * 4 + 1) + sizeV));
					vert.indices.push_back((unsigned short)((j * 4 + 0) + sizeV));
					vert.indices.push_back((unsigned short)((j * 4 + 3) + sizeV));

					vert.indices.push_back((unsigned short)((j * 4 + 3) + sizeV));
					vert.indices.push_back((unsigned short)((j * 4 + 0) + sizeV));
					vert.indices.push_back((unsigned short)((j * 4 + 2) + sizeV));
				}
				vert.vertices[divNum * 4 + sizeV - 1] = vert.vertices[oldsizeV];
				vert.vertices[divNum * 4 + sizeV - 3] = v[0];
			}
			else
			{
				for (size_t j = 0; j < divNum; j++)
				{
					vert.vertices.push_back(v[j]);

					vert.indices.push_back((unsigned short)((j + 0) + oldsizeV));
					vert.indices.push_back((unsigned short)((j + 0) + sizeV));
					vert.indices.push_back((unsigned short)((j + 0) + sizeV));
					vert.indices.push_back((unsigned short)((j + 1) + sizeV));
				}
				vert.indices[(divNum * 4 - 1) + sizeI] = (unsigned short)sizeV;
			}
		}
	}
	oldsizeV = sizeV;
	oldsizeI = sizeI;
	sizeV = vert.vertices.size();
	sizeI = vert.indices.size();

	v[divNum].pos.z = -r;

	if (isFill == true)
	{
		for (size_t i = 0; i < divNum; i++)
		{
			vert.vertices.push_back(v[i + 0]);
			vert.vertices.push_back(v[i + 1]);
			vert.vertices.push_back(v[divNum]);

			vert.indices.push_back((unsigned short)(i * 3 + 0 + sizeV));
			vert.indices.push_back((unsigned short)(i * 3 + 1 + sizeV));
			vert.indices.push_back((unsigned short)(i * 3 + 2 + sizeV));
		}
		vert.vertices[(divNum * 3 - 2) + sizeV] = v[0];
	}
	else
	{
		for (size_t i = 0; i < divNum; i++)
		{
			if (divNum == 3)
			{
				vert.indices.push_back((unsigned short)((i + 0) + oldsizeV));
				vert.indices.push_back((unsigned short)(divNum + 1 + oldsizeV));
			}
			else
			{
				vert.indices.push_back((unsigned short)((i + 0) + oldsizeV));
				vert.indices.push_back((unsigned short)(divNum + oldsizeV));
			}
		}
		vert.vertices.push_back(v[divNum]);
	}

	free(v);
	free(index);

	return CreateVertexAndIndexBuffer();
}

int DrawPolygon::CreateOBJModel(const char* filePath, const char* directoryPath)
{
	if (filePath == nullptr) { return EoF; }

	using namespace std;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	HRESULT hr;

	ifstream file;
	const string modelPath = filePath;
	file.open(modelPath);

	vector<XMFLOAT3> positions; //���_���W
	vector<XMFLOAT3> normals; //�@���x�N�g��
	vector<XMFLOAT2> texcoords; //�e�N�X�`��UV

	bool isFaceStart = false;
	size_t verticesCount = 0;
	int objectsCount = static_cast<int>(obj.size());

	Object* parent = nullptr;

	// ��s���ǂݍ���
	string line;

	while (getline(file, line))
	{
		// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪��������擾����
		std::string key;
		std::getline(line_stream, key, ' ');

		// �擪������v�Ȃ璸�_���W
		if (key == "v")
		{
			// X,Y,Z���W�ǂݍ���
			XMFLOAT3 pos{};
			line_stream >> pos.x;
			line_stream >> pos.y;
			line_stream >> pos.z;
			// ���W�f�[�^�̒ǉ�
			positions.emplace_back(pos);
		}

		// �擪������vt�Ȃ�e�N�X�`��
		if (key == "vt")
		{
			// U,V�����ǂݍ���
			XMFLOAT2 tex{};
			line_stream >> tex.x;
			line_stream >> tex.y;
			// V�������]
			tex.y = 1.0f - tex.y;
			// �e�N�X�`���f�[�^�̒ǉ�
			texcoords.emplace_back(tex);
		}

		// �擪������vn�Ȃ�@���x�N�g��
		if (key == "vn")
		{
			// X,Y,Z�����ǂݍ���
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			// �@���x�N�g���f�[�^�̒ǉ�
			normals.emplace_back(normal);
		}

		// �擪������f�Ȃ�|���S���i�O�p�`�j
		if (key == "f")
		{
			size_t index_count = 0;
			// ���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
			std::string index_string;
			while (std::getline(line_stream, index_string, ' '))
			{
				if (isFaceStart == false)
				{
					isFaceStart = true;
					vertices.push_back(VertexData());
				}

				// ���_�C���f�b�N�X1���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
				std::istringstream index_stream(index_string);
				unsigned short indexPos, indexUv, indexNormal;
				// �C���f�b�N�X�̏�񂪒��_���W�݂̂��ǂ���
				auto resultIter = std::find(index_string.begin(), index_string.end(), '/');
				auto temp = std::distance(index_string.begin(), ++resultIter);
				// �ϐ��ȗ�
				auto& vert = vertices[vertices.size() - 1];

				if (index_string[temp] == '/')
				{
					index_stream >> indexPos;

					// ���_�f�[�^�̒ǉ�
					Vertex vertex{};
					vertex.pos = positions[indexPos - 1Ui64];
					vertex.normal = XMFLOAT3();
					vertex.uv = XMFLOAT2();
					vert.vertices.emplace_back(vertex);
				}
				else
				{
					index_stream >> indexPos;
					index_stream.seekg(1, std::ios_base::cur); //�X���b�V�����΂�
					index_stream >> indexUv;
					index_stream.seekg(1, std::ios_base::cur); //�X���b�V�����΂�
					index_stream >> indexNormal;

					// ���_�f�[�^�̒ǉ�
					Vertex vertex{};
					vertex.pos = positions[indexPos - 1Ui64];
					vertex.normal = normals[indexNormal - 1Ui64];
					vertex.uv = texcoords[indexUv - 1Ui64];
					vert.vertices.emplace_back(vertex);
				}

				// �C���f�b�N�X�f�[�^�̒ǉ�
				vert.indices.emplace_back((unsigned short)vert.vertices.size() - 1);
				index_count++;

				if (index_count > 3)
				{
					vert.indices.emplace_back((unsigned short)(vert.vertices.size() - index_count));
					vert.indices.emplace_back(vert.indices[vert.indices.size() - 2] - 1);
				}
			}
		}

		// �擪������mtllib�Ȃ�}�e���A���t�@�C���̓ǂݍ���
		if (key == "mtllib")
		{
			// �}�e���A���̃t�@�C�����ǂݍ���
			std::string fileName;
			line_stream >> fileName;
			// �}�e���A���ǂݍ���
			LoadMaterial(directoryPath, fileName);
		}

		// �擪������usemtl�Ȃ�g�p����}�e���A��
		if (key == "usemtl")
		{
			// �}�e���A�����ǂݍ���
			std::string materialName;
			line_stream >> materialName;

			size_t materialIndex = 0;
			for (materialIndex = 0; materialIndex < material.size(); materialIndex++)
			{
				if (materialName == material[materialIndex].name)
				{
					break;
				}
			}
			if (materialIndex >= material.size())
			{
				return EoF;
			}

			isFaceStart = false;
			obj.push_back({});
			if (parent == nullptr)
			{
				parent = &obj[obj.size() - 1];
			}
			else
			{
				obj[obj.size() - 1].parent = parent;
			}
			obj[obj.size() - 1].polygonData = (int)(vertices.size());
			obj[obj.size() - 1].material = material[materialIndex];
			objModelCount++;
			verticesCount++;
		}
	}
	// �t�@�C�������
	file.close();

	// ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	for (size_t i = vertices.size() - verticesCount; i < vertices.size(); i++)
	{
#pragma region VertexBuffer

		UINT sizeVB;
		sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices[i].vertices.size());

		hr = w->dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertices[i].vertBuff));
		if (FAILED(hr))
		{
			return EoF;
		}

		hr = vertices[i].vertBuff->Map(0, nullptr, (void**)&vertMap);

		// �S���_�ɑ΂���
		for (UINT j = 0; j < vertices[i].vertices.size(); j++)
		{
			vertMap[j] = vertices[i].vertices[j]; //���W���R�s�[
		}

		// �}�b�v������
		vertices[i].vertBuff->Unmap(0, nullptr);

		vertices[i].vbView.BufferLocation = vertices[i].vertBuff->GetGPUVirtualAddress();
		vertices[i].vbView.SizeInBytes = sizeVB;
		vertices[i].vbView.StrideInBytes = sizeof(Vertex);

#pragma endregion //VertexBuffer

#pragma region IndexBuffer

		//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
		UINT sizeIB;
		sizeIB = static_cast<UINT>(sizeof(unsigned short) * vertices[i].indices.size());

		hr = w->dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertices[i].indexBuff));
		if (FAILED(hr))
		{
			return EoF;
		}

		// GPU��̃o�b�t�@�ɑΉ��������z���������擾
		static unsigned short* indexMap = nullptr;
		hr = vertices[i].indexBuff->Map(0, nullptr, (void**)&indexMap);

		// �S�C���f�b�N�X�ɑ΂���
		for (size_t j = 0; j < vertices[i].indices.size(); j++)
		{
			indexMap[j] = vertices[i].indices[j]; //�C���f�b�N�X���R�s�[
		}

		// �q���������
		vertices[i].indexBuff->Unmap(0, nullptr);

		vertices[i].ibView.BufferLocation = vertices[i].indexBuff->GetGPUVirtualAddress();
		vertices[i].ibView.Format = DXGI_FORMAT_R16_UINT;
		vertices[i].ibView.SizeInBytes = sizeIB;

#pragma endregion //IndexBuffer
	}

	for (size_t i = obj.size() - verticesCount; i < obj.size(); i++)
	{
		hr = w->dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xFF) & ~0xFF), //���\�[�X�ݒ�
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&obj[i].constBuff));
		if (FAILED(hr))
		{
			return hr;
		}
		hr = w->dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(MaterialConstBufferData) + 0xFF) & ~0xFF), //���\�[�X�ݒ�
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&obj[i].materialConstBuff));
		if (FAILED(hr))
		{
			return hr;
		}
	}

	return (int)(obj.size() - 1);
}

void DrawPolygon::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	// �t�@�C���X�g���[��
	std::ifstream file;
	// �}�e���A���t�@�C�����J��
	file.open(directoryPath + filename);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail())
	{
		assert(0);
	}

	material.clear();

	// 1�s���ǂݍ���
	std::string line;
	while (std::getline(file, line))
	{
		// 1�s���̕�������X�g���[���ɕϊ�
		std::istringstream line_stream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;
		std::getline(line_stream, key, ' ');

		// �擪�̃^�u�͖�������
		if (key[0] == '\t')
		{
			key.erase(key.begin()); //�擪�̕������폜
		}

		// �擪������newmtl�Ȃ�}�e���A����
		if (key == "newmtl")
		{
			material.push_back({});
			// �}�e���A�����ǂݍ���
			line_stream >> material[material.size() - 1].name;
		}

		// �擪������Ka�Ȃ�A���r�G���g�F
		if (key == "Ka")
		{
			line_stream >> material[material.size() - 1].ambient.x;
			line_stream >> material[material.size() - 1].ambient.y;
			line_stream >> material[material.size() - 1].ambient.z;
		}

		// �擪������Kd�Ȃ�f�B�t���[�Y�F
		if (key == "Kd")
		{
			line_stream >> material[material.size() - 1].diffuse.x;
			line_stream >> material[material.size() - 1].diffuse.y;
			line_stream >> material[material.size() - 1].diffuse.z;
		}

		// �擪������Ks�Ȃ�X�y�L�����[�F
		if (key == "Ks")
		{
			line_stream >> material[material.size() - 1].specular.x;
			line_stream >> material[material.size() - 1].specular.y;
			line_stream >> material[material.size() - 1].specular.z;
		}

		// �擪������d��Tr�Ȃ�A���t�@
		if (key == "d" || key == "Tr")
		{
			line_stream >> material[material.size() - 1].alpha;
		}

		// �擪������map_Kd�Ȃ�e�N�X�`���t�@�C����
		if (key == "map_Kd")
		{
			// �e�N�X�`���̃t�@�C�����ǂݍ���
			line_stream >> material[material.size() - 1].textureFilename;
			std::string texFilePath = directoryPath + material[material.size() - 1].textureFilename;
			// ���j�R�[�h������ɕϊ�����
			wchar_t filepath[128];
			int iBufferSize = MultiByteToWideChar(CP_ACP, 0, texFilePath.c_str(), -1, filepath, _countof(filepath));
			// �e�N�X�`���ǂݍ���
			material[material.size() - 1].textrueIndex = LoadTextrue(filepath);
		}
	}
	// �t�@�C�������
	file.close();
}

void DrawPolygon::Tiring(const int& polygonData, const float& uvScaleX, const float& uvScaleY)
{
	if (polygonData < 0 || (size_t)polygonData >= vertices.size())
	{
		return;
	}

	for (size_t i = 0; i < vertices[polygonData].vertices.size(); i++)
	{
		vertices[polygonData].vertices[i].uv.x *= uvScaleX;
		vertices[polygonData].vertices[i].uv.y *= uvScaleY;
	}

	vertices[polygonData].vertBuff->Map(0, nullptr, (void**)&vertMap);

	// �S���_�ɑ΂���
	for (UINT i = 0; i < vertices[polygonData].vertices.size(); i++)
	{
		vertMap[i] = vertices[polygonData].vertices[i]; //���W���R�s�[
	}

	// �}�b�v������
	vertices[polygonData].vertBuff->Unmap(0, nullptr);
}

void DrawPolygon::AutoTiring(const int& polygonData, const int& graphHandle)
{
	if ((polygonData < 0 || (size_t)polygonData >= vertices.size()) ||
		(graphHandle < 0 || (size_t)graphHandle >= textrueData.size()))
	{
		return;
	}

	// �e�N�X�`���f�[�^�擾
	D3D12_RESOURCE_DESC resDesc = textrueData[graphHandle].texbuff->GetDesc();
	float left = 0.0f, right = 0.0f, top = 0.0f, bottom = 0.0f;

	for (size_t i = 0; i < vertices[polygonData].vertices.size(); i++)
	{
		if (vertices[polygonData].vertices[i].pos.x < left)
		{
			left = vertices[polygonData].vertices[i].pos.x;
		}
		if (vertices[polygonData].vertices[i].pos.x > right)
		{
			right = vertices[polygonData].vertices[i].pos.x;
		}
		if (vertices[polygonData].vertices[i].pos.y < top)
		{
			top = vertices[polygonData].vertices[i].pos.y;
		}
		if (vertices[polygonData].vertices[i].pos.y > bottom)
		{
			bottom = vertices[polygonData].vertices[i].pos.y;
		}
	}
	DirectX::XMFLOAT2 size = { right - left, bottom - top };
	size.x /= resDesc.Width;
	size.y /= resDesc.Height;

	for (size_t i = 0; i < vertices[polygonData].vertices.size(); i++)
	{
		vertices[polygonData].vertices[i].uv.x *= size.x;
		vertices[polygonData].vertices[i].uv.y *= size.y;
	}

	vertices[polygonData].vertBuff->Map(0, nullptr, (void**)&vertMap);

	// �S���_�ɑ΂���
	for (UINT i = 0; i < vertices[polygonData].vertices.size(); i++)
	{
		vertMap[i] = vertices[polygonData].vertices[i]; //���W���R�s�[
	}

	// �}�b�v������
	vertices[polygonData].vertBuff->Unmap(0, nullptr);
}

void DrawPolygon::MoveUV(const int& polygonData, const int& graphHandle, DirectX::XMFLOAT2 speed)
{
	if ((polygonData < 0 || (size_t)polygonData >= vertices.size()) ||
		(graphHandle < 0 || (size_t)graphHandle >= textrueData.size()))
	{
		return;
	}

	// �e�N�X�`���f�[�^�擾
	D3D12_RESOURCE_DESC resDesc = textrueData[graphHandle].texbuff->GetDesc();
	DirectX::XMFLOAT2 addUV = { speed.x / resDesc.Width, speed.y / resDesc.Height };

	for (size_t i = 0; i < vertices[polygonData].vertices.size(); i++)
	{
		vertices[polygonData].vertices[i].uv.x += addUV.x;
		vertices[polygonData].vertices[i].uv.y += addUV.y;
	}

	vertices[polygonData].vertBuff->Map(0, nullptr, (void**)&vertMap);

	// �S���_�ɑ΂���
	for (UINT i = 0; i < vertices[polygonData].vertices.size(); i++)
	{
		vertMap[i] = vertices[polygonData].vertices[i]; //���W���R�s�[
	}

	// �}�b�v������
	vertices[polygonData].vertBuff->Unmap(0, nullptr);
}

int DrawPolygon::DrawPolygonInit()
{
	HRESULT hr = S_FALSE;
	static bool isDrawPolygonInit = false;

	int index = (int)obj.size();
	hr = CreateConstBuffer(&index);
	if (FAILED(hr))
	{
		return EoF;
	}

	if (isDrawPolygonInit == false)
	{
		isDrawPolygonInit = true;

		if (LoadTextrue() == EoF)
		{
			return EoF;
		}
	}

	return index;
}

int DrawPolygon::DrawOBJInit()
{
	HRESULT hr = S_FALSE;

	int index = (int)obj.size();
	hr = CreateConstBuffer(&index);
	if (FAILED(hr))
	{
		return EoF;
	}

	if (isDrawOBJPolygonInit == false)
	{
		isDrawOBJPolygonInit = true;
	}

	return index;
}

int DrawPolygon::Draw(
	const int& polygonData, const XMFLOAT3& position, const XMMATRIX& rotation, const XMFLOAT3& scale,
	const XMFLOAT4& color, const int& graphHandle, const bool& isFill,
	const bool& isOrthoGraphic, const bool& viewMultiFlag, Object* parent)
{
	if ((polygonData < 0 || (size_t)polygonData >= vertices.size()) ||
		(graphHandle < 0 || (UINT)graphHandle > texCommonData.textrueCount)/* ||
		(parent < -1 && (parent != -1 && (size_t)parent >= obj.size()))*/)
	{
		return EoF;
	}

	using namespace DirectX;

	bool isInit = false;
	XMMATRIX mat = XMMatrixIdentity();

	if ((size_t)(polygonCount + 1) < objIndex.size())
	{
		isInit = true;
	}

	if (isInit == false)
	{
		int size = DrawPolygonInit();
		if (size == EoF)
		{
			return EoF;
		}

		objIndex.push_back({ size, graphHandle });

		if (isFill == false)
		{
			for (size_t i = 0; i < vertices[polygonData].vertices.size(); i++)
			{
				vertices[polygonData].vertices[i].normal = { -light.x, -light.y, -light.z };
			}
		}
	}

	if (objIndex.size() <= 0)
	{
		return EoF;
	}

	polygonCount++;
	polygonCount %= objIndex.size();

	IndexData& index = objIndex[polygonCount];

	UpdataConstant(position, rotation, scale, color, index.constant, polygonData, parent);

	if (index.textrue != graphHandle)
	{
		index.textrue = graphHandle;
	}

	if (viewMultiFlag == true)
	{
		mat = matView[cameraNo];
	}
	else
	{
		mat = XMMatrixIdentity();
	}

	if (isOrthoGraphic == true)
	{
		mat *= objectData[isDepthWriteBan].matProjection[CommonData::Projection::ORTHOGRAPHIC];
		lightVec = {
			-vertices[polygonData].vertices[0].normal.x,
			-vertices[polygonData].vertices[0].normal.y,
			-vertices[polygonData].vertices[0].normal.z
		};
	}
	else
	{
		mat *= objectData[isDepthWriteBan].matProjection[CommonData::Projection::PERSPECTIVE];
		lightVec = light;
	}

#pragma region GraphicsCommand

	BaseDrawGraphics();

	w->cmdList->SetPipelineState(objectData[isDepthWriteBan].pipelinestate[blendMode].Get());
	w->cmdList->SetGraphicsRootSignature(objectData[isDepthWriteBan].rootsignature.Get());

	ConstBufferData* constMap = nullptr;
	obj[index.constant].constBuff->Map(0, nullptr, (void**)&constMap); //�}�b�s���O

	constMap->color = color;
	constMap->mat = obj[index.constant].matWorld * mat;
	constMap->lightVec = lightVec;
	obj[index.constant].constBuff->Unmap(0, nullptr); //�}�b�s���O����

	// �f�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap* ppHeaps[] = { texCommonData.descHeap.Get() };
	w->cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// �萔�o�b�t�@�r���[���Z�b�g
	w->cmdList->SetGraphicsRootConstantBufferView(0, obj[index.constant].constBuff->GetGPUVirtualAddress());
	w->cmdList->SetGraphicsRootDescriptorTable(1, textrueData[index.textrue].gpuDescHandle);

	if (isFill == true)
	{
		// �v���~�e�B�u�`��̐ݒ�
		w->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else
	{
		// �v���~�e�B�u�`��̐ݒ�
		w->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	}

	// ���_�o�b�t�@�̐ݒ�
	w->cmdList->IASetVertexBuffers(0, 1, &vertices[polygonData].vbView);
	// �C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
	w->cmdList->IASetIndexBuffer(&vertices[polygonData].ibView);
	// �`��R�}���h
	w->cmdList->DrawIndexedInstanced((UINT)vertices[polygonData].indices.size(), 1, 0, 0, 0);

	return index.constant;
}

int DrawPolygon::DrawOBJ(const int& object, const XMFLOAT3& position, const XMMATRIX& rotation, const XMFLOAT3& scale,
	const XMFLOAT4& color, const bool& isOrthoGraphic, const bool& viewMultiFlag, Object* parent)
{
	if ((obj[object].polygonData < 0 || (size_t)obj[object].polygonData >= vertices.size()) ||
		(obj[object].material.textrueIndex < 0 || (UINT)obj[object].material.textrueIndex > texCommonData.textrueCount)/* ||
		(parent < -1 && (parent != -1 && (size_t)parent >= obj.size()))*/)
	{
		return EoF;
	}

	using namespace DirectX;

	bool isInit = false;
	XMMATRIX mat = XMMatrixIdentity();
	size_t parentIndex = object;

	for (int i = object; i >= 0; i--)
	{
		if ((size_t)(polygonCount + 1) < objIndex.size())
		{
			isInit = true;
		}

		if (isInit == false)
		{
			int size = DrawOBJInit();
			if (size == EoF)
			{
				return EoF;
			}

			objIndex.push_back({ size, (int)obj[i].material.textrueIndex });
		}

		if (obj[i].parent != nullptr)
		{
			parentIndex = i;
			break;
		}
	}

	if (objIndex.size() <= 0)
	{
		return EoF;
	}

	polygonCount++;
	polygonCount %= objIndex.size();

	IndexData& index = objIndex[polygonCount];

	for (int i = static_cast<int>(parentIndex); i <= object; i++)
	{
		if (i == parentIndex)
		{
			UpdataConstant(position, rotation, scale, color, index.constant, obj[i].polygonData, parent);
		}
		else
		{
			UpdataConstant(position, rotation, scale, color, index.constant, obj[i].polygonData, obj[i].parent);
		}
	}

	if (viewMultiFlag == true)
	{
		mat = matView[cameraNo];
	}
	else
	{
		mat = XMMatrixIdentity();
	}

	if (isOrthoGraphic == true)
	{
		mat *= materialData[isDepthWriteBan].matProjection[CommonData::Projection::ORTHOGRAPHIC];
		lightVec = {
			-vertices[obj[object].polygonData].vertices[0].normal.x,
			-vertices[obj[object].polygonData].vertices[0].normal.y,
			-vertices[obj[object].polygonData].vertices[0].normal.z
		};
	}
	else
	{
		mat *= materialData[isDepthWriteBan].matProjection[CommonData::Projection::PERSPECTIVE];
		lightVec = {
			-vertices[obj[object].polygonData].vertices[0].normal.x,
			-vertices[obj[object].polygonData].vertices[0].normal.y,
			-vertices[obj[object].polygonData].vertices[0].normal.z
		};
	}

#pragma region GraphicsCommand

	BaseDrawGraphics();

	w->cmdList->SetPipelineState(materialData[isDepthWriteBan].pipelinestate[blendMode].Get());
	w->cmdList->SetGraphicsRootSignature(materialData[isDepthWriteBan].rootsignature.Get());

	for (int i = static_cast<int>(parentIndex); i <= object; i++)
	{
		ConstBufferData* constMap = nullptr;
		obj[index.constant].constBuff->Map(0, nullptr, (void**)&constMap); //�}�b�s���O

		constMap->color = color;
		constMap->mat = obj[index.constant].matWorld * mat;
		constMap->lightVec = lightVec;
		obj[index.constant].constBuff->Unmap(0, nullptr); //�}�b�s���O����

		MaterialConstBufferData* materialConstMap = nullptr;
		obj[index.constant].materialConstBuff->Map(0, nullptr, (void**)&materialConstMap); //�}�b�s���O

		materialConstMap->ambient = obj[i].material.ambient;
		materialConstMap->diffuse = obj[i].material.diffuse;
		materialConstMap->specular = obj[i].material.specular;
		materialConstMap->alpha = obj[i].material.alpha;
		obj[index.constant].materialConstBuff->Unmap(0, nullptr); //�}�b�s���O����

		// �f�X�N���v�^�q�[�v���Z�b�g
		ID3D12DescriptorHeap* ppHeaps[] = { texCommonData.descHeap.Get() };
		w->cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		// �萔�o�b�t�@�r���[���Z�b�g
		w->cmdList->SetGraphicsRootConstantBufferView(0, obj[index.constant].constBuff->GetGPUVirtualAddress());
		w->cmdList->SetGraphicsRootConstantBufferView(1, obj[index.constant].materialConstBuff->GetGPUVirtualAddress());
		w->cmdList->SetGraphicsRootDescriptorTable(2, textrueData[obj[i].material.textrueIndex].gpuDescHandle);

		// �v���~�e�B�u�`��̐ݒ�
		w->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ���_�o�b�t�@�̐ݒ�
		w->cmdList->IASetVertexBuffers(0, 1, &vertices[obj[i].polygonData].vbView);
		// �C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
		w->cmdList->IASetIndexBuffer(&vertices[obj[i].polygonData].ibView);
		// �`��R�}���h
		w->cmdList->DrawIndexedInstanced((UINT)vertices[obj[i].polygonData].indices.size(), 1, 0, 0, 0);
	}

	return static_cast<int>(parentIndex);
}

int DrawPolygon::CreateCamera(const XMFLOAT3& cameraPos, const XMFLOAT3& cameraTarget, const XMFLOAT3& upVector)
{
	using namespace DirectX;

	matView.push_back(Camera::CreateCamera(
		XMLoadFloat3(&cameraPos),
		XMLoadFloat3(&cameraTarget),
		XMLoadFloat3(&upVector)
	));

	return (int)matView.size() - 1;
}

void DrawPolygon::SetCamera(const XMFLOAT3& cameraPos, const XMFLOAT3& cameraTarget, const XMFLOAT3& upVector,
	const int& cameraNo)
{
	if (cameraNo < 0 || (size_t)cameraNo >= matView.size())
	{
		return;
	}

	using namespace DirectX;

	matView[cameraNo] = Camera::CreateCamera(
		XMLoadFloat3(&cameraPos),
		XMLoadFloat3(&cameraTarget),
		XMLoadFloat3(&upVector)
	);
}

void DrawPolygon::ChangeCamera(const int& cameraNo)
{
	if (cameraNo < 0 || (size_t)cameraNo >= matView.size())
	{
		return;
	}

	this->cameraNo = cameraNo;
}

DirectX::XMFLOAT3 DrawPolygon::ScreenToWorld(int x, int y, float z)
{
	using namespace DirectX;

	XMFLOAT3 world = {};
	XMFLOAT3 screen = { (float)x, (float)y, z };
	XMMATRIX InvView, InvPrj, VP, InvViewport;

	InvView = XMMatrixInverse(nullptr, matView[cameraNo]);
	InvPrj = XMMatrixInverse(nullptr, objectData[isDepthWriteBan].matProjection[CommonData::Projection::PERSPECTIVE]);
	VP = XMMatrixIdentity();
	VP.r[0].m128_f32[0] = w->windowWidth / 2.0f;
	VP.r[1].m128_f32[1] = -(w->windowHeight) / 2.0f;
	VP.r[3].m128_f32[0] = w->windowWidth / 2.0f;
	VP.r[3].m128_f32[1] = w->windowHeight / 2.0f;
	InvViewport = XMMatrixInverse(nullptr, VP);

	XMMATRIX temp = InvViewport * InvPrj * InvView;
	XMStoreFloat3(&world, XMVector3TransformCoord(XMLoadFloat3(&screen), temp));

	return world;
}

DirectX::XMMATRIX DrawPolygon::InverseView()
{
	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(nullptr, matView[cameraNo]);
	invView.r[3].m128_f32[0] = 0.0f;
	invView.r[3].m128_f32[1] = 0.0f;
	invView.r[3].m128_f32[2] = 0.0f;
	return invView;
}

void DrawPolygon::PolygonLoopEnd()
{
	loopCount++;

	TextLoopEnd();

	polygonCount = static_cast<int>(objModelCount) - 1;
}

void DrawPolygon::DataClear()
{
	matView.clear();
	verticesCount.clear();
	material.clear();
}

void DrawPolygon::Circle(const XMFLOAT3& centerPos, const float& r, const size_t& divNum,
	const bool& flag, Vertex* v, unsigned short* index)
{
	using namespace DirectX;

	if (v == nullptr || index == nullptr)
	{
		return;
	}

	for (size_t i = 0; i < divNum; i++)
	{
		/*���_���W*/
		v[i].pos.x = r * sinf(2 * XM_PI / divNum * i) + centerPos.x;
		v[i].pos.y = r * cosf(2 * XM_PI / divNum * i) + centerPos.y;
		v[i].pos.z = centerPos.z;
		v[i].normal = {};
		v[i].uv.x = (sinf(2 * XM_PI / divNum * i) + 1.0f) / 2.0f;
		v[i].uv.y = (cosf(2 * XM_PI / divNum * i) + 1.0f) / 2.0f;

		if (v[i].pos.x >= r * -0.00000005f && v[i].pos.x <= r * 0.00000005f)
		{
			v[i].pos.x = 0;
		}
		if (v[i].pos.y >= r * -0.00000005f && v[i].pos.y <= r * 0.00000005f)
		{
			v[i].pos.y = 0;
		}
		if (v[i].uv.x >= -0.00000005f && v[i].uv.x <= 0.00000005f)
		{
			v[i].uv.x = 0;
		}
		if (v[i].uv.y >= -0.00000005f && v[i].uv.y <= 0.00000005f)
		{
			v[i].uv.y = 0;
		}

		if (flag == true)
		{
			/*fill�p�C���f�b�N�X�f�[�^*/
			index[i * 3 + 0] = (unsigned short)(i + 1);
			index[i * 3 + 1] = (unsigned short)(i + 0);
			index[i * 3 + 2] = (unsigned short)(divNum);
		}
		else
		{
			/*line�p�C���f�b�N�X�f�[�^*/
			index[i * 2 + 0] = (unsigned short)(i + 1);
			index[i * 2 + 1] = (unsigned short)(i + 0);
		}
	}

	// vertices��divNum�Ԗڂ�centerPos����
	v[divNum].pos = { centerPos.x, centerPos.y, centerPos.z };
	v[divNum].normal = {};
	v[divNum].uv = { 0.5f, 0.5f };

	for (size_t i = 0; i < divNum + 1; i++)
	{
		v[i].uv.y *= -1;
	}

	/*�C���f�b�N�X�f�[�^����*/
	if (flag == true)
	{
		/*fill�p�C���f�b�N�X�f�[�^*/
		index[(divNum - 1) * 3] = (unsigned short)0; //��ԍŌ�̎�O�̗v�f��'0'�ɒu������
	}
	else
	{
		/*line�p�C���f�b�N�X�f�[�^*/
		index[(divNum - 1) * 2] = (unsigned short)0; //��ԍŌ�̗v�f��'0'�ɒu������
	}

	return;
}
