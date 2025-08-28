#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "CameraController.h"
#include "Player.h"
#include "Skydome.h"
#include "Enemy.h"
#include "DeathParticles.h"
#include "Fade.h"
#include <vector>

class GameScene {
public:
	void Initialize();
	void Update();
	void Draw();
	void CheckAllCollisions();
	void ChangePhase();
	~GameScene();

	// 終了フラグのgetter
	bool IsFinished() const { return finished; }

	// 終了フラグ(変数名に注意)
	bool finished = false;

private:
	void GenerateBlocks(); // ← 追加
	
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelEnemy_ = nullptr;
	KamataEngine::Model* modelDeathParticle_ = nullptr;
	KamataEngine::Model* modelTitleFont_ = nullptr;
	KamataEngine::Camera camera_;
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	Player* player_ = nullptr;
	Skydome* skydome_ = nullptr;
	MapChipField* mapChipField_ = nullptr; // ← 追加
	CameraController* cameraController_ = nullptr;
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	std::list<Enemy*> enemies_;

	DeathParticles* deathParticles_ = nullptr;

	KamataEngine::Model* modelGoal_ = nullptr;
	KamataEngine::WorldTransform worldTransformGoal_;


	

	// シーンのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kPlay,    // ゲームプレイ
		kDeath,   // デス演出
		kFadeOut, // フェードアウト
	};
	Phase phase_;
	// フェード
	Fade* fade_ = nullptr;
	};
