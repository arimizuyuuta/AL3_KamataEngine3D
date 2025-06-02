#define NOMINMAX
#include "Player.h"
#include "MyMath.h"
#include <algorithm>
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {
	Input* input = Input::GetInstance();
	Vector3 acceleration = {};

	// --- 入力処理と向き変更 ---
	if (input->PushKey(DIK_RIGHT)) {
		if (lrDirection_ != LRDirection::kRight) {
			lrDirection_ = LRDirection::kRight;
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			turnTimer_ = kTimeTurn;
		}
		if (velocity_.x < 0.0f)
			velocity_.x *= (1.0f - kAttenuation);
		acceleration.x += kAcceleration;

	} else if (input->PushKey(DIK_LEFT)) {
		if (lrDirection_ != LRDirection::kLeft) {
			lrDirection_ = LRDirection::kLeft;
			turnFirstRotationY_ = worldTransform_.rotation_.y;
			turnTimer_ = kTimeTurn;
		}
		if (velocity_.x > 0.0f)
			velocity_.x *= (1.0f - kAttenuation);
		acceleration.x -= kAcceleration;

	} else {
		velocity_.x *= (1.0f - kAttenuation);
	}

	// --- 加速反映 ---
	velocity_.x += acceleration.x;
	velocity_.y += acceleration.y;
	velocity_.z += acceleration.z;

	// --- 最大速度制限 ---
	velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	// --- ジャンプ ---
	if (onGround_ && input->TriggerKey(DIK_UP)) {
		velocity_.y = kJumpAcceleration;
		onGround_ = false;
	}

	// --- 重力処理 ---
	if (!onGround_) {
		velocity_.y -= kGravityAcceleration;
		velocity_.y = std::max(velocity_.y, kLimitFallSpeed);
	}

	// --- 仮接地判定 ---
	bool landing = false;
	if (velocity_.y < 0.0f && worldTransform_.translation_.y <= 1.0f) {
		landing = true;
	}

	// --- 接地切替 ---
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
	} else {
		if (landing) {
			worldTransform_.translation_.y = 1.0f;
			velocity_.x *= (1.0f - kAttenuation); // ← 修正済
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}

	// --- 回転補間 ---
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		float t = 1.0f - std::clamp(turnTimer_ / kTimeTurn, 0.0f, 1.0f);
		t = t * t * (3 - 2 * t); // EaseInOut

		float table[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float target = table[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = std::lerp(turnFirstRotationY_, target, t);
	} else {
		float table[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		worldTransform_.rotation_.y = table[static_cast<uint32_t>(lrDirection_)];
	}

	// --- 移動反映と行列更新 ---
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }
