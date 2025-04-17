#pragma once
#include "KamataEngine.h"

//ゲームシーン
class GameScene
{
public:
	//初期化
	void Initialize();

	//更新
	void Update();

	//描画
	void Draw();

	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	//3Dモデル
	KamataEngine::Model* model_ = nullptr;

	//ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	//カメラ
	KamataEngine::Camera camera_;

	//デストラクタ
	~GameScene();

};
