#include "./Header/TestScene.h"
#include "./Header/Input.h"
#include "./Header/Camera.h"
#include "./Header/Color.h"

const float TestScene::sphereRadius = 10.0f;
const float TestScene::gravity = 9.8f;

TestScene::TestScene(SceneChenger* sceneChenger) :
	BaseScene(sceneChenger),
	background(-1),
	playerObject(-1),
	playerPos{},
	playerRota(EngineMath::Identity()),
	playerVel{},
	playerAccel{},
	playerAddForce{},
	playerInitPos{},
	playerInitVel{}
{
	Init();
}

void TestScene::Init()
{
	using namespace DirectX;
	using namespace EngineMath;

	// �摜�̓ǂݍ���
	if (background == -1)
	{
		background = draw.LoadTextrue(L"./Resources/background.png");
	}

	// �I�u�W�F�N�g�̐���
	if (playerObject == -1)
	{
		playerObject = draw.CreateSphere(sphereRadius, 16);
	}

	Camera::targetRadius = 150.0f;

	Camera::pos = { 0.0f, 0.0f, -Camera::targetRadius };
	Camera::target = { 0.0f, 0.0f, 0.0f };
	Camera::upVec = { 0.0f, 1.0f, 0.0f };
	draw.SetCamera(Camera::pos, Camera::target, Camera::upVec);

	playerInitPos = { -900.0f, -500.0f, 750.0f };
	playerInitVel = { 10.0f, 15.0f, 0.0f };

	playerPos = playerInitPos;
	playerVel = playerInitVel;
	playerAccel = { 0.0f, 0.0f, 0.0f };
}

void TestScene::Update()
{
	using namespace DirectX;
	using namespace EngineMath;

	if (Input::IsKey(DIK_R))
	{
		playerPos = playerInitPos;
		playerVel = playerInitVel;
		playerAccel = { 0.0f, 0.0f, 0.0f };
	}

	playerPos += playerVel;
	playerVel += playerAccel;
	playerAccel.y = -gravity / 60.0f;
}

void TestScene::Draw()
{
	using namespace DirectX;
	using namespace EngineMath;

	DirectXInit* w = DirectXInit::GetInstance();

	w->ClearScreen();
	draw.SetDrawBlendMode(BLENDMODE_ALPHA);

	// �w�i
	draw.DrawTextrue(
		0.0f,
		0.0f,
		(float)w->windowWidth,
		(float)w->windowHeight,
		0.0f,
		background,
		XMFLOAT2(0.0f, 0.0f)
	);

	draw.Draw(playerObject, playerPos, playerRota, scale_xyz(1.0f), Color::WHITE);

	draw.DrawTextrue(0.0f, 0.0f, 120.0f, 40.0f * (1 + 1), 0.0f, 0, XMFLOAT2(0.0f, 0.0f), Color::BLACK);
	draw.DrawString(0.0f, 40.0f * 0, 2.0f, Color::WHITE, "reset:R");
#ifdef _DEBUG
	draw.DrawTextrue(0.0f, 40.0f * (1 + 1), 320.0f, 40.0f * (2 + 1), 0.0f, 0, XMFLOAT2(0.0f, 0.0f), Color::BLACK);
	draw.DrawString(0.0f, 40.0f * (0 + 2), 2.0f, Color::WHITE, "posY:%f", playerPos.y);
	draw.DrawString(0.0f, 40.0f * (1 + 2), 2.0f, Color::WHITE, "velocityY:%f", playerVel.y);
	draw.DrawString(0.0f, 40.0f * (2 + 2), 2.0f, Color::WHITE, "accelY:%f", playerAccel.y);
#endif // _DEBUG

	// ���[�v�̏I������
	draw.PolygonLoopEnd();
	w->ScreenFlip();
}
