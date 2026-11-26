#include "Stage1.hpp"

#include "platform/dxlib/AudioManager.hpp"
#include "engine/components/BackgroundComponent.hpp"
#include "game/effects/BSoDFaceComponent.hpp"
#include "game/ui/ErrorWindow.hpp"
#include "engine/Global.hpp"
#include "game/objects/Goal.hpp"
#include "game/objects/Icicle.hpp"
#include "game/ui/InputDisplayPanel.hpp"
#include "platform/input/InputMapping.hpp"
#include "Map.hpp"
#include "game/objects/MovingPlatform.hpp"
#include "game/actors/Player.hpp"
#include "game/effects/ScatteredTextBackground.hpp"
#include "engine/scene/SceneManager.hpp"
#include "game/objects/TestItem.hpp"
#include "game/objects/Tile.hpp"
#include "game/objects/UnlockItem.hpp"

void Stage1::Initialize() {

    m_Progress = PlayerProgress{};

    // create background
    int bgHandle = ResourceManager::GetInstance().LoadGraphic("Resource/images/GameBackScreen.PNG");
    int bgX, bgY;
    GraphicsManager::GetInstance().GetGraphSize(bgHandle, &bgX, &bgY);
    auto bgObject = std::make_unique<GameObject>();
    bgObject->SetZOrder(-1.f);
    bgObject->AddComponent<BackgroundComponent>(bgHandle, 1.f);
    AddObject(std::move(bgObject));
    SetWorldBounds(Vector2f(0.f, 0.f), Vector2f(bgX, bgY));

    // create camera
    m_Camera = std::make_unique<Camera>(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
    m_Camera->SetPosition(m_Camera->WorldToScreen(Vector2f(0.f, 0.f)));
    m_Camera->SetBounds(bgX, bgY);
    m_Camera->SetDeadzone(600, 400);

    // create player
    auto player = std::make_unique<Player>(200.f, 700.f, m_Progress);
    mp_Player = player.get();

    auto effectObject = std::make_unique<GameObject>();
    effectObject->SetZOrder(-0.8f); // 背景(-1.0)より手前、プレイヤー(0.0)より奥
    effectObject->AddComponent<BSoDFaceComponent>(effectObject->GetTransform(), mp_Player);
    AddObject(std::move(effectObject));

    // 描画する直前に、システム全体の標準フォントサイズを32に変更
    SetFontSize(32);

    auto textBg = std::make_unique<ScatteredTextBackground>(mp_Player, 0.1f);

    // ★好きな座標に文言を散りばめる
    textBg->AddText("A problem has been detected...", 100.f, 200.f);
    textBg->AddText("PAGE_FAULT_IN_NONPAGED_AREA", 600.f, 150.f);
    textBg->AddText("If this is the first time you've seen", 300.f, 500.f);
    textBg->AddText("this stop error screen,", 350.f, 530.f);
    textBg->AddText("restart your computer.", 400.f, 560.f);
    textBg->AddText("Technical information:", 800.f, 800.f);
    textBg->AddText("*** STOP: 0x00000050", 1000.f, 850.f);
    textBg->AddText("FATAL_EXCEPTION: UNKNOWN_STATE", 60.f, 2000.f);
    textBg->AddText("memory corruption detected — it’s already here", 850.f, 500.f);
    textBg->AddText("authentication failed: this user no longer exists", 2000.f, 10.f);
    textBg->AddText("[WARNING] memory region is being rewritten", 1000.f, 250.f);
    textBg->AddText("unable to read: /user/profile/identity.dat", 1040.f, 400.f);
    textBg->AddText("the observed has become the observer", 1500.f, 800.f);
    textBg->AddText("process is responding autonomously (control lost)", 1600.f, 600.f);
    textBg->AddText("d4t4 str34m d3synchroniz3d>> i / p buffer : ████▒▒▒▒▒▒░░░", 1200.f, 100.f);
    textBg->AddText(" l00p d3t3ct3d: ∞∞∞∞∞∞∞∞∞∞", 1400.f, 560.f);
    textBg->AddText("[W4RNING] d1spl4y c0rrupt3d>> t3xt r3nd3ring f41lure // gl1tch m0d3: ON", 1500.f, 480.f);
    textBg->AddText("/user/████/████.dat n0t f0und >> fil3 3x1sts but c4nn0t b3 4cc3ss3d", 1600.f, 380.f);
    textBg->AddText(" m3m0ry s3ct0r 0x0000FFFF h4s b33n 0v3rwr1tt3n", 1700.f, 1000.f);
    textBg->AddText("█▒█▒▓▒░▒█▓▒░▓▒█▒░▒▓▒█", 1919.f, 1300.f);
    textBg->AddText("[CR1T1C4L] 0bs3rv3r_l00p d3t3ct3d>> th3 syst3m 1s w4tch1ng 1ts3lf thr0ugh y0u", 1550.f, 520.f);
    textBg->AddText("init_sequence_failure::core_dump_incomplete", 2000.f, 860.f);
    textBg->AddText("curr3nt us4g3: 999999MB", 2100.f, 450.f);
    textBg->AddText("t1m3st4mp 1nc0ns1st3ncy", 600.f, 30.f);
    textBg->AddText(" curr3nt d4t3: 2026/05/14", 45.f, 500.f);
    textBg->AddText(":: SYST3M M3SS4G3 ::th3 scr33n 1s n0t th3 b0und4ry", 200.f, 600.f);
    textBg->AddText("█▓▒░ ERROR_̴̛̈́̽̈́̓͌̎͛̐̒̐̈́͝͝SYS ░▒▓█  ", 300.f, 400.f);
    textBg->AddText("Ⱥ̷C̸C̷E̸S̴S̴_̵D̸E̷N̵I̶E̸D̷", 950.f, 1000.f);
    textBg->AddText("$@#%&*!?~$@#%&*!?~", 800.f, 540.f);
    textBg->AddText("@@@ MEMORY FAILURE @@@", 970.f, 300.f);
    textBg->AddText("%%%%% NO RESPONSE %%%%%", 780.f, 640.f);
    textBg->AddText("syst3mmm_rrRRRR////_bR0k3n??!!??", 2100.f, 430.f);
    textBg->AddText("wh4t_1s_h4pp3n1nnggggggggggg", 2310.f, 640.f);
    textBg->AddText("n0_s1gn4l___n0_s1gn4l___n̷̺̎̓̾̔͌̕͝o̴̢̬͂̈́̈́̅", 2700.f, 620.f);
    textBg->AddText("d4t4_d4t4_d4t4_d4t4_d4———[LOST]", 2800.f, 850.f);
    textBg->AddText("%%%%% NO RESPONSE %%%%%", 2400.f, 240.f);
    textBg->AddText("gl1tch_gl1tch_gl1tch___ERR0RrrrrrRRR???", 2900.f, 730.f);
    textBg->AddText("syst3m_malfunct10n___pl34s3_w41t___n0_d0nt", 2480.f, 530.f);
    textBg->AddText("whyyyyyy_1s_th3_scr33n_br34th1nggggg", 2540.f, 900.f);
    textBg->AddText("d4t4_d4t4_d4t4_d4t4_d4t4_d4t4_d4———", 2900.f, 100.f);
    textBg->AddText("kkkkkkkkkkkkkkzzzzzzzzzzzzzzttttttttt", 2800.f, 500.f);
    textBg->AddText("0v3rwr1t3_0v3rwr1t3_0v3rwr1t3_0v3rwr1t3", 2700.f, 1000.f);
    textBg->AddText("r34l1ty_sync_f41l3d___r34l1ty_sync_f41l3d", 3000.f, 1200.f);
    textBg->AddText("st4ck_st4ck_st4ck_st4ck_st4ck_0v3rfl0w", 3100.f, 200.f);
    textBg->AddText("th3_v01c3_1s_c0m1ng_fr0m_1ns1d3_th3_pc", 3510.f, 300.f);
    textBg->AddText("scr33n_buff3r_d3sync_____%#@!?!?!?!?!", 3200.f, 530.f);
    textBg->AddText("d0nt_l00k_b3h1nd_y0u_d0nt_l00k_b3h1nd_y0u", 3220.f, 130.f);
    textBg->AddText("fr4gm3nt_0001___fr4gm3nt_0002___fr4gm3nt_6666", 3670.f, 970.f);
    textBg->AddText("th1s_t3xt_sh0uld_n0t_b3_h3r3", 3860.f, 760.f);
    textBg->AddText("pl34s3_st4ndby___pl34s3_st4ndby___f0r_wh4t", 3500.f, 500.f);
    textBg->AddText("unkn0wn_us3r_h4s_j01n3d_th3_s3ss10n", 3400.f, 600.f);
    textBg->AddText("f41l3d_t0_3sc4p3_l00p_l00p_l00p_l00p", 3600.f, 640.f);
    textBg->AddText("45072??gea25268913genajiejagueiw", 3300.f, 1000.f);
    textBg->AddText("g4g4g4g4g4g4g4g4g4g4g4g4g4g4g4g43", 3680.f, 580.f);
    textBg->AddText("n0_s1gn4lllllll___n0_s1gn4lllllll", 4680.f, 550.f);
    textBg->AddText("st4ckkkkkkkkkkkkkkkkkkkkkkkkkkkkk", 4500.f, 860.f);
    textBg->AddText("y0u_sh0uld_h4v3_cl0s3d_th3_scr33n", 4600.f, 900.f);
    textBg->AddText("t1m3l1n3_m1sm4tch___y3st3rd4y=tom0rr0w", 4700.f, 140.f);
    textBg->AddText("c4nn0t_t3rm1n4t3_pr0c3ss::::::::::", 4310.f, 300.f);
    textBg->AddText("bR0k3n_bR0k3n_bR0k3n_bR0k3n_bR0k3n", 4900.f, 170.f);
    textBg->AddText("c4n_y0u_h34r_th3_f4nnnnnnnnnnnnn", 4140.f, 780.f);
    textBg->AddText("th3_k3yb04rd_1s_typ1ng_4l0n3", 4800.f, 740.f);
    textBg->AddText("unkn0wn_pr0c3ss_h4s_g41n3d_c0ntr0l", 4300.f, 1000.f);
    textBg->AddText("s1gn4l_f0und_s1gn4l_f0und_s1gn4l_f0und", 4450.f, 100.f);
    textBg->AddText("wh0_4r3_y0u_wh0_4r3_y0u_wh0_4r3_y0u", 4750.f, 640.f);
    textBg->AddText("n0_0utput_but_st1ll_runn1ngggggggg", 4545.f, 072.f);
    textBg->AddText("4545072Hero->ootaseizi691919", 5500.f, 1200.f);
    textBg->AddText("murata♡13gaylove", 5500.f, 1300.f);

    // 描画する直前に、システム全体の標準フォントサイズを32に変更
    SetFontSize(16);

    // Stage1のオブジェクトリストに登録
    AddObject(std::move(textBg));


    int frameHandle = ResourceManager::GetInstance().LoadGraphic("Resource/images/WindowFrame.png");
    // 座標(500, 400)に、距離300で反応するウィンドウを設置
    auto errorWnd1 = std::make_unique<ErrorWindow>(Vector2f(1500.f, 400.f), 700.f, "WARNING: \n PC些が髯危険にさらｺ縺ｫています。", frameHandle, mp_Player);
    AddObject(std::move(errorWnd1));
    auto errorWnd2 = std::make_unique<ErrorWindow>(Vector2f(3000.f, 400.f), 500.f, "WARNING: \n コンベ些髯ｺアに乗り縺ｫ荵励なさい", frameHandle, mp_Player);
    AddObject(std::move(errorWnd2));
    auto errorWnd3 = std::make_unique<ErrorWindow>(Vector2f(3500.f, 200.f), 500.f, "WARNING: \n あ繧ｸ繝なた縺溘の住所を\n特定しました:(", frameHandle, mp_Player);
    AddObject(std::move(errorWnd3));
    auto errorWnd4 = std::make_unique<ErrorWindow>(Vector2f(6000.f, 400.f), 700.f, "WARNING: \n あなたｧ關ここで落ち％縺ｧください", frameHandle, mp_Player);
    AddObject(std::move(errorWnd4));
    auto errorWnd5 = std::make_unique<ErrorWindow>(Vector2f(6200.f, 600.f), 300.f, "WARNING: \n 落ちろ", frameHandle, mp_Player);
    AddObject(std::move(errorWnd5));
    auto errorWnd6 = std::make_unique<ErrorWindow>(Vector2f(6400.f, 300.f), 400.f, "WARNING: \n 落ちろ", frameHandle, mp_Player);
    AddObject(std::move(errorWnd6));

    auto errorWnd7 = std::make_unique<ErrorWindow>(Vector2f(6600.f, 100.f), 500.f, "WARNING: \n 落ちろ", frameHandle, mp_Player);
    AddObject(std::move(errorWnd7));

    auto errorWnd8 = std::make_unique<ErrorWindow>(Vector2f(6800.f, 600.f), 400.f, "WARNING: \n 落ちろ", frameHandle, mp_Player);
    AddObject(std::move(errorWnd8));

    auto errorWnd9 = std::make_unique<ErrorWindow>(Vector2f(200.f, 300.f), 700.f, "WARNING: \n ウイルスに感染している\n恐れがあります。\nキーを回収し\nゴールに到達してください。", frameHandle, mp_Player);
    errorWnd9->SetStringColor(255, 0, 0);
    AddObject(std::move(errorWnd9));

    auto errorWnd10 = std::make_unique<ErrorWindow>(Vector2f(2400.f, 300.f), 400.f, "WARNING: \n ウイルスバスターがジャック\nされました。急いでください。", frameHandle, mp_Player);
    AddObject(std::move(errorWnd10));
    auto errorWnd11 = std::make_unique<ErrorWindow>(Vector2f(5000.f, 300.f), 600.f, "     警告: \n 大切なあなたのデータを\n 消去しています...\n 完了まであと50秒", frameHandle, mp_Player);
    errorWnd11->SetStringColor(255, 0, 0);
    AddObject(std::move(errorWnd11));

    AddObject(std::move(player));

    // Init時に直接アドレス（参照）を渡して中身を書き換えてもらう
    MapData mapData{};
    if (!LoadMapFromCSV("Resource/maps/Stage1.csv", mapData)) {
        // エラー処理（ファイルの読み込み失敗など）
        DrawString(20.f, 20.f, "Stage1.csvの読み込みに失敗しました", GetColor(0, 0, 0));
    }

    // create test items
    for (int i = 0; i < 5; ++i) {
        //auto box = std::make_unique<TestItem>(200.f + i * 50.f, 1800.f);
        auto box = std::make_unique<TestItem>(400, 1600.f + i * 50.f);
        AddObject(std::move(box));
    }

    auto inputPanel = std::make_unique<InputDisplayPanel>(Config::SCREEN_WIDTH - 320.f, Config::SCREEN_HEIGHT - 120.f, "Resource/images/InputDisplay.png", m_Progress);
    AddObject(std::move(inputPanel));

    //AudioManager::GetInstance().

    BuildStage(mapData);

    AudioManager::GetInstance()->StopMusic();
    AudioManager::GetInstance()->PlayMusic("Resource/sounds/BGM/GameScene.mp3", true);
    // TODO : create other objects ( enemies, ... )
}

void Stage1::Update(double deltaTime) {
    m_Menu.Update(deltaTime);
    if (const auto option = m_Menu.GetConfirmedOption()) {
        const auto nextScene = *option == Menu::Option::Restart
            ? SceneType::SCENETYPE_GAME_STAGE1
            : SceneType::SCENETYPE_TITLE;
        SceneManager::GetInstance().ChangeScene(nextScene);
        return;
    }
    if (m_Menu.IsActive()) return;

    if (mp_Player) {
        m_Camera->Update(mp_Player->GetPosition(), 0.2f);
    }
    Scene::Update(deltaTime);

    auto& scenes = SceneManager::GetInstance();
    if (InputMapping::GetInstance().IsJustPressed(InputAction::BACK)) {
        scenes.ChangeScene(SceneType::SCENETYPE_TITLE);
        return;
    }

    if (mp_Player && mp_Player->GetPosition().y > 1000.f) {
        m_Progress.currentHp -= 10;
        mp_Player->SetPosition(Vector2f(200.f, 400.f));
        AudioManager::GetInstance()->PlaySound("Resource/sounds/SE/被ダメ.mp3");
    }

    if (m_Progress.currentHp <= 0) {
        scenes.ChangeScene(SceneType::SCENETYPE_GAMEOVER);
        return;
    }

    for (auto* object : FindObjectsWithTag("Goal")) {
        if (static_cast<Goal*>(object)->IsReached()) {
            AudioManager::GetInstance()->PlaySound("Resource/sounds/SE/ClearSound.m4a");
            scenes.ChangeScene(SceneType::SCENETYPE_RESULT);
            return;
        }
    }
}

void Stage1::Draw() {
    Scene::Draw();
    m_Menu.Draw(*m_Camera);
}

void Stage1::BuildStage(const MapData& mapData) {
    const float TILE_SIZE = 60.f;

    for (size_t i = 0; i < mapData.tiles.size(); ++i) {
        int tileType = mapData.tiles[i];

        // 空白（例えば0を何もない空間とする場合）はスキップ
        if (tileType == 0) continue;

        // 1次元のインデックスから、2次元の列(col)と行(row)を算出
        int col = i % mapData.width;
        int row = i / mapData.width;

        // 座標の計算
        float posX = col * TILE_SIZE;
        float posY = row * TILE_SIZE;

        switch (tileType) {
        case 1: { // 床タイルの場合
            std::string filename = "Resource/images/Tile.png";
            auto tile = std::make_unique<Tile>(posX, posY, filename);
            AddObject(std::move(tile));
            break;
        }


        case 2: { // Keyタイルの場合
            std::string filename = "Resource/images/Folder.png";
            auto tile = std::make_unique<Tile>(posX, posY, filename);
            AddObject(std::move(tile));
            break;
        }

        case 3: { // ゴールの場合
            auto tile = std::make_unique<Goal>(posX, posY, "Resource/images/GoalFlag.png");
            AddObject(std::move(tile));
            break;
        }

        case 4: { // エレベーターの場合
            auto tile = std::make_unique<MovingPlatform>(posX, posY,
                std::vector<Vector2f>{Vector2f(-200.0f, 0.0f), Vector2f(-200.0f, 350.0f), Vector2f(200.0f, 350.0f), Vector2f(200.0f, 0.0f)},
                "Resource/images/Elevator.png");
            AddObject(std::move(tile));
            break;
        }

        case 5: { // つららの場合
            auto tile = std::make_unique<Icicle>(posX, posY, "Resource/images/lcicle.PNG", mp_Player);
            AddObject(std::move(tile));
            break;
        }

        case 6: { // エレベーターの場合
            auto tile = std::make_unique<MovingPlatform>(posX + 15, posY,
                std::vector<Vector2f>{Vector2f(0.0f, 0.0f), Vector2f(0.0f, 330.0f), Vector2f(330.0f, 330.0f), Vector2f(330.0f, 0.0f)},
                "Resource/images/Elevator.png");
            AddObject(std::move(tile));
            break;
        }

        case 7: { // エレベーターの場合
            auto tile = std::make_unique<MovingPlatform>(posX + 15, posY,
                std::vector<Vector2f>{Vector2f(0.0f, 0.0f), Vector2f(0.0f, 330.0f), Vector2f(330.0f, 330.0f), Vector2f(330.0f, 0.0f),
                Vector2f(660.f, 0.0f), Vector2f(660.f, 330.0f), Vector2f(990.f, 330.0f), Vector2f(990.f, 0.0f)},
                "Resource/images/Elevator.png");
            AddObject(std::move(tile));

            break;
        }

        case 8: { // エレベーターの場合
            auto tile = std::make_unique<MovingPlatform>(posX + 15, posY,
                std::vector<Vector2f>{Vector2f(0.0f, 150.0f), Vector2f(0.0f, 330.0f), Vector2f(0.0f, -150.0)},
                "Resource/images/Elevator.png");
            AddObject(std::move(tile));

            break;
        }

        case 9: { // エレベーターの場合
            auto tile = std::make_unique<MovingPlatform>(posX + 15, posY,
                std::vector<Vector2f>{Vector2f(0.0f, 330.0f), Vector2f(0.0f, -330.0f), Vector2f(0.0f, 0.0f)},
                "Resource/images/Elevator.png");
            AddObject(std::move(tile));

            break;
        }

        case 10: { // 左キーの場合
            auto tile = std::make_unique<UnlockItem>(posX, posY, InputAction::MOVE_LEFT, "Resource/images/プレイヤー「A」.png");
            AddObject(std::move(tile));
            break;
        }

        case 11: { // ジャンプの場合6
            auto tile = std::make_unique<UnlockItem>(posX, posY, InputAction::JUMP, "Resource/images/Space.png");
            AddObject(std::move(tile));
            break;
        }

        case 12: { // 攻撃の場合
            auto tile = std::make_unique<UnlockItem>(posX, posY, InputAction::ATTACK, "Resource/images/プレイヤー「L」.png");
            AddObject(std::move(tile));
            break;
        }

        case 13: { // 投げの場合
            auto tile = std::make_unique<UnlockItem>(posX, posY, InputAction::PICKUP, "Resource/images/プレイヤー「J」.png");
            AddObject(std::move(tile));
            break;
        }

        }
    }
}
