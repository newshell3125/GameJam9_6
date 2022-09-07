#pragma once
#include "ParticleManager.h"
#include "Text.h"
#include "DebugCamera.h"
#include "LightGroup.h"
#include "XinputControll.h"
#include "Sprite.h"
#include "Object3d.h"

#include "CollisionPrimitive.h"

#include "BaseScene.h"

#include "BaseTarget.h"

class TestScene :
    public BaseScene
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private: // �ÓI�����o�ϐ�
	static const int texNumber = 0;

public:
	TestScene();
	~TestScene();

	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio) override;

	void Update() override;

	void Draw() override;

private:
	Text* text;

	LightGroup* lightGroup = nullptr;
	float ambientColor0[3] = { 1,1,1 };

	ParticleManager* particleMan = nullptr;

	float centerX = 1280 / 2;
	float centerY = 720 / 2;

	XMFLOAT2 mousePos;

	Object3d* object = nullptr;
	Model* model = nullptr;

	Ray ray;
	Sphere sphere;

	BaseTarget* target = nullptr;
};

