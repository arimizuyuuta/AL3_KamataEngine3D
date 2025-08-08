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
	assert(model);
	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	
	// 引数の内容をメンバ変数に記録
	camera = camera;

	

	// ワールド変換の初期化
	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}
}

void DeathParticles::Update() {
	
	for (WorldTransform& worldTransform : worldTransforms_) {
		// アフィン変換行列の作成
	worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);

	// 行列を定数バッファに転送
	worldTransform.TransferMatrix();

	}


}


void DeathParticles::Draw() {
	// 3Dモデルを描画
	
	for (WorldTransform& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *camera_);
	}

}