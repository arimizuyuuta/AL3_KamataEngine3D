#include "Player.h"

using namespace KamataEngine;

void Player::Initialize(Model* model, uint32_t textureHandle, Camera* camera) {
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	camera_ = camera;
}


void Player::Update() {
//行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() { 
model_->Draw(worldTransform_, *camera_, textureHandle_);
}



