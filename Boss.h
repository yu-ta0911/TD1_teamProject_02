#pragma once

class Boss {
public:
    void Initialize(float playerY, int playerHeight);
    void Update(float playerX, int playerWidth);
    void Draw();

    // 当たり判定の左端
    float GetHitLeft() const { return x_ + kHitOffsetLeft_; }
    float GetHitRight() const { return x_ + kWidth_ - kHitOffsetRight_; }
    float GetHitTop() const { return y_ + kHitOffsetTop_; }
    float GetHitBottom() const { return y_ + kHeight_ - kHitOffsetBottom_; }

    float GetX() const { return x_; }
    float GetY() const { return y_; }
    int GetWidth() const { return kWidth_; }
    int GetHeight() const { return kHeight_; }

    void OnHitBullet();

private:
    const int kWidth_ = 327;
    const int kHeight_ = 288;
    const float kHitOffsetLeft_ = 100.0f;
    const float kHitOffsetRight_ = 100.0f;
    const float kHitOffsetTop_ = 100.0f;
    const float kHitOffsetBottom_ = 30.0f;
    const float kStopDistance_ = 0.0f;
    const float kSpeed_ = 0.8f;

    // 歩行と足音の間隔（数値を大きくするほどゆっくりになる：45フレーム＝約0.75秒に1歩）
    const int kAnimationSpeed_ = 45;

    float x_ = 1500.0f;
    float y_ = 0.0f;

    int animationFrame_ = 0;
    int animationTimer_ = 0;
    bool isLeft_ = true;
    int hitTimer_ = 0;

    int leftTextureHandle_ = 0;
    int rightTextureHandle_ = 0;

    // ボスの足音SEハンドル
    int stepSeHandle_ = -1;
    int stepPlayHandle_ = -1;
};