#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include <vector>

class GameScene {
public:
	void Initialize();
	void Update();
	void Draw();
	~GameScene();

private:
	void GenerateBlocks(); // ← 追加

	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;

	KamataEngine::Camera camera_;
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	Player* player_ = nullptr;
	Skydome* skydome_ = nullptr;
	MapChipField* mapChipField_ = nullptr; // ← 追加

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
};
