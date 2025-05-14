# Zaqro U

Zaqro U は、Wii U 向けのチートツールです。C++20 を使用しており、GUI や描画には GLFW、ImGui、OpenGL などのライブラリを利用しています。  
内部開発コードネームとして「Raim」が使われていますが、利用やビルドに影響はありません。  
「Zaqro」は、私の名前「イラン人」にちなんで、**イランの代表的な果物「ザクロ」**を日本語風にアレンジしたものです。
「U」は、対象プラットフォームである Wii U や、同系統のツール「JGecko U」へのオマージュです。

## 必要環境

- CMake ≥ 3.15  
- C++20 対応のコンパイラ  
- [vcpkg](https://github.com/microsoft/vcpkg) による依存関係の管理  
- （オプション）対応 OS：Windows（他 OS は未検証）

## 使用ライブラリ（vcpkg 経由）

本プロジェクトでは以下のライブラリを使用しています。すべて [vcpkg](https://github.com/microsoft/vcpkg) を用いて導入可能です。

- [GLFW](https://www.glfw.org/) — ウィンドウ作成および入力処理  
- [GLAD](https://github.com/Dav1dde/glad) — OpenGL ローダー（関数読み込み）  
- [OpenGL](https://www.opengl.org/) — グラフィックス API  
- [ImGui](https://github.com/ocornut/imgui) — GUI ライブラリ  
- [cURL](https://curl.se/libcurl/) — HTTP 通信ライブラリ  
- [LZ4](https://github.com/lz4/lz4) — 高速圧縮ライブラリ  
- [nlohmann/json](https://github.com/nlohmann/json) — JSON パーサ／シリアライザ  
- [tinyxml2](https://github.com/leethomason/tinyxml2) — 軽量 XML パーサ  
- [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/) — クロスプラットフォームなファイルダイアログ

## ビルド方法

```bash
git clone https://github.com/Iranjin/ZaqroU.git
cd ZaqroU
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
