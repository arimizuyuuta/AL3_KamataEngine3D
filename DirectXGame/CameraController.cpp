#define NOMINMAX
#include "Player.h"
#include "CameraController.h"
using namespace KamataEngine;
using namespace MathUtility;
void CameraController::Initialize() { 
	camera_.Initialize(); 
}

void CameraController::Update() {
const WorldTransform& targetWorldTransform = target_ -> GetWorldTransform();
// 追従対象とオフセットからカメラの座標を計算
camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
// 行列を更新する
camera_.UpdateMatrix();
}

void CameraController::Reset() { const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;



}
