#include "MapChipField.h"
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>

using namespace KamataEngine;

namespace {
const std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}

void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& row : mapChipData_.data) {
		row.resize(kNumBlockHorizontal, MapChipType::kBlank);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	ResetMapChipData();

	std::ifstream file(filePath);
	assert(file.is_open());

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	for (uint32_t y = 0; y < kNumBlockVirtical; ++y) {
		std::string line;
		if (!std::getline(buffer, line)) {
			break;
		}

		std::istringstream lineStream(line);
		for (uint32_t x = 0; x < kNumBlockHorizontal; ++x) {
			std::string word;
			if (!std::getline(lineStream, word, ',')) {
				break;
			}

			auto it = mapChipTable.find(word);
			if (it != mapChipTable.end()) {
				mapChipData_.data[y][x] = it->second;
			}
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) const {
	if (xIndex >= kNumBlockHorizontal || yIndex >= kNumBlockVirtical) {
		return MapChipType::kBlank;
	}
	return mapChipData_.data[yIndex][xIndex];
}

Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) const {
	return Vector3(kBlockWidth * static_cast<float>(xIndex), kBlockHeight * static_cast<float>(kNumBlockVirtical - 1 - yIndex), 0.0f);
}
