#include "Enemy.h"
#define NOMINMAX
#include "MapChipField.h"
#include "MyMath.h"
#include "Player.h"
#include <algorithm>
#include <numbers>
using namespace KamataEngine;
using namespace MathUtility;



void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) 

	{
		// NULLポインタチェック
		assert(model);
		// 引数として受け取ったデータをメンバ変数に記録する
		model_ = model;
		worldTransform_.translation_ = position;
		// 引数の内容をメンバ変数に記録
		camera = camera;
		// ワールド変換の初期化
		worldTransform_.Initialize();
	    // 初期回転
	    worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;
		camera_ = camera;

		 velocity_ = {-kWalkSpeed_,0,0};

		 walkTimer_ = 0.0f;


}

void Enemy::Update() {

	walkTimer_ += 1.0f / 60.0f;
	worldTransform_.translation_ += velocity_;

 // アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	worldTransform_.rotation_.x = std::sin(walkTimer_ * 5.0f);
}

void Enemy::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);

}
Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

	void Enemy::OnCollision(const Player * player) { 
		(void)player;
	}





AABB Enemy::GetAABB() {

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - 1.0f / 2.0f, worldPos.y - 1.0f / 2.0f, worldPos.z - 1.0f / 2.0f};
	aabb.max = {worldPos.x + 1.0f / 2.0f, worldPos.y + 1.0f / 2.0f, worldPos.z + 1.0f / 2.0f};

	return aabb;
}