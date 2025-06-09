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

const Vector3& targetVelocity = target_->GetVelocity();

// 追従対象とオフセットからカメラの座標を計算
	targetPosition_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;

camera_.translation_.x = Lerp(camera_.translation_.x, targetPosition_.x, kInterpolationRate);
camera_.translation_.y = Lerp(camera_.translation_.y, targetPosition_.y, kInterpolationRate);

camera_.translation_.x = std::max(camera_.translation_.x, camera_.translation_.x + targetMargin.left);
camera_.translation_.x = std::min(camera_.translation_.x, camera_.translation_.x + targetMargin.right);
camera_.translation_.y = std::max(camera_.translation_.y, camera_.translation_.y + targetMargin.bottom);
camera_.translation_.y = std::min(camera_.translation_.y, camera_.translation_.y + targetMargin.top);

// 移動範囲制限    
camera_.translation_.x = std::max(camera_.translation_.x, movableArea_.left);
camera_.translation_.x = std::min(camera_.translation_.x, movableArea_.right);
camera_.translation_.y = std::max(camera_.translation_.y, movableArea_.bottom);
camera_.translation_.y = std::min(camera_.translation_.y, movableArea_.top);

// 行列を更新する
camera_.UpdateMatrix();
}

void CameraController::Reset() { const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;



}
