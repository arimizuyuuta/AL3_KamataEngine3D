#include "GameScene.h"
#include "MyMath.h"

using namespace KamataEngine;

void GameScene::Initialize() {

	modelBlock_ = Model::CreateFromOBJ("block");
	modelPlayer_ = Model::CreateFromOBJ("player");
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	camera_.Initialize();
	camera_.farZ = 1000.0f;
	camera_.UpdateMatrix();

	player_ = new Player();
	player_->Initialize(modelPlayer_, 0, &camera_);

	debugCamera_ = new DebugCamera(1280, 720);

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, 0, &camera_);

	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	GenerateBlocks(); // ← ブロック生成をここで呼び出す

	// プレイヤーの初期位置を左下あたりにセット
	player_->worldTransform_.translation_ = mapChipField_->GetMapChipPositionByIndex(1, 18);

	// 行列更新
	player_->worldTransform_.matWorld_ = MakeAffineMatrix(player_->worldTransform_.scale_, player_->worldTransform_.rotation_, player_->worldTransform_.translation_);
	player_->worldTransform_.TransferMatrix();


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

void GameScene::Update() {
	player_->Update();
	skydome_->Update();

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
		camera_.UpdateMatrix();
	}
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

	Model::PostDraw();
}

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
}







