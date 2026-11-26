#pragma once

static const int kMapWidth = 30;
static const int kMapHeight = 17;
static const int kTileSize = 64;

class Map {
public:
    void Initialize();
    void Draw();

private:
    int map_[kMapHeight][kMapWidth] = { 0 };
    int blockHandle_ = 0;
};