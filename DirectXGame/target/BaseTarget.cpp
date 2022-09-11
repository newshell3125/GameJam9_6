#include "BaseTarget.h"
#include "ModelManager.h"

BaseTarget::BaseTarget()
{
}

BaseTarget::~BaseTarget()
{
	safe_delete(obj);
}

//初期化
void BaseTarget::Initialize(std::string modelName, XMFLOAT3 pos,XMFLOAT3 vel, float radius)
{
	//object3D関連初期化
	obj = Object3d::Create(ModelManager::GetInstance()->GetModel(SPHERE));
	obj->SetPosition(pos);

	position = pos;
	velocity = vel;
	sphere.center = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	sphere.radius = radius;
}

//更新
void BaseTarget::Update()
{
	position.x += velocity.x;
	position.y += velocity.y;
	position.z += velocity.z;

	sphere.center = DirectX::XMVectorSet(position.x, position.y, position.z, 0);

	obj->SetPosition(position);
	obj->SetScale(scale);

	sphere.radius = scale.x;
	
	//Object3D更新
	obj->Update();
}

//描画
void BaseTarget::Draw()
{
	if(!isDead)
	{
		obj->Draw();
	}
}
