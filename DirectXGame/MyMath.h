#pragma once
#include "KamataEngine.h"


KamataEngine::Matrix4x4 MakeAffineMatrix(KamataEngine::Vector3& scale, KamataEngine::Vector3& rotation, KamataEngine::Vector3& translation);
float EaselnOut(float x1, float x2, float t);
