#pragma once
#include "KamataEngine.h"
#include <string>
#include <vector>

enum class MapChipType {
	kBlank, // 空白
	kBlock  // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
public:
	void LoadMapChipCsv(const std::string& filePath);
	void ResetMapChipData();

	// ユーティリティ
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) const;
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) const;

	// サイズ取得
	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;
};
