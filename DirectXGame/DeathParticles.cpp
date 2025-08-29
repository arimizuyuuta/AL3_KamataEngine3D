#include "DeathParticles.h"
#define NOMINMAX
#include "MapChipField.h"
#include "Player.h"
#include <algorithm>
#include <numbers>
using namespace KamataEngine;
using namespace MathUtility;


void DeathParticles::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// NULLポインタチェック
	//assert(model);
	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	
	// 引数の内容をメンバ変数に記録
	camera_ = camera;

	

	// ワールド変換の初期化
	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};
}

void DeathParticles::Update() {
	
	for (WorldTransform& worldTransform : worldTransforms_) {
		// アフィン変換行列の作成
	worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);

	// 行列を定数バッファに転送
	worldTransform.TransferMatrix();

	}

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		//基本速度ベクトル
		Vector3 velocity = {kSpeed, 0, 0};
		//回転角計算
		float angle = kAngleUnit * i;
		//z回転行列
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);

		velocity = Transform(velocity, matrixRotation);

		worldTransforms_[i].translation_ += velocity;

		

		if (counter_ >= kDuration) {
			counter_ = kDuration;
			//終了
			isFinished_ = true;
		}
	}
	if (isFinished_) {
		return;
		
	}
	counter_ += 1.0f / 60.f;
	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);

	objectColor_.SetColor(color_);
}


void DeathParticles::Draw() {
	// 3Dモデルを描画
	
	for (WorldTransform& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *camera_,&objectColor_);
	}
	if (isFinished_) {
		return;
	}
}