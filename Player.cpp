#include "Player.h"
#include "Map.h" // kTileSize参照用
#include "Boss.h"
#include <Novice.h>
#include <cmath>

void Player::Initialize() {
    textureHandle_ = Novice::LoadTexture("./Resources/images/playermove.png");
    catchTextureHandle_ = Novice::LoadTexture("./Resources/images/playerCatch.png");
    wireAttachSeHandle_ = Novice::LoadAudio("./Resources/sounds/wire.mp3");
    wireDetachSeHandle_ = Novice::LoadAudio("./Resources/sounds/wireCut.mp3");

    x_ = 100.0f;
    y_ = kGroundY_;
    velocityX_ = 0.0f;
    velocityY_ = 0.0f;
    isGrounded_ = true;

    hp_ = 5;
    invincibleTimer_ = 0;

    animationFrame_ = 0;
    animationTimer_ = 0;
    isLeft_ = false;

    hoveredHookIndex_ = -1;
    hookTarget_ = 0;
    orbStock_ = 0;
    for (int i = 0; i < kMaxBullets_; i++) {
        isBulletActive_[i] = false;
    }
}

void Player::GetWireOrigin(float& outX, float& outY) const {
    if (isLeft_) {
        outX = x_ + kWidth_ * (1.0f - 0.84f);
    }
    else {
        outX = x_ + kWidth_ * 0.84f;
    }
    outY = y_ + kHeight_ * 0.14f;
}

void Player::UpdateTutorial(const char* keys, const char* preKeys, int windowWidth) {
    bool isMoving = false;

    // 左移動・右移動
    if (isGrounded_) {
        if (keys[DIK_A]) {
            velocityX_ = -kSpeed_;
            isMoving = true;
            isLeft_ = true;
        }
        else if (keys[DIK_D]) {
            velocityX_ = kSpeed_;
            isMoving = true;
            isLeft_ = false;
        }
        else {
            velocityX_ *= 0.8f;
        }
    }
    else {
        if (keys[DIK_A] && velocityX_ > -kSpeed_) {
            velocityX_ -= kAirControl_;
            if (velocityX_ < -kSpeed_) {
                velocityX_ = -kSpeed_;
            }
            isLeft_ = true;
        }
        if (keys[DIK_D] && velocityX_ < kSpeed_) {
            velocityX_ += kAirControl_;
            if (velocityX_ > kSpeed_) {
                velocityX_ = kSpeed_;
            }
            isLeft_ = false;
        }
    }

    // ジャンプ
    if (preKeys[DIK_W] == 0 && keys[DIK_W] != 0 && isGrounded_) {
        velocityY_ = kJumpPower_;
        isGrounded_ = false;
    }

    // マウス座標と鍾乳洞ターゲット判定
    int mouseX = 0;
    int mouseY = 0;
    Novice::GetMousePosition(&mouseX, &mouseY);

    hoveredHookIndex_ = -1;
    isHoveringBoss_ = false;
    if (hookTarget_ == 0) {
        float closestDist = hookPointRadius_;
        for (int i = 0; i < kNumHookPoints_; i++) {
            float toMouseX = static_cast<float>(mouseX) - hookPointX_[i];
            float toMouseY = static_cast<float>(mouseY) - hookPointY_[i];
            float mouseDist = sqrtf(toMouseX * toMouseX + toMouseY * toMouseY);
            if (mouseDist <= closestDist) {
                closestDist = mouseDist;
                hoveredHookIndex_ = i;
            }
        }
    }

    // 左クリックでワイヤー発射
    if (Novice::IsTriggerMouse(0) != 0 && hookTarget_ == 0) {
        if (hoveredHookIndex_ != -1) {
            anchorX_ = static_cast<int>(hookPointX_[hoveredHookIndex_]);
            anchorY_ = static_cast<int>(hookPointY_[hoveredHookIndex_]);
            hookTarget_ = 1;
            Novice::PlayAudio(wireAttachSeHandle_, false, 0.5f);
        }
    }

    // 左クリックを離したら解除
    if (hookTarget_ == 1 && Novice::IsPressMouse(0) == 0) {
        hookTarget_ = 0;
        Novice::PlayAudio(wireDetachSeHandle_, false, 0.5f);
    }

    float centerX = x_ + kWidth_ * 0.5f;
    float centerY = y_ + kHeight_ * 0.5f;

    if (hookTarget_ == 1) {
        float dx = static_cast<float>(anchorX_) - centerX;
        float dy = static_cast<float>(anchorY_) - centerY;
        float distance = sqrtf(dx * dx + dy * dy);

        isLeft_ = (dx < 0.0f);

        if (distance > 1.0f) {
            float dirX = dx / distance;
            float dirY = dy / distance;
            velocityX_ += dirX * hookAccel_;
            velocityY_ += dirY * hookAccel_;
        }

        velocityX_ *= 0.95f;
        velocityY_ *= 0.95f;

        if (velocityY_ < 0.0f) {
            isGrounded_ = false;
        }
    }
    else {
        velocityX_ *= 0.99f;
    }

    // 重力と座標更新
    velocityY_ += kGravity_;
    x_ += velocityX_;
    y_ += velocityY_;

    // 画面外制限・床判定
    isGrounded_ = false;

    if (y_ >= kGroundY_) {
        y_ = kGroundY_;
        velocityY_ = 0.0f;
        isGrounded_ = true;
    }
    if (y_ < 0.0f) {
        y_ = 0.0f;
        if (velocityY_ < 0.0f) {
            velocityY_ = 0.0f;
        }
    }
    if (x_ < 0.0f) {
        x_ = 0.0f;
        if (velocityX_ < 0.0f) {
            velocityX_ = 0.0f;
        }
    }
    float maxRight = static_cast<float>(windowWidth - kWidth_ - kTileSize);
    if (x_ > maxRight) {
        x_ = maxRight;
        if (velocityX_ > 0.0f) {
            velocityX_ = 0.0f;
        }
    }

    // アニメーション更新
    if (isMoving && isGrounded_) {
        animationTimer_++;
        if (animationTimer_ >= kAnimationSpeed_) {
            animationTimer_ = 0;
            animationFrame_++;
            if (animationFrame_ >= 4) {
                animationFrame_ = 0;
            }
        }
    }
    else {
        animationFrame_ = 0;
        animationTimer_ = 0;
    }
}

void Player::Update(const char* keys, const char* preKeys, Boss& boss, int windowWidth) {
    bool isMoving = false;

    if (invincibleTimer_ > 0) {
        invincibleTimer_--;
    }

    // 左移動・右移動
    if (isGrounded_) {
        if (keys[DIK_A]) {
            velocityX_ = -kSpeed_;
            isMoving = true;
            isLeft_ = true;
        }
        else if (keys[DIK_D]) {
            velocityX_ = kSpeed_;
            isMoving = true;
            isLeft_ = false;
        }
        else {
            velocityX_ *= 0.8f;
        }
    }
    else {
        if (keys[DIK_A] && velocityX_ > -kSpeed_) {
            velocityX_ -= kAirControl_;
            if (velocityX_ < -kSpeed_) {
                velocityX_ = -kSpeed_;
            }
            isLeft_ = true;
        }
        if (keys[DIK_D] && velocityX_ < kSpeed_) {
            velocityX_ += kAirControl_;
            if (velocityX_ > kSpeed_) {
                velocityX_ = kSpeed_;
            }
            isLeft_ = false;
        }
    }

    // ジャンプ
    if (preKeys[DIK_W] == 0 && keys[DIK_W] != 0 && isGrounded_) {
        velocityY_ = kJumpPower_;
        isGrounded_ = false;
    }

    // 弾発射
    if (preKeys[DIK_SPACE] == 0 && keys[DIK_SPACE] != 0 && orbStock_ > 0) {
        for (int i = 0; i < kMaxBullets_; i++) {
            if (!isBulletActive_[i]) {
                bulletX_[i] = isLeft_ ? x_ : (x_ + kWidth_);
                bulletY_[i] = y_ + kHeight_ * 0.5f;
                bulletVx_[i] = isLeft_ ? -bulletSpeed_ : bulletSpeed_;
                isBulletActive_[i] = true;
                orbStock_--;
                break;
            }
        }
    }

    float bossLeft = boss.GetHitLeft();
    float bossRight = boss.GetHitRight();
    float bossTop = boss.GetHitTop();
    float bossBottom = boss.GetHitBottom();

    for (int i = 0; i < kMaxBullets_; i++) {
        if (isBulletActive_[i]) {
            bulletX_[i] += bulletVx_[i];

            if (bulletX_[i] < -bulletRadius_ || bulletX_[i] > static_cast<float>(windowWidth) + bulletRadius_) {
                isBulletActive_[i] = false;
                continue;
            }

            if (bulletX_[i] + bulletRadius_ > bossLeft &&
                bulletX_[i] - bulletRadius_ < bossRight &&
                bulletY_[i] + bulletRadius_ > bossTop &&
                bulletY_[i] - bulletRadius_ < bossBottom) {
                isBulletActive_[i] = false;
                boss.OnHitBullet();
            }
        }
    }

    // マウス座標とターゲット判定
    int mouseX = 0;
    int mouseY = 0;
    Novice::GetMousePosition(&mouseX, &mouseY);

    hoveredHookIndex_ = -1;
    isHoveringBoss_ = false;
    if (hookTarget_ == 0) {
        float closestDist = hookPointRadius_;
        for (int i = 0; i < kNumHookPoints_; i++) {
            float toMouseX = static_cast<float>(mouseX) - hookPointX_[i];
            float toMouseY = static_cast<float>(mouseY) - hookPointY_[i];
            float mouseDist = sqrtf(toMouseX * toMouseX + toMouseY * toMouseY);
            if (mouseDist <= closestDist) {
                closestDist = mouseDist;
                hoveredHookIndex_ = i;
            }
        }

        isHoveringBoss_ = (mouseX >= bossLeft && mouseX <= bossRight &&
            mouseY >= bossTop && mouseY <= bossBottom);
    }

    // 左クリックでワイヤー発射
    if (Novice::IsTriggerMouse(0) != 0 && hookTarget_ == 0) {
        if (hoveredHookIndex_ != -1) {
            anchorX_ = static_cast<int>(hookPointX_[hoveredHookIndex_]);
            anchorY_ = static_cast<int>(hookPointY_[hoveredHookIndex_]);
            hookTarget_ = 1;
            Novice::PlayAudio(wireAttachSeHandle_, false, 0.5f);
        }
        else if (isHoveringBoss_) {
            orbX_ = (bossLeft + bossRight) * 0.5f;
            orbY_ = (bossTop + bossBottom) * 0.5f;
            hookTarget_ = 2;
            Novice::PlayAudio(wireAttachSeHandle_, false, 0.5f);
        }
    }

    // 左クリックを離したら解除
    if (hookTarget_ == 1 && Novice::IsPressMouse(0) == 0) {
        hookTarget_ = 0;
        Novice::PlayAudio(wireDetachSeHandle_, false, 0.5f);
    }

    float centerX = x_ + kWidth_ * 0.5f;
    float centerY = y_ + kHeight_ * 0.5f;

    if (hookTarget_ == 1) {
        float dx = static_cast<float>(anchorX_) - centerX;
        float dy = static_cast<float>(anchorY_) - centerY;
        float distance = sqrtf(dx * dx + dy * dy);

        isLeft_ = (dx < 0.0f);

        if (distance > 1.0f) {
            float dirX = dx / distance;
            float dirY = dy / distance;
            velocityX_ += dirX * hookAccel_;
            velocityY_ += dirY * hookAccel_;
        }

        velocityX_ *= 0.95f;
        velocityY_ *= 0.95f;

        if (velocityY_ < 0.0f) {
            isGrounded_ = false;
        }
    }
    else if (hookTarget_ == 2) {
        float wireStartX = 0.0f;
        float wireStartY = 0.0f;
        GetWireOrigin(wireStartX, wireStartY);

        float dx = wireStartX - orbX_;
        float dy = wireStartY - orbY_;
        float distance = sqrtf(dx * dx + dy * dy);

        if (distance <= 24.0f) {
            if (orbStock_ < kMaxOrbStock_) {
                orbStock_++;
            }
            hookTarget_ = 0;
            Novice::PlayAudio(wireDetachSeHandle_, false, 0.5f);
        }
        else {
            float dirX = dx / distance;
            float dirY = dy / distance;
            orbX_ += dirX * orbSpeed_;
            orbY_ += dirY * orbSpeed_;
        }

        velocityX_ *= 0.99f;
    }
    else {
        velocityX_ *= 0.99f;
    }

    // 重力と座標更新
    velocityY_ += kGravity_;
    x_ += velocityX_;
    y_ += velocityY_;

    // 画面外制限・床判定
    isGrounded_ = false;

    if (y_ >= kGroundY_) {
        y_ = kGroundY_;
        velocityY_ = 0.0f;
        isGrounded_ = true;
    }
    if (y_ < 0.0f) {
        y_ = 0.0f;
        if (velocityY_ < 0.0f) {
            velocityY_ = 0.0f;
        }
    }
    if (x_ < 0.0f) {
        x_ = 0.0f;
        if (velocityX_ < 0.0f) {
            velocityX_ = 0.0f;
        }
    }
    float maxRight = static_cast<float>(windowWidth - kWidth_ - kTileSize);
    if (x_ > maxRight) {
        x_ = maxRight;
        if (velocityX_ > 0.0f) {
            velocityX_ = 0.0f;
        }
    }

    // ボスとの接触ダメージ判定（貫通）
    if (invincibleTimer_ == 0 && hp_ > 0) {
        if (x_ + kWidth_ > bossLeft &&
            x_ < bossRight &&
            y_ + kHeight_ > bossTop &&
            y_ < bossBottom) {
            hp_--;
            invincibleTimer_ = kMaxInvincibleTime_;
        }
    }

    // アニメーション更新
    if (isMoving && isGrounded_) {
        animationTimer_++;
        if (animationTimer_ >= kAnimationSpeed_) {
            animationTimer_ = 0;
            animationFrame_++;
            if (animationFrame_ >= 4) {
                animationFrame_ = 0;
            }
        }
    }
    else {
        animationFrame_ = 0;
        animationTimer_ = 0;
    }
}

void Player::Draw() {
    Novice::ScreenPrintf(20, 20, "PLAYER HP: %d", hp_);

    // 鍾乳洞フックポイントの描画
    for (int i = 0; i < kNumHookPoints_; i++) {
        unsigned int hookColor = (hoveredHookIndex_ == i) ? 0xFFFF00FF : 0x00FFFFFF;
        int radius = (hoveredHookIndex_ == i) ? 6 : 4;
        Novice::DrawEllipse(
            static_cast<int>(hookPointX_[i]), static_cast<int>(hookPointY_[i]),
            radius, radius, 0.0f, hookColor, kFillModeSolid
        );
    }

    // ワイヤーの描画
    float wireStartX = 0.0f;
    float wireStartY = 0.0f;
    GetWireOrigin(wireStartX, wireStartY);

    if (hookTarget_ == 1) {
        Novice::DrawLine(static_cast<int>(wireStartX), static_cast<int>(wireStartY), anchorX_, anchorY_, 0xFFFFFFFF);
    }
    else if (hookTarget_ == 2) {
        Novice::DrawLine(static_cast<int>(wireStartX), static_cast<int>(wireStartY), static_cast<int>(orbX_), static_cast<int>(orbY_), 0xFFFFFFFF);
        Novice::DrawEllipse(static_cast<int>(orbX_), static_cast<int>(orbY_), static_cast<int>(orbRadius_), static_cast<int>(orbRadius_), 0.0f, 0xFF0000FF, kFillModeSolid);
    }

    // プレイヤー描画
    bool isVisible = (invincibleTimer_ == 0) || ((invincibleTimer_ / 4) % 2 == 0);
    if (isVisible) {
        if (hookTarget_ == 1) {
            if (isLeft_) {
                Novice::DrawQuad(
                    static_cast<int>(x_) + kWidth_, static_cast<int>(y_),
                    static_cast<int>(x_), static_cast<int>(y_),
                    static_cast<int>(x_) + kWidth_, static_cast<int>(y_) + kHeight_,
                    static_cast<int>(x_), static_cast<int>(y_) + kHeight_,
                    0, 0,
                    kWidth_, kHeight_,
                    catchTextureHandle_,
                    0xFFFFFFFF
                );
            }
            else {
                Novice::DrawQuad(
                    static_cast<int>(x_), static_cast<int>(y_),
                    static_cast<int>(x_) + kWidth_, static_cast<int>(y_),
                    static_cast<int>(x_), static_cast<int>(y_) + kHeight_,
                    static_cast<int>(x_) + kWidth_, static_cast<int>(y_) + kHeight_,
                    0, 0,
                    kWidth_, kHeight_,
                    catchTextureHandle_,
                    0xFFFFFFFF
                );
            }
        }
        else {
            if (isLeft_) {
                Novice::DrawQuad(
                    static_cast<int>(x_) + kWidth_, static_cast<int>(y_),
                    static_cast<int>(x_), static_cast<int>(y_),
                    static_cast<int>(x_) + kWidth_, static_cast<int>(y_) + kHeight_,
                    static_cast<int>(x_), static_cast<int>(y_) + kHeight_,
                    animationFrame_ * kWidth_, 0,
                    kWidth_, kHeight_,
                    textureHandle_,
                    0xFFFFFFFF
                );
            }
            else {
                Novice::DrawQuad(
                    static_cast<int>(x_), static_cast<int>(y_),
                    static_cast<int>(x_) + kWidth_, static_cast<int>(y_),
                    static_cast<int>(x_), static_cast<int>(y_) + kHeight_,
                    static_cast<int>(x_) + kWidth_, static_cast<int>(y_) + kHeight_,
                    animationFrame_ * kWidth_, 0,
                    kWidth_, kHeight_,
                    textureHandle_,
                    0xFFFFFFFF
                );
            }
        }
    }

    // 頭上のオーブストックを描画
    if (orbStock_ > 0) {
        float spacing = 22.0f;
        float startX = -((orbStock_ - 1) * spacing) / 2.0f;
        float stackY = y_ - 15.0f;
        float centerX = x_ + kWidth_ * 0.5f;

        for (int i = 0; i < orbStock_; i++) {
            float stackX = centerX + startX + i * spacing;
            Novice::DrawEllipse(static_cast<int>(stackX), static_cast<int>(stackY), static_cast<int>(orbRadius_), static_cast<int>(orbRadius_), 0.0f, 0xFF0000FF, kFillModeSolid);
            Novice::DrawEllipse(static_cast<int>(stackX), static_cast<int>(stackY), static_cast<int>(orbRadius_), static_cast<int>(orbRadius_), 0.0f, 0xFFFFFFFF, kFillModeWireFrame);
        }
    }

    // 発射された弾を描画
    for (int i = 0; i < kMaxBullets_; i++) {
        if (isBulletActive_[i]) {
            Novice::DrawEllipse(static_cast<int>(bulletX_[i]), static_cast<int>(bulletY_[i]), static_cast<int>(bulletRadius_), static_cast<int>(bulletRadius_), 0.0f, 0xFF0000FF, kFillModeSolid);
            Novice::DrawEllipse(static_cast<int>(bulletX_[i]), static_cast<int>(bulletY_[i]), static_cast<int>(bulletRadius_), static_cast<int>(bulletRadius_), 0.0f, 0xFFFFFFFF, kFillModeWireFrame);
        }
    }
}