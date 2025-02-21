#pragma once
#include "DirectXCommon.h"
#include "Input.h"
#include "Audio.h"
#include <DirectXMath.h>

struct SceneButton
{
	DirectX::XMFLOAT2 position = { 0,0 };
	DirectX::XMFLOAT2 scale = { 0,0 };
};

class BaseScene
{
public:
	BaseScene();
	virtual ~BaseScene();

	virtual void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);

	virtual void Update() = 0;

	virtual void Draw() = 0;

	BaseScene* GetNextScene() { return nextScene; }

protected:
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;

	BaseScene* nextScene = nullptr;
};
