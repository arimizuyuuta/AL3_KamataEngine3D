#pragma once

#include "KamataEngine.h"
#include "MapChipField.h"

class Player;

class CameraController {
public:

	void Initialize();
	void Update();

	KamataEngine::Camera camera_;

	Player* target_ = nullptr;
	void SetTarget(Player* target) { target_ = target; }
	void Reset();
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};

	const KamataEngine ::Camera& GetViewProjection() const { return camera_; }
};
