#pragma once
#include "KamataEngine.h"
#include "Skydome.h"
#include "player.h"
#include <vector>
class GameScene {
public:
	void Initialize();
	void Update();
	void Draw();
	~GameScene();

	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;

	KamataEngine::Camera camera_;
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	Player* player_ = nullptr;
	Skydome* skydome_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
};
