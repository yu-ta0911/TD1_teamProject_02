#pragma once

class Boss; // 前方宣言

class Player {
public:
    void Initialize();
    // 更新処理（ボスの当たり判定左端と画面幅を受け取る）
    void Update(const char* keys, const char* preKeys, Boss& boss, int windowWidth);
    // チュートリアル用更新処理（ボスなし・右端抜け可能）
    void UpdateTutorial(const char* keys, const char* preKeys, int windowWidth);
    void Draw();

    // ボス側から座標を参照するためのゲッター
    float GetX() const { return x_; }
    float GetY() const { return y_; }
    int GetWidth() const { return kWidth_; }
    int GetHeight() const { return kHeight_; }
    int GetHp() const { return hp_; }

private:
    void GetWireOrigin(float& outX, float& outY) const;

    const int kWidth_ = 64;
    const int kHeight_ = 64;
    const float kSpeed_ = 6.0f;
    const float kAirControl_ = 0.5f;
    const float kJumpPower_ = -15.0f;
    const float kGravity_ = 0.6f;
    const float kGroundY_ = 958.0f;
    const int kAnimationSpeed_ = 8;

    float x_ = 100.0f;
    float y_ = 958.0f;
    float velocityX_ = 0.0f;
    float velocityY_ = 0.0f;
    bool isGrounded_ = true;

    int hp_ = 5;
    int invincibleTimer_ = 0;
    const int kMaxInvincibleTime_ = 60;

    int animationFrame_ = 0;
    int animationTimer_ = 0;
    bool isLeft_ = false;

    int textureHandle_ = 0;
    int catchTextureHandle_ = 0;

    int wireAttachSeHandle_ = 0;
    int wireDetachSeHandle_ = 0;

    static const int kNumHookPoints_ = 16;
    float hookPointX_[kNumHookPoints_] = {
        0 * 64.0f + 32.0f,
        2 * 64.0f + 32.0f,
        3 * 64.0f + 32.0f,
        7 * 64.0f + 32.0f,
        9 * 64.0f + 32.0f,
        10 * 64.0f + 32.0f,
        13 * 64.0f + 32.0f,
        14 * 64.0f + 32.0f,
        16 * 64.0f + 32.0f,
        18 * 64.0f + 32.0f,
        20 * 64.0f + 32.0f,
        23 * 64.0f + 32.0f,
        24 * 64.0f + 32.0f,
        26 * 64.0f + 32.0f,
        27 * 64.0f + 32.0f,
        28 * 64.0f + 32.0f
    };
    float hookPointY_[kNumHookPoints_] = {
        128.0f, 128.0f, 128.0f,
        128.0f,
        128.0f, 128.0f,
        128.0f, 128.0f,
        128.0f,
        128.0f,
        128.0f,
        128.0f, 128.0f,
        128.0f, 128.0f, 128.0f
    };
    float hookPointRadius_ = 40.0f;
    int hoveredHookIndex_ = -1;
    bool isHoveringBoss_ = false;

    int anchorX_ = 0;
    int anchorY_ = 0;
    int hookTarget_ = 0;
    float hookAccel_ = 1.2f;

    float orbX_ = 0.0f;
    float orbY_ = 0.0f;
    float orbRadius_ = 8.0f;
    float orbSpeed_ = 12.0f;
    int orbStock_ = 0;
    static const int kMaxOrbStock_ = 3;

    static const int kMaxBullets_ = 3;
    float bulletX_[kMaxBullets_] = { 0.0f };
    float bulletY_[kMaxBullets_] = { 0.0f };
    float bulletVx_[kMaxBullets_] = { 0.0f };
    bool isBulletActive_[kMaxBullets_] = { false };
    float bulletRadius_ = 8.0f;
    float bulletSpeed_ = 16.0f;
};