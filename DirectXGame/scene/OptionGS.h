#pragma once
#include "Camera.h"
#include "Input.h"
#include "Audio.h"
#include "Sprite.h"
#include <array>

class OptionGS
{
public:
	OptionGS(Input* input, Camera* camera, Audio* audio);
	~OptionGS();

	void Initialize();
	void Update();
	void Draw();

	void ChangeIsOption();
	bool GetIsOption() { return isOption; }

private:
	Input* input = nullptr;
	Camera* camera = nullptr;
	Audio* audio = nullptr;

	// 設定用UI
	Sprite* option_base = nullptr;
	std::array<Sprite*, 3> option_bar = { nullptr, nullptr, nullptr };
	std::array<float, 3> bar_ratio = { 0,0,0 };

	// オプション中か
	bool isOption = false;

	// 設定項目
	float fov = 0;
	float sensi = 0;

	// 指定用
	DirectX::XMFLOAT2 mousePos;

};

