#pragma once
#include "base/Vector3.h"
#include "3d/Object3d.h"
#include "3d/Model.h"
#include "base/SafeDelete.h"

class BaseTarget
{
private:
	BaseTarget();
	~BaseTarget();

	//初期化
	virtual void Initialize(std::string modelName, Vector3 pos,int r);
	//更新
	virtual void Update();
	//描画
	virtual void Draw();

	//Set
	void SetPosition(Vector3 pos) { position = pos; }

	//Get
	Vector3 GetPosition() { return position; }
	Vector3 GetVelocity() { return velocity; }
	int GetR() { return r; }


private:

	Object3d* obj;
	Model* model;
	
	Vector3 position = { 0,0,0 };
	Vector3 velocity = { 0,0,0 };
	int r = 1;
};

