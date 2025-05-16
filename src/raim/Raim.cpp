#include "Raim.h"

#include <imgui.h>

#include <utils/TCPGecko.h>
#include <utils/Config.h>
#include <utils/downloader.h>
#include "ui/RaimUI.h"
#include "ui/style/RaimUI_Theme.h"


Raim::Raim(GLFWwindow *window)
    : mWindow(window),
      mConfig(std::make_shared<Config>()),
      mTCPGecko(std::make_shared<TCPGecko>()),
      mLastSaveTime(std::chrono::steady_clock::now()),
      mConfigPath("zaqro_u/config.json")
{
    mRaimUI = new RaimUI(this);

    Init();
}

Raim::~Raim()
{
    delete mRaimUI;
}

void Raim::Init()
{
    mConfig->load(mConfigPath);
    LoadFonts();
    mRaimUI->Init();
}

void Raim::LoadFonts()
{
    constexpr size_t font_size = 2;
    const char *fonts[font_size] = {
        "/res/fonts/HackNerdFont-Regular.ttf",
        "/res/fonts/NotoSansJP-Regular.ttf"
    };

    for (size_t i = 0; i < font_size; i++)
    {
        std::string font_path = std::string("zaqro_u") + fonts[i];
        
        if (std::filesystem::exists(font_path))
            continue;
        
        std::string folder_path = std::filesystem::path(font_path).parent_path();
        std::string url = std::string("https://github.com/Iranjin/ZaqroU/raw/refs/heads/main") + fonts[i];
        
        std::vector<char> data;
        download_file(url, data);

        std::filesystem::create_directories(folder_path);

        save_to_file(font_path, data);
    }

    ImGuiIO &io = ImGui::GetIO();

    io.IniFilename = nullptr;

    static ImWchar const glyph_ranges[] = {
        0x0020, 0xfffd,
        0,
    };

    io.Fonts->AddFontFromFileTTF("zaqro_u/res/fonts/HackNerdFont-Regular.ttf", 64.0f, NULL, glyph_ranges);

    ImFontConfig font_config;
    font_config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF("zaqro_u/res/fonts/NotoSansJP-Regular.ttf", 88.0f, &font_config, io.Fonts->GetGlyphRangesJapanese());

    io.FontGlobalScale = 0.25f;
}

void Raim::Update()
{
    mRaimUI->Update();

    // auto now = std::chrono::steady_clock::now();
    // if (std::chrono::duration_cast<std::chrono::seconds>(now - mLastSaveTime).count() >= 5 && !mConfig->empty())
    // {
    //     if (!mSaveFuture.valid() || mSaveFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    //     {
    //         mSaveFuture = std::async(std::launch::async, [config = mConfig, configPath = mConfigPath]() {
    //             config->save(configPath);
    //         });
    //         mLastSaveTime = now;
    //     }
    // }
}
