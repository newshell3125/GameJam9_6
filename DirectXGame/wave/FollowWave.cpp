#include "FollowWave.h"

using namespace DirectX;

FollowWave::FollowWave()
{
}

FollowWave::~FollowWave()
{
	delete target;
	delete ui;
}

void FollowWave::Initialize(Input* input, Camera* camera)
{
	// uiの初期化
	BaseWave::Initialize(input, camera);

	// ここに初期化処理
	{
		target = new BaseTarget();
		target->Initialize("sphere");
	}
}

void FollowWave::Update()
{
	// ここに更新処理
	{


		target->Update();
	}

	// uiの更新
	BaseWave::Update();
}

void FollowWave::Draw()
{
	// ここに描画処理
	{
		target->Draw();
	}
}

void FollowWave::DrawUi(ID3D12GraphicsCommandList* cmdList)
{
	// uiの描画
	BaseWave::DrawUi(cmdList);
}
