#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include <algorithm>
#include <numbers>
using namespace KamataEngine;
using namespace MathUtility;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) 
{
// NULLポインタチェック
assert(model);
// 引数として受け取ったデータをメンバ変数に記録する
model_ = model;
worldTransform_.translation_ = position;
// 引数の内容をメンバ変数に記録
camera = camera;
// ワールド変換の初期化
worldTransform_.Initialize();
// 初期回転
worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

camera_ = camera;
}

// 更新
void Player::Update()
{
	//1
	 InputMove();

	// 2衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;
	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	// 3判定結果を反映して移動させる
	CheckMapMove(collisionMapInfo);

	// 4天井に接触している場合の処理
	CheckMapCeiling(collisionMapInfo);

	// 5壁に接触している場合の処理
	CheckMapWall(collisionMapInfo);
	// 6接地状態の切り替え
	CheckMapLanding(collisionMapInfo);
        //7旋回制御
	    AnimateTurn();



		// 着地フラグ
		bool landing = false;
		// 地面との当たり判定
		// 下降中?
	    if (velocity_.y < 0) {
		    // Y座標が地面以下になったら着地
		    if (worldTransform_.translation_.y <= 1.0f) {
			    landing = true;
		    }
	    }
	    // 接地判定
	    if (onGround_) {
		    // ジャンプ開始
		    if (velocity_.y > 0.0f) {
			    // 空中状態に移行
			    onGround_ = false;
		    }
	    } else {

		    // 着地
		    if (landing) {
			    // めり込み排斥
			    worldTransform_.translation_.y = 1.0f;
			    // 摩擦で横方向速度が減衰する
			    velocity_.x *= (1.0f - kAttenuation);
			    // 下方向速度をリセット
			    velocity_.y = 0.0f;
			    // 接地状態に移行
			    onGround_ = true;
		    }
	    }
	    

	   

		    // アフィン変換行列の作成
		    worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

		    // 行列を定数バッファに転送
		    worldTransform_.TransferMatrix();

			

			

			

	        

	        // 地面との当たり判定
	        // 下降中?
	        if (velocity_.y < 0) {
		        // Y座標が地面以下になったら着地
		        if (worldTransform_.translation_.y <= 1.0f) {
			        landing = true;
		        }
	        }

}

// 描画
void Player:: Draw()
{
                // 3Dモデルを描画
                model_->Draw(worldTransform_, *camera_);
}

void Player::InputMove() {

	 // 接地状態
	if (onGround_) {
		// 移動入力
		//  左右移動操作
		if (Input ::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			} else if (Input ::GetInstance()->PushKey(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection ::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			}
			// 加速/減速
			velocity_ += acceleration;
			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {

			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_ += Vector3(0, kJumpAcceleration, 0);
		}
	}
	// 空中
	else {

		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}



	}

void Player::CheckMapCollision(CollisionMapInfo& info) {

	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight (info);
	CheckMapCollisionLeft (info);
}

    void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	
		// 上昇あり?
	    if (info.move.y <= 0) {
		    return;
	    }
	    // 移動後の4つの角の座標
	    std ::array<Vector3, kNumCorner> positionsNew;

		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		    positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	    }
	    MapChipType mapChipType;
	    MapChipType mapChipTypeNext;
	    // 真上の当たり判定を行う
	    bool hit = false;
		// 左上点の判定
		MapChipField::IndexSet indexSet;
	    indexSet = mapChipField_ -> GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	    mapChipType = mapChipField_ -> GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	    mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	    if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		    hit = true;
	    }
	    // 右上点の判定
	    // kRightTopについて同様に判定する
	    indexSet = mapChipField_ -> GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	    mapChipType = mapChipField_ -> GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	    mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex , indexSet.yIndex + 1);
	    if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		    hit = true;
	    }
	    // ブロックにヒット?
	    if (hit) {
		    // めり込みを排除する方向に移動量を設定する
		    indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, +kHeight / 2.0f, 0));
		   
				
		    
		    // めり込み先ブロックの範囲矩形
		    MapChipField::Rect rect = mapChipField_ -> GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		    info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		    // 天井に当たったことを記録する
		    info.ceiling = true;
		    
	    }
		
	}
// ③判定結果を反映して移動させる
void Player::CheckMapMove(const CollisionMapInfo& info) {
// 移動
    worldTransform_.translation_ += info.move;
}

    void Player::CheckMapCeiling(const CollisionMapInfo& info) { 
		// 天井に当たった?
	    if (info.ceiling) {
		    DebugText::GetInstance()->ConsolePrintf("hitceiling\n");
		    velocity_.y = 0;
	    }
	}


	void Player::CheckMapCollisionDown(CollisionMapInfo& info) 
	{

		// 下降あり?
		if (info.move.y >= 0) {
			return;
		}
		// 移動後の4つの角の座標
		std::array<Vector3, kNumCorner> positionsNew;
		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
		}
		MapChipType mapChipType;
	    MapChipType mapChipTypeNext;
		// 真下の当たり判定を行う
		bool hit = false;
		// 左下点の判定
		MapChipField::IndexSet indexSet;
		indexSet = mapChipField_ -> GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		mapChipType = mapChipField_ -> GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	    mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	    if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			hit = true;
		}
		// 右下点の判定
		// kRightBottomについて同様に判定する
		indexSet = mapChipField_ -> GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		mapChipType = mapChipField_ -> GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	    mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex , indexSet.yIndex -1);
	    if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			hit = true;
		}
		// ブロックにヒット?
		if (hit) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move - Vector3(0, -kHeight / 2.0f, 0));
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_ -> GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
			// 地面に当たったことを記録する
			info.landing = true;
	    }
	}
    // マップ衝突チェック 右
    void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	    // 右移動あり？
	    if (info.move.x <= 0) {
		    return;
	    }

	    // 移動後の4つの角の座標
	    std::array<Vector3, kNumCorner> positionsNew;

	    for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		    positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	    }

	    MapChipType mapChipType;
	    MapChipType mapChipTypeNext;
	    // 右の当たり判定を行う
	    bool hit = false;
	    // 右上点の判定
	    MapChipField::IndexSet indexSet;
	    indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	    mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	    if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		    hit = true;
	    }
	    // 右下点の判定
	    indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	    mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	    if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		    hit = true;
	    }

	    // ブロックにヒット？
	    if (hit) {
		    // めり込みを排除する方向に移動量を設定する
		    indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0));
		    // めり込み先ブロックの範囲矩形
		    MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		    info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		    // 壁に当たったことを判定結果に記録する
		    info.hitWall = true;
	    }
    }

	// マップ衝突チェック 左
    void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	    // 左移動あり？
	    if (info.move.x >= 0) {
		    return;
	    }

	    // 移動後の4つの角の座標
	    std::array<Vector3, kNumCorner> positionsNew;

	    for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		    positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	    }

	    MapChipType mapChipType;
	    MapChipType mapChipTypeNext;
	    // 左の当たり判定を行う
	    bool hit = false;
	    // 左上点の判定
	    MapChipField::IndexSet indexSet;
	    indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	    mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	    if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		    hit = true;
	    }
	    // 左下点の判定
	    indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	    mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	    if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		    hit = true;
	    }

	    // ブロックにヒット？
	    if (hit) {
		    // めり込みを排除する方向に移動量を設定する
		    indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0));
		    // めり込み先ブロックの範囲矩形
		    MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		    info.move.x = std::min(0.0f, rect.right - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));

		    // 壁に当たったことを判定結果に記録する
		    info.hitWall = true;
	    }
    }



void Player::CheckMapWall(const CollisionMapInfo& info) {

	if (info.hitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

// ⑥接地状態の切り替え処理
void Player::CheckMapLanding(const CollisionMapInfo& info) {
	// 自キャラが接地状態？
	if (onGround_) {
		// 接地状態の処理

		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {

			// 落下判定

			// 移動後の4つの角の座標
			std::array<Vector3, kNumCorner> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}

			MapChipType mapChipType;
			// 真上の当たり判定を行う
			bool hit = false;
			// 左下点の判定
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			//	落下なら空中状態に切り替え
			// 落下開始
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	} else {
		// 空中状態の処理

		// 着地フラグ
		if (info.landing) {
			// 着地状態に切り替える（落下を止める）
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		}
	}
}
// ⑦旋回制御
void Player::AnimateTurn() {
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}
}
Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

Vector3 Player::GetWorldPosition() { 
Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::OnCollision(const Enemy* enemy) {

	(void)enemy;
	velocity_ += Vector3(0,1,0);
	isDead_ = true;
}



AABB Player::GetAABB() { 

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kHeight / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kHeight / 2.0f};

	return aabb;
}




