#include <Novice.h>
#include "Map.h"
#include "Player.h"
#include "Boss.h"

const char kWindowTitle[] = "TD1_teamProject_02 with Novice 20250623";
const int kWindowWidth = 1920;
const int kWindowHeight = 1080;

enum Scene {
    TITLE,
    TUTORIAL,
    BATTLE
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    // テクスチャ読み込み
    int titleHandle = Novice::LoadTexture("./Resources/images/gameTitle.png");
    int bgHandle = Novice::LoadTexture("./Resources/images/bg.png");
    int bossStageHandle = Novice::LoadTexture("./Resources/images/BossStage.png");
    int enterHandle = Novice::LoadTexture("./Resources/images/Enter.png");

    // 各クラスの生成・初期化
    Map map;
    map.Initialize();

    Player player;
    player.Initialize();

    Boss boss;
    boss.Initialize(player.GetY(), player.GetHeight());

    Scene scene = TITLE;

    // ウィンドウの×ボタンが押されるまでループ
    while (Novice::ProcessMessage() == 0) {
        // フレームの開始
        Novice::BeginFrame();

        // キー入力を受け取る
        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

        ///
        /// ↓更新処理ここから
        ///
        switch (scene) {
        case TITLE:
            // スペースキーまたは左クリックでチュートリアルへ（エンターでは反応しない）
            if ((keys[DIK_SPACE] && !preKeys[DIK_SPACE]) || Novice::IsTriggerMouse(0)) {
                player.Initialize();
                scene = TUTORIAL;
            }
            break;

        case TUTORIAL:
            player.UpdateTutorial(keys, preKeys, kWindowWidth);

            // チュートリアルからボス戦に移るときだけEnterキー
            if (keys[DIK_RETURN] && !preKeys[DIK_RETURN]) {
                player.Initialize();
                boss.Initialize(player.GetY(), player.GetHeight());
                scene = BATTLE;
            }
            break;

        case BATTLE:
            player.Update(keys, preKeys, boss, kWindowWidth);
            boss.Update(player.GetX(), player.GetWidth());

            // プレイヤーHPが0になったらRキーでタイトルへ戻る（エンターでは反応しない）
            if (player.GetHp() <= 0) {
                if (keys[DIK_R] && !preKeys[DIK_R]) {
                    scene = TITLE;
                }
            }
            break;
        }
        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///
        switch (scene) {
        case TITLE:
            Novice::DrawSprite(0, 0, titleHandle, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
            break;

        case TUTORIAL:
            Novice::DrawSprite(0, 0, bgHandle, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
            map.Draw();
            player.Draw();

            // 画面右側に「Boss stage」と「Enter」を表示
            Novice::DrawSprite(1550, 620, bossStageHandle, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
            Novice::DrawSprite(1550, 820, enterHandle, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);

            Novice::ScreenPrintf(20, 20, "PLAYER HP: %d", player.GetHp());
            break;

        case BATTLE:
            Novice::DrawSprite(0, 0, bgHandle, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
            map.Draw();
            boss.Draw();
            player.Draw();

            Novice::ScreenPrintf(20, 20, "PLAYER HP: %d", player.GetHp());
            break;
        }
        ///
        /// ↑描画処理ここまで
        ///

        // フレームの終了
        Novice::EndFrame();

        // ESCキーが押されたらループを抜ける
        if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
            break;
        }
    }

    // ライブラリの終了
    Novice::Finalize();
    return 0;
}