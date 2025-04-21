#pragma once
#include"KamataEngine.h"
class Player {
public:

	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera);

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	uint32_t textureHandle_ = 0u;



	void Update();

	void Draw();

	KamataEngine::Camera* camera_ = nullptr;

};
