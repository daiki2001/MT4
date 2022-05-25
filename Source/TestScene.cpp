#include "./Header/TestScene.h"
#include "./Header/Input.h"
#include "./Header/Camera.h"
#include "./Math/Collision/Collision.h"
#include "./Header/Color.h"

const float TestScene::gravity = 9.8f;
const float TestScene::airResistance = 0.005f;

TestScene::TestScene(SceneChenger* sceneChenger) :
	BaseScene(sceneChenger),
	background(-1),
	playerObject{},
	playerDrawObject(-1),
	cubeObject{},
	cubeDrawObject(-1),
	isHit(false)
{
	Init();
}

void TestScene::Init()
{
	using namespace DirectX;
	using namespace Engine::Math;

	// 画像の読み込み
	if (background == -1)
	{
		background = draw.LoadTextrue(L"./Resources/background.png");
	}

	// オブジェクトの生成
	if (playerDrawObject == -1)
	{
		playerDrawObject = draw.CreateSphere(1.0f, 16);
	}
	if (cubeDrawObject == -1)
	{
		cubeDrawObject = draw.CreateSphere(1.0f, 6);
	}

	Camera::targetRadius = 150.0f;

	Camera::pos = { 0.0f, 0.0f, -Camera::targetRadius };
	Camera::target = { 0.0f, 0.0f, 0.0f };
	Camera::upVec = { 0.0f, 1.0f, 0.0f };
	draw.SetCamera(Camera::pos, Camera::target, Camera::upVec);

	playerObject.mass = 10.0f;
	playerObject.restitution = 0.0f;
	playerObject.initPos = { -600.0f, 0.0f, 500.0f };
	playerObject.initVel = { 20.0f, 0.0f, 0.0f };
	playerObject.pos = playerObject.initPos;
	playerObject.vel = playerObject.initVel;
	playerObject.accel = { 0.0f, 0.0f, 0.0f };

	cubeObject.mass = 20.0f;
	cubeObject.restitution = 1.0f;
	cubeObject.initPos = { 150.0f, 0.0f, 500.0f };
	cubeObject.initVel = { -10.0f, 0.0f, 0.0f };
	cubeObject.pos = cubeObject.initPos;
	cubeObject.vel = cubeObject.initVel;
	cubeObject.accel = { 0.0f, 0.0f, 0.0f };
}

void TestScene::Update()
{
	using namespace DirectX;
	using namespace Engine::Math;

	playerObject.pos += playerObject.vel;
	playerObject.vel += playerObject.accel;
	playerObject.accel = -airResistance * playerObject.vel; //空気抵抗
	//playerObject.accel.y += (-gravity / 60.0f);             //重力

	cubeObject.pos += cubeObject.vel;
	cubeObject.vel += cubeObject.accel;
	cubeObject.accel = -airResistance * cubeObject.vel; //空気抵抗

	if (Input::IsKey(DIK_R))
	{
		playerObject.pos = playerObject.initPos;
		playerObject.vel = playerObject.initVel;
		playerObject.accel = { 0.0f, 0.0f, 0.0f };

		cubeObject.pos = cubeObject.initPos;
		cubeObject.vel = cubeObject.initVel;
		cubeObject.accel = { 0.0f, 0.0f, 0.0f };
	}

	isHit = Collision::IsSphereToSphereCollision(
		Sphere(playerObject.pos, playerObject.mass),
		Sphere(cubeObject.pos, cubeObject.mass)
	);

	if (isHit)
	{
		playerObject.pos -= playerObject.vel;
		cubeObject.pos -= cubeObject.vel;
		CollisionPhysics(&playerObject, &cubeObject);
	}
}

void TestScene::Draw()
{
	using namespace DirectX;
	using namespace Engine::Math;

	static XMFLOAT4 objectColor = Color::WHITE;
	DirectXInit* w = DirectXInit::GetInstance();

	if (isHit)
	{
		objectColor = Color::RED;
	}
	else
	{
		objectColor = Color::WHITE;
	}

	w->ClearScreen();
	draw.SetDrawBlendMode(BLENDMODE_ALPHA);

	// 背景
	draw.DrawTextrue(
		0.0f,
		0.0f,
		(float)w->windowWidth,
		(float)w->windowHeight,
		0.0f,
		background,
		XMFLOAT2(0.0f, 0.0f)
	);

	draw.Draw(playerDrawObject, playerObject.pos, playerObject.rota, scale_xyz(playerObject.mass), objectColor);
	draw.Draw(cubeDrawObject, cubeObject.pos, cubeObject.rota, scale_xyz(cubeObject.mass), objectColor);

	draw.DrawTextrue(0.0f, 0.0f, 120.0f, 40.0f * (0 + 1), 0.0f, 0, XMFLOAT2(0.0f, 0.0f), Color::BLACK);
	draw.DrawString(0.0f, 40.0f * 0, 2.0f, Color::WHITE, "reset:R");
#ifdef _DEBUG
	draw.DrawTextrue(0.0f, 40.0f * (0 + 1), 700.0f, 40.0f * (5 + 1), 0.0f, 0, XMFLOAT2(0.0f, 0.0f), Color::BLACK);
	draw.DrawString(0.0f, 40.0f * (0 + 1), 2.0f, Color::WHITE, "vel1(x:%f,y:%f,z:%f)",
					playerObject.vel.x, playerObject.vel.y, playerObject.vel.z);
	draw.DrawString(0.0f, 40.0f * (1 + 1), 2.0f, Color::WHITE, "mass1:%f", playerObject.mass);
	draw.DrawString(0.0f, 40.0f * (2 + 1), 2.0f, Color::WHITE, "restitution1:%f", playerObject.restitution);
	draw.DrawString(0.0f, 40.0f * (3 + 1), 2.0f, Color::WHITE, "vel2(x:%f,y:%f,z:%f)",
					cubeObject.vel.x, cubeObject.vel.y, cubeObject.vel.z);
	draw.DrawString(0.0f, 40.0f * (4 + 1), 2.0f, Color::WHITE, "mass2:%f", cubeObject.mass);
	draw.DrawString(0.0f, 40.0f * (5 + 1), 2.0f, Color::WHITE, "restitution2:%f", cubeObject.restitution);
#endif // _DEBUG

	// ループの終了処理
	draw.PolygonLoopEnd();
	w->ScreenFlip();
}
