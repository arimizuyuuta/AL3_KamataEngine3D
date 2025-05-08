#include "GameScene.h"
#include "MyMath.h"
using namespace KamataEngine;

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");

	// 3Dモデルの生成
	model_ = Model::Create();
	modelBlock_ = Model::CreateFromOBJ("cube");
	
	//カメラの初期化
	camera_.Initialize();
	//自キャラの生成
	player_ = new Player();
	//自キャラの初期化
	player_->Initialize(model_,textureHandle_,&camera_);

	debugCamera_ = new DebugCamera(1280, 720);

	    // 要素数
	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;
	// ブロック1個分の横幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		// 1列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}
	// ブロックの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if ((i+j) % 2 == 0)
				continue;
			worldTransformBlocks_[i][j] = new WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();
			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
		}
	}
	}

void GameScene::Update() {
	    player_->Update();

	    // ブロックの更新
	    for (std ::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		    for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			    if (!worldTransformBlock)
				    continue;
			    worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			    // 定数バッファに転送する
			    worldTransformBlock->TransferMatrix();
		    }
	    }
	    debugCamera_->Update();
	    if (Input ::GetInstance()->TriggerKey(DIK_O)) {
		    isDebugCameraActive_ = !isDebugCameraActive_;
	    }
	    if (isDebugCameraActive_) {

		    // デバッグカメラの更新
		    debugCamera_->Update();
		    // デバッグカメラのビュー行列;
		    camera_.matView = debugCamera_->GetCamera().matView;
		    // デバッグカメラのプロジェクション行列;
		    camera_.matProjection = debugCamera_->GetCamera().matProjection;
		    camera_.TransferMatrix();
	    } else {
		    camera_.UpdateMatrix();
	    }
    }



void GameScene::Draw() { 
	

	//DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	//3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());
	for (std ::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, camera_);

		}
		
	}
	// 自キャラの更新
	player_->Draw();
	
	//3Dモデル描画後処理
	Model::PostDraw();

	
}

GameScene::~GameScene() 
{
	 delete model_;
	 //自キャラの解放
	delete player_;
	 delete modelBlock_;
	for (std ::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

		worldTransformBlocks_.clear();

		delete debugCamera_;
	}







