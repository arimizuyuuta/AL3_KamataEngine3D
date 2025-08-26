#include "GameScene.h"
#include "MyMath.h"
#include "CameraController.h"
#include"Player.h"
using namespace KamataEngine;

void GameScene::Initialize() {

	modelBlock_ = Model::CreateFromOBJ("block");
	modelPlayer_ = Model::CreateFromOBJ("player");
	modelEnemy_ = Model::CreateFromOBJ("enemy");
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle");
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	modelTitleFont_ = Model::CreateFromOBJ("titleFont");
	camera_.Initialize();
	camera_.farZ = 1000.0f;
	camera_.UpdateMatrix();

	// フェードインから開始
	phase_ = Phase::FadeIn;
	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status:: FadeIn, 1.0f);

	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(5, 18);
	player_-> Initialize(modelPlayer_, &camera_, playerPosition);

	debugCamera_ = new DebugCamera(1280, 720);

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, 0, &camera_);

	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	GenerateBlocks(); // ← ブロック生成をここで呼び出す

	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();
	CameraController ::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);

	player_->SetMapChipField(mapChipField_);
	

	for (int32_t i = 0; i < 2; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(50 + i, 18);

		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);

		enemies_.push_back(newEnemy);
	}

	
	phase_ = Phase::kPlay;

}

void GameScene::GenerateBlocks() {
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal, nullptr);
	}

	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* wt = new WorldTransform();
				wt->Initialize();
				wt->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				worldTransformBlocks_[i][j] = wt;
			}
		}
	}

	

	
}
//更新
void GameScene::Update() {
	

	// フェーズ変更
	ChangePhase();

	// フェーズによる処理
	switch (phase_) {
	case Phase::kPlay:
		// 全ての当たり判定を行う
		CheckAllCollisions();
		break;
	case Phase::kDeath:
		// デスパーティクルの更新
		deathParticles_->Update();
		break;
	case Phase::kFadeIn:
		// フェード
		fade_->Update();
		break;
	case Phase::kFadeOut:
		// フェード
		fade_->Update();
		break;
	}
	// 共通の処理
	// 自キャラの更新
	player_->Update();
	// スカイドームの更新
	skydome_->Update();
	// カメラコントローラ
	cameraController_->Update();

	for (const auto& line : worldTransformBlocks_) {
		for (WorldTransform* wt : line) {
			if (!wt)
				continue;
			wt->matWorld_ = MakeAffineMatrix(wt->scale_, wt->rotation_, wt->translation_);
			wt->TransferMatrix();
		}
	}
	
	if (Input::GetInstance()->TriggerKey(DIK_O)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.matView = cameraController_ -> GetViewProjection().matView;
		camera_.matProjection = cameraController_ -> GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	}

	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}
	

	

	switch (phase_) {
	case Phase::kPlay:
		// 全ての当たり判定を行う
		CheckAllCollisions();
		break;
	case Phase::kDeath:
		if (deathParticles_) {
			deathParticles_->Update();
		}
		break;
	}
	// デスパーティクルが終了したらシーンを終了する
	if (deathParticles_ && deathParticles_ -> IsFinished()) {

		finished = true;
	}

	ChangePhase();
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	skydome_->Draw();

	for (const auto& line : worldTransformBlocks_) {
		for (WorldTransform* wt : line) {
			if (!wt)
				continue;
			modelBlock_->Draw(*wt, camera_);
		}
	}
	player_->Draw();
	
	

	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}
	if (deathParticles_) {
		deathParticles_->Draw();
	}
	Model::PostDraw();

	
}

void GameScene::CheckAllCollisions() {

	#pragma region {
	AABB aabb1, aabb2;

	aabb1 = player_->GetAABB();

	for (Enemy* enemy : enemies_) {
		aabb2 = enemy->GetAABB();

		if (IsCollision(aabb1, aabb2)) {

			// 当たり判定があった場合の処理
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
}
void GameScene::ChangePhase() { 
	switch (phase_) {
	case Phase::kPlay:
		// ゲームプレイフェーズの処理
		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自手ャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelDeathParticles_, &camera_, deathParticlesPosition);
		}
		break;
	case Phase::kDeath:
		if (deathParticles_->IsFinished()) {
			// フェードアウト開始
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			break;
		case Phase::kFadeIn:
			if (fade_->IsFinished()) {
				// ゲームプレイへ
				phase_ = Phase::kPlay;
			}
			break;
		case Phase::kFadeOut:
			// シーン終了
			if (fade_->IsFinished()) {
				finished = true;
			}
			break;
		}
	}



			const Vector3& playerPosition = player_->GetWorldPosition();
	
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize(modelDeathParticle_, &camera_, playerPosition);

		}
		break;
	case Phase::kDeath:


		break;
	}

}
#pragma endregion



GameScene::~GameScene() {
	delete modelBlock_;
	delete modelSkydome_;
	delete modelPlayer_;
	delete player_;
	delete skydome_;
	delete debugCamera_;
	delete mapChipField_;

	for (auto& line : worldTransformBlocks_) {
		for (auto& wt : line) {
			delete wt;
		}
	}
	worldTransformBlocks_.clear();

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}


}







