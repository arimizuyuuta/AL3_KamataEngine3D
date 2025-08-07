#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
class Player;

class Enemy 
{
public:

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();
	
	AABB GetAABB();
	void OnCollision(const Player* player);

	private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	//歩行の速さ
	static inline const float kWalkSpeed_ = 0.1f;
	KamataEngine::Vector3 velocity_ = {};
	//最初の角度
	static inline const float kWalkMotionAngleStart = 0.1f;
	//最後の角度
	static inline const float kWalkMotionAngleEnd = 0.1f;
	//アニメーション周期時間
	static inline const float floatkWalkMotionTime = 0.1f;
	//経過時間
	float walkTimer_ = 0.0f;

	KamataEngine::Vector3 GetWorldPosition();
	
};
