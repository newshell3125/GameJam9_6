#pragma once
#pragma once
#include "Vector3.h"
#include "Object3d.h"
#include "Model.h"
#include "SafeDelete.h"
#include "CollisionPrimitive.h"

class BaseTarget
{
private:
	BaseTarget();
	~BaseTarget();

	//初期化
	virtual void Initialize(std::string modelName, Vector3 pos, int radius);
	//更新
	virtual void Update();
	//描画
	virtual void Draw();

	//Set
	void SetPosition(Vector3 pos) { position = pos; }

	//Get
	Vector3 GetPosition() { return position; }
	Vector3 GetVelocity() { return velocity; }
	int GetR() { return sphere.radius; }
	Sphere GetSphere() { return sphere; }

private:

	Object3d* obj;
	Model* model;

	Sphere sphere;
	Vector3 position = { 0,0,0 };
	Vector3 velocity = { 0,0,0 };
};

