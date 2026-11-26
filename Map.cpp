#include "Map.h"
#include <Novice.h>
#include <fstream>
#include <sstream>
#include <string>

void Map::Initialize() {
    blockHandle_ = Novice::LoadTexture("./Resources/images/Tile.png");
    if (blockHandle_ == -1) {
        blockHandle_ = Novice::LoadTexture("./resource/images/tile.png");
    }

    // 配列の初期化
    for (int y = 0; y < kMapHeight; y++) {
        for (int x = 0; x < kMapWidth; x++) {
            map_[y][x] = -1;
        }
    }

    // CSVファイルの読み込み
    const char* csvPaths[] = {
        "./Resources/stage.csv",
        "./Resource/stage.csv",
        "./Resources/map/stage.csv",
        "./Resource/map/stage.csv",
        "./resource/stage.csv",
        "../Resources/stage.csv",
        "../Resource/stage.csv"
    };

    std::ifstream file;
    for (int i = 0; i < 7; i++) {
        file.open(csvPaths[i]);
        if (file.is_open()) {
            break;
        }
        file.clear();
    }

    int loadedCount = 0;
    if (file.is_open()) {
        std::string line;
        int y = 0;

        while (std::getline(file, line) && y < kMapHeight) {
            std::stringstream ss(line);
            std::string cell;
            int x = 0;

            while (std::getline(ss, cell, ',') && x < kMapWidth) {
                if (!cell.empty()) {
                    map_[y][x] = std::stoi(cell);
                    if (map_[y][x] == 1 || map_[y][x] == 2) {
                        loadedCount++;
                    }
                }
                x++;
            }
            y++;
        }
    }

    // CSVが開けなかった場合の自動バックアップ配置
    if (loadedCount == 0) {
        for (int x = 0; x < kMapWidth; x++) {
            map_[0][x] = 1;               // 天井のレンガ
            map_[kMapHeight - 1][x] = 1;  // 床のレンガ
        }
        for (int y = 1; y < kMapHeight - 1; y++) {
            map_[y][kMapWidth - 1] = 1;   // 右壁のレンガ
        }
        int stalactiteCols[] = { 0, 2, 3, 7, 9, 10, 13, 14, 16, 18, 20, 23, 24, 26, 27, 28 };
        for (int i = 0; i < 16; i++) {
            map_[1][stalactiteCols[i]] = 2;
        }
    }
}

void Map::Draw() {
    for (int y = 0; y < kMapHeight; y++) {
        for (int x = 0; x < kMapWidth; x++) {
            if (map_[y][x] == 1) {
                int drawX = x * kTileSize;
                int drawY = y * kTileSize;
                Novice::DrawQuad(
                    drawX, drawY,
                    drawX + kTileSize, drawY,
                    drawX, drawY + kTileSize,
                    drawX + kTileSize, drawY + kTileSize,
                    64, 0,
                    64, 64,
                    blockHandle_,
                    0xFFFFFFFF
                );
            }
            else if (map_[y][x] == 2) {
                int drawX = x * kTileSize;
                int drawY = y * kTileSize;
                Novice::DrawQuad(
                    drawX, drawY,
                    drawX + kTileSize, drawY,
                    drawX, drawY + kTileSize,
                    drawX + kTileSize, drawY + kTileSize,
                    128, 0,
                    64, 64,
                    blockHandle_,
                    0xFFFFFFFF
                );
            }
        }
    }
}