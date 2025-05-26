#include "Skydome.h"
using namespace KamataEngine;

void Skydome::Initialize(Model* model, uint32_t, Camera* camera) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
}

void Skydome::Update() { worldTransform_.TransferMatrix(); }

void Skydome::Draw() { model_->Draw(worldTransform_, *camera_); }
