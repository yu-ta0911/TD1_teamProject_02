#include "Boss.h"
#include <Novice.h>
#include <cmath>

void Boss::Initialize(float playerY, int playerHeight) {
    leftTextureHandle_ = Novice::LoadTexture("./Resources/images/BossMoveLeft.png");
    rightTextureHandle_ = Novice::LoadTexture("./Resources/images/BossMoveRight.png");

    // 足音SEの読み込み
    stepSeHandle_ = Novice::LoadAudio("./Resources/sounds/BossFootMark.mp3");
    stepPlayHandle_ = -1;

    x_ = 1500.0f;
    y_ = (playerY + playerHeight) - kHeight_;

    animationFrame_ = 0;
    animationTimer_ = 0;
    isLeft_ = true;
    hitTimer_ = 0;
}

void Boss::Update(float playerX, int playerWidth) {
    if (hitTimer_ > 0) {
        hitTimer_--;
    }

    float bossCenterX = x_ + kWidth_ * 0.5f;
    float playerCenterX = playerX + playerWidth * 0.5f;
    float diffX = playerCenterX - bossCenterX;

    bool isMoving = false;

    if (std::abs(diffX) > kStopDistance_) {
        if (diffX < 0.0f) {
            x_ -= kSpeed_;
            isLeft_ = true;
        }
        else {
            x_ += kSpeed_;
            isLeft_ = false;
        }
        isMoving = true;
    }

    // 歩行アニメーション＆足音再生
    if (isMoving) {
        animationTimer_++;
        if (animationTimer_ >= kAnimationSpeed_) {
            animationTimer_ = 0;
            animationFrame_ = (animationFrame_ + 1) % 2;

            // 前の足音が鳴っていた場合止めてから新しい一歩の音を鳴らす
            if (stepSeHandle_ != -1) {
                if (stepPlayHandle_ != -1) {
                    Novice::StopAudio(stepPlayHandle_);
                }
                stepPlayHandle_ = Novice::PlayAudio(stepSeHandle_, false, 0.8f);
            }
        }
    }
    else {
        animationFrame_ = 0;
        animationTimer_ = 0;
    }
}

void Boss::Draw() {
    int handle = isLeft_ ? leftTextureHandle_ : rightTextureHandle_;
    unsigned int color = (hitTimer_ > 0) ? 0xFF4444FF : 0xFFFFFFFF;

    Novice::DrawQuad(
        static_cast<int>(x_), static_cast<int>(y_),
        static_cast<int>(x_ + kWidth_), static_cast<int>(y_),
        static_cast<int>(x_), static_cast<int>(y_ + kHeight_),
        static_cast<int>(x_ + kWidth_), static_cast<int>(y_ + kHeight_),
        animationFrame_ * kWidth_, 0,
        kWidth_, kHeight_,
        handle,
        color
    );
}

void Boss::OnHitBullet() {
    hitTimer_ = 10;
}