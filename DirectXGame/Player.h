#pragma once
#include "KamataEngine.h"
class MapChipField;

class Player {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();
	const KamataEngine ::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};

	MapChipField* mapChipField_ = nullptr;



	enum class LRDirection {
		kRight,
		kLeft,
	};
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	bool onGround_ = true;

	static inline const float kAcceleration = 0.05f;
	static inline const float kAttenuation = 0.1f;
	static inline const float kLimitRunSpeed = 0.5f;

	static inline const float kJumpAcceleration = 0.5f;
	static inline const float kGravityAcceleration = 0.05f;
	static inline const float kLimitFallSpeed = 1.0f;

	static inline const float kTimeTurn = 0.3f;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 隙間
	static inline const float kBlank = 0.1f;

	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;

		KamataEngine::Vector3 move;
	};

	void InputMove();
	
	void CheckMapCollision(CollisionMapInfo& info);
	void CheckMapCollisionUp(CollisionMapInfo& info);

	void CheckMapMove(const CollisionMapInfo& info);

	void CheckMapCeiling(const CollisionMapInfo& info);

	void CheckMapCollisionDown(CollisionMapInfo& info);

	void AnimateTurn();


	void CheckMapWall(const CollisionMapInfo& info);

	void CheckMapLanding(const CollisionMapInfo& info);

	void CheckMapCollisionRight( CollisionMapInfo& info);

	void CheckMapCollisionLeft( CollisionMapInfo& info);
	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  //  左下
		kRightTop,    //  右上
		kLeftTop,     //  左上

		kNumCorner // 要素数

	};
		
	
		KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

		

		  // 着地時の速度減衰率
	    static inline const float kAttenuationLanding = 0.1f;
	    // 落下用
	    static inline const float kGroundSearchHeight = 0.1f;
	    // 壁での速度減衰率
	    static inline const float kAttenuationWall = 0.1f;


	};

