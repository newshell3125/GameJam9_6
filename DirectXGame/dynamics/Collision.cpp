﻿#include "Collision.h"
#include<cmath>

using namespace DirectX;

void Collision::ClosestPtPoint2Triangle(const DirectX::XMVECTOR& point, const Triangle& triangle, DirectX::XMVECTOR* closest)
{
	// pointがp0の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	XMVECTOR p0_p2 = triangle.p2 - triangle.p0;
	XMVECTOR p0_pt = point - triangle.p0;

	float d1 = XMVector3Dot(p0_p1,p0_pt).m128_f32[0];
	float d2 = XMVector3Dot(p0_p2,p0_pt).m128_f32[0];

	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		// p0が最近傍
		*closest = triangle.p0;
		return;
	}

	// pointがp1の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p1_pt = point - triangle.p1;

	float d3 = XMVector3Dot(p0_p1,p1_pt).m128_f32[0];
	float d4 = XMVector3Dot(p0_p2,p1_pt).m128_f32[0];

	if (d3 >= 0.0f && d4 <= d3)
	{
		// p1が最近傍
		*closest = triangle.p1;
		return;
	}

	// pointがp0_p1の辺領域の中にあるかどうかチェックし、あればpointのp0_p1上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		*closest = triangle.p0 + v * p0_p1;
		return;
	}

	// pointがp2の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p2_pt = point - triangle.p2;

	float d5 = XMVector3Dot(p0_p1,p2_pt).m128_f32[0];
	float d6 = XMVector3Dot(p0_p2,p2_pt).m128_f32[0];
	if (d6 >= 0.0f && d5 <= d6)
	{
		*closest = triangle.p2;
		return;
	}

	// pointがp0_p2の辺領域の中にあるかどうかチェックし、あればpointのp0_p2上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		*closest = triangle.p0 + w * p0_p2;
		return;
	}

	// pointがp1_p2の辺領域の中にあるかどうかチェックし、あればpointのp1_p2上に対する射影を返す
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		return;
	}

	// pointは面領域の中にある。closestを重心座標を用いて計算する
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;
}

bool Collision::CheckSphere2Sphere(const Sphere & sphereA, const Sphere & sphereB, DirectX::XMVECTOR*inter)
{
	// 中心点の距離の２乗 <= 半径の和の２乗　なら交差
	float dist = XMVector3LengthSq(sphereA.center - sphereB.center).m128_f32[0];

	float radius2 = sphereA.radius + sphereB.radius;
	radius2 *= radius2;

	if (dist <= radius2) {
		if (inter) {
			// Aの半径が0の時座標はBの中心　Bの半径が0の時座標はAの中心　となるよう補完
			float t = sphereB.radius / (sphereA.radius + sphereB.radius);
			*inter = XMVectorLerp(sphereA.center, sphereB.center, t);
		}
		return true;
	}

	return false;
}

bool Collision::CheckSphere2Plane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR* inter)
{
	// 座標系の原点から球の中心座標への距離から
	// 平面の原点距離を減算することで、平面と球の中心との距離が出る
	float dist = XMVector3Dot(sphere.center, plane.normal).m128_f32[0] - plane.distance;
	// 距離の絶対値が半径より大きければ当たっていない
	if (fabsf(dist) > sphere.radius)	return false;

	// 擬似交点を計算
	if (inter)
	{
		// 平面上の再接近点を、疑似交点とする
		*inter = -dist * plane.normal + sphere.center;                                                                                              + sphere.center;
	}

	return true;
}

bool Collision::CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, DirectX::XMVECTOR* inter)
{
	XMVECTOR p;
	// 球の中心に対する最近接点である三角形上にある点pを見つける
	ClosestPtPoint2Triangle(sphere.center, triangle, &p);
	// 点pと球の中心の差分ベクトル
	XMVECTOR v = p - sphere.center;
	// 距離の二乗を求める
	//（同じベクトル同士の内積は三平方の定理のルート内部の式と一致する）
	float distanceSquare = XMVector3Dot(v, v).m128_f32[0];
	// 球と三角形の距離が半径以下なら当たっていない
	if (distanceSquare > sphere.radius * sphere.radius)	return false;
	// 擬似交点を計算
	if (inter) {
		// 三角形上の最近接点pを疑似交点とする
		*inter = p;
	}
	return true;
}

bool Collision::CheckRay2Plane(const Ray & lay, const Plane & plane, float*distance, DirectX::XMVECTOR * inter)
{
	const float epsilon = 1.0e-5f;	// 誤差吸収用の微小な値

	float d1 = XMVector3Dot(plane.normal, lay.dir).m128_f32[0];
	// 裏面には当たらない
	if (d1 > -epsilon) {
		return false;
	}
	
	float d2 = XMVector3Dot(plane.normal, lay.start).m128_f32[0];
	float t = (plane.distance - d2) / d1;

	if (t < 0) return false;

	// 距離を書き込む
	if (distance) {
		*distance = t;
	}

	// 交点を計算
	if (inter) {
		*inter = lay.start + t * lay.dir;
	}

	return true;
}

bool Collision::CheckRay2Triangle(const Ray & lay, const Triangle & triangle, float*distance, DirectX::XMVECTOR * inter)
{
	// 三角形が乗っている平面を算出
	Plane plane;
	XMVECTOR interPlane;
	plane.normal = triangle.normal;
	plane.distance = XMVector3Dot(triangle.normal, triangle.p0).m128_f32[0];
	// レイと平面が当たっていなければ、当たっていない	
	if (!CheckRay2Plane(lay, plane, distance, &interPlane)) {
		return false;
	}
	// レイと平面が当たっていたので、距離と交点が書き込まれた

	// レイと平面の交点が三角形の内側にあるか判定
	const float epsilon = 1.0e-5f;	// 誤差吸収用の微小な値
	XMVECTOR m;
	// 辺p0_p1について
	XMVECTOR pt_p0 = triangle.p0 - interPlane;
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	m = XMVector3Cross(pt_p0, p0_p1);
	// 辺の外側
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) {
		return false;
	}

	// 辺p1_p2について
	XMVECTOR pt_p1 = triangle.p1 - interPlane;
	XMVECTOR p1_p2 = triangle.p2 - triangle.p1;
	m = XMVector3Cross(pt_p1, p1_p2);
	// 辺の外側
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) {
		return false;
	}

	// 辺p2_p0について
	XMVECTOR pt_p2 = triangle.p2 - interPlane;
	XMVECTOR p2_p0 = triangle.p0 - triangle.p2;
	m = XMVector3Cross(pt_p2, p2_p0);
	// 辺の外側
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) {
		return false;
	}

	if (inter) {
		*inter = interPlane;
	}

	// 内側なので、当たっている
	return true;
}

bool Collision::CheckRay2Sphere(const Ray & lay, const Sphere & sphere, float*distance, DirectX::XMVECTOR * inter)
{
	XMVECTOR m = lay.start - sphere.center;
	float b = XMVector3Dot(m, lay.dir).m128_f32[0];
	float c = XMVector3Dot(m, m).m128_f32[0] - sphere.radius * sphere.radius;
	// layの始点がsphereの外側にあり(c > 0)、layがsphereから離れていく方向を
	// 差している場合(b > 0)、当たらない
	if (c > 0.0f && b > 0.0f) {
		return false;
	}

	float discr = b * b - c;
	// 負の判別式はレイが球を外れていることに一致
	if (discr < 0.0f) {
		return false;
	}

	// レイは球と交差している。
	// 交差する最小の値tを計算
	float t = -b - sqrtf(discr);
	// tが負である場合、レイは球の内側から開始しているのでtをゼロにクランプ
	if (t < 0) t = 0.0f;
	
	if (distance) {
		*distance = t;
	}

	if (inter) {
		*inter = lay.start + t * lay.dir;
	}

	return true;
}

bool Collision::CheckBox2D(const XMFLOAT2& OBJ1Pos, const XMFLOAT2& OBJ2Pos, const XMFLOAT2& OBJ1size, const XMFLOAT2& OBJ2size)
{
	// オブジェクト同士の距離 (絶対値)
	XMFLOAT2 distance = { abs(OBJ1Pos.x - OBJ2Pos.x),abs(OBJ1Pos.y - OBJ2Pos.y) };
	// サイズの大きさの合計値
	XMFLOAT2 size = { (OBJ1size.x + OBJ2size.x ) / 2 ,(OBJ1size.y + OBJ2size.y) / 2 };

	// ボックス同士のヒット確認
	if (distance.x < size.x && distance.y < size.y)
	{
		return true;
	}

	return false;
}

bool Collision::CheckBox3D(const XMFLOAT3& OBJ1Pos, const XMFLOAT3& OBJ2Pos, const XMFLOAT3& OBJ1size, const XMFLOAT3& OBJ2size)
{
	// オブジェクト同士の距離 (絶対値)
	XMFLOAT3 distance = { abs(OBJ1Pos.x - OBJ2Pos.x),abs(OBJ1Pos.y - OBJ2Pos.y), abs(OBJ1Pos.z - OBJ2Pos.z) };
	// サイズの大きさの合計値
	XMFLOAT3 size = { OBJ1size.x + OBJ2size.x,OBJ1size.y + OBJ2size.y,OBJ1size.z + OBJ2size.z };

	// ボックス同士のヒット確認
	if (distance.x < size.x && distance.y < size.y && distance.z <= size.z)
	{
		return true;
	}

	return false;
}

bool Collision::CheckBox2D(const XMFLOAT2& OBJ1Pos, const XMFLOAT2& OBJ2Pos,
	const XMFLOAT2& OBJ1size, const XMFLOAT2& OBJ2size,
	const float OBJ1colsize, const float OBJ2colsize)
{
	// オブジェクト同士の距離 (絶対値)
	XMFLOAT2 distance = { abs(OBJ1Pos.x + OBJ1size.x * ((1 - OBJ1colsize) / 2) - OBJ2Pos.x + OBJ2size.x * ((1 - OBJ2colsize) / 2))
						 ,abs(OBJ1Pos.y + OBJ1size.y * ((1 - OBJ1colsize) / 2) - OBJ2Pos.y + OBJ2size.y * ((1 - OBJ2colsize) / 2)) };
	// サイズの大きさの合計値
	XMFLOAT2 size = { (OBJ1size.x * OBJ1colsize + OBJ2size.x * OBJ2colsize) / 2,
					  (OBJ1size.y * OBJ1colsize + OBJ2size.y * OBJ2colsize) / 2};

	// ボックス同士のヒット確認
	if (distance.x < size.x && distance.y < size.y)
	{
		return true;
	}

	return false;
}

bool Collision::IsCollidingDistance(Rect rect_a, Rect rect_b)
{
	// 矩形の中心座標を割り出す(座標原点は左上)
	XMFLOAT2 center_a = {
		rect_a.X + (rect_a.Width / 2.0f),
		rect_a.Y + (rect_a.Height / 2.0f)
	};
	XMFLOAT2 center_b = {
		rect_b.X + rect_b.Width / 2.0f,
		rect_b.Y + rect_b.Height / 2.0f
	};

	// 距離を割り出す
	XMFLOAT2 distance = {
		center_a.x - center_b.x,
		center_a.y - center_b.y
	};

	if (distance.x < 0.0f)
	{
		distance.x *= -1.0f;
	}

	if (distance.y < 0.0f)
	{
		distance.y *= -1.0f;
	}

	// サイズの和を割り出す
	XMFLOAT2 size_sum = {
		(rect_a.Width + rect_b.Width) / 2.0f,
		(rect_a.Height + rect_b.Height) / 2.0f
	};

	// 判定(距離 < サイズの合計なら当たり)
	if (distance.x < size_sum.x &&
		distance.y < size_sum.y)
	{
		return true;
	}

	return false;
}