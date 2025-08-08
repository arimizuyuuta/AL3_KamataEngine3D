#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include<array>
#include <numbers>

class DeathParticles {

	public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	bool IsFinished() const { return isFinished_; }
	private:
	
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	static inline const uint32_t kNumParticles = 8;

	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;
	//存在時間
	static inline const float kDuration = 1.0f;
	//速さ
	static inline const float kSpeed = 0.05f;
	//角度
	static inline const float kAngleUnit = 2.0f * std::numbers::pi_v<float> / kNumParticles;
	//終了フラグ
	bool isFinished_ = false;
	//経過時間カウント
	float counter_ = 0.0f;

	KamataEngine::ObjectColor objectColor_;

	KamataEngine::Vector4 color_;
};
