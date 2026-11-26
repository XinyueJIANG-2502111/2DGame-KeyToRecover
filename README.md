# KeyToRecover

失われた操作キーを拾い、できることを増やしながらゴールを目指す Windows 向けの 2D アクションゲームです。C++17 と DxLib で実装しています。

## 遊び方

タイトル画面で Enter を押すとゲームが始まります。最初は右へ移動でき、ステージ上のキーに触れると左移動・ジャンプ・攻撃が使えるようになります。画面下のパネルで取得した操作を確認し、障害物を越えてゴールに到達してください。

| キー | 操作 |
| --- | --- |
| D / → | 右へ移動 |
| A / ← | 左へ移動（対応するキーを取得後） |
| Space | ジャンプ（対応するキーを取得後） |
| L | 攻撃（対応するキーを取得後） |
| Shift | 押している間、移動速度を上げる |
| Tab | ポーズメニューを開閉 |
| ↑ / ↓、Enter | メニュー項目を選択・決定 |
| Backspace | プレイ中にタイトルへ戻る |
| Esc | ゲームを終了 |

## ビルドと実行

Windows、Visual Studio 2022 の「C++ によるデスクトップ開発」、MSVC v143 ツールセットが必要です。以下は x64 Release の手順です。

### Visual Studio 2022

1. `KeyToRecover.sln` を開き、構成を `Release`、プラットフォームを `x64` に設定してビルドします。ソリューションは同梱の `deps/include_dxlib`（DxLib 3.24f）を使用します。
2. プロジェクトのルートから `./x64/Release/KeyToRecover.exe` を実行します。

### CMake

Visual Studio 2022 ジェネレーターには CMake 3.21 以降が必要です。既定の DxLib ディレクトリは `C:/DxLib_VC/プロジェクトに追加すべきファイル_VC用` です。この場所の DxLib 3.24b でビルドを確認しています。

プロジェクトのルートで、PowerShell から以下を実行します。

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -T host=x64
cmake --build build --config Release
Push-Location ./build/Release
./KeyToRecover.exe
Pop-Location
```

別の場所に DxLib がある場合は、構成時に `DXLIB_DIR` を指定します。以下は同梱の DxLib を使う例です。

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -T host=x64 "-DDXLIB_DIR=$PWD/deps/include_dxlib"
```

CMake はビルド後に `Resource/` を実行ファイルの隣へコピーします。Visual Studio のビルドから試遊用フォルダーを作る場合も、`KeyToRecover.exe` と `Resource/` を同じフォルダーに入れ、そのフォルダーから実行してください。

## ソース構成

| パス | 主な内容 |
| --- | --- |
| `src/app/` | プログラムの入口、ゲームループ |
| `src/engine/` | シーン管理、オブジェクト、コンポーネント、衝突処理、描画順 |
| `src/game/` | ステージ、プレイヤー、操作の解放、HUD、演出 |
| `src/platform/` | 入力マッピング、DxLib を使う音声・描画・リソース処理 |
| `src/third_party/` | サードパーティーのヘッダー |
| `Resource/` | 画像、音声、CSV のステージデータ |

## 実装上の工夫

- **操作の解放** — 取得済みのキーとプレイヤーの状態を `PlayerProgress` に集約しています。HUD はそこから表示内容を決め、リザルト画面はクリア時点の状態を受け取ります。
- **ゲームループとシーン遷移** — ゲームロジックは 60 Hz の固定時間刻みで更新します。プレイ中のリスタート、ゲームオーバー、クリアの判定はステージ側に集約しています。
- **シーン内の処理** — オブジェクトの寿命管理、衝突解決、描画順の決定を分けています。衝突解決は 1 更新につき 4 回反復し、接触の通知は同じ組に対して 1 回にまとめています。

## 現在の状態

- 収録ステージは `Stage1` の 1 つです。
- ゲームオーバー画面の BGM は、同梱ファイルと参照名が一致していないため再生されません。
- ステージを繰り返しリスタートすると、一部のエフェクト画像のハンドルが残ります。

## サードパーティー

- DxLib。配布物に含まれる説明は `deps/include_dxlib/DxLib.txt` を参照してください。
- 同梱ヘッダーの stb_image_write。ライセンス文は `src/third_party/stb_image_write.h` に含まれます。