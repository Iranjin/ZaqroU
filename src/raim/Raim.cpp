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
    mConfig->load(mConfigPath);
    LoadFonts();
    mRaimUI = new RaimUI(this);
    mRaimUI->Init();
}

Raim::~Raim()
{
    delete mRaimUI;
}

void Raim::LoadFonts()
{
    auto download_font = [](const std::string &path) {
        std::string font_path = "zaqro_u/" + path;
        std::string font_url = "https://github.com/Iranjin/ZaqroU/raw/refs/heads/main/" + font_path;

        if (std::filesystem::exists(font_path))
            return;
        
        std::string folder_parent_path = std::filesystem::path(font_path).parent_path();
        
        std::vector<char> data;
        download_file(font_url, data);

        std::filesystem::create_directories(folder_parent_path);

        save_to_file(font_path, data);
    };
    
    ImGuiIO &io = ImGui::GetIO();

    io.IniFilename = nullptr;

    ImFontConfig font_config;

    { // English font
        std::string font_path = "res/fonts/HackNerdFont-Regular.ttf";
        std::string res_font_path = "zaqro_u/" + font_path;
        
        download_font(font_path);

        static ImWchar const glyph_ranges[] = {
            0x0020, 0xfffd,
            0,
        };

        io.Fonts->AddFontFromFileTTF(res_font_path.c_str(), 64.0f, &font_config, glyph_ranges);
    }

    { // Japanese font
        std::string font_path = "res/fonts/NotoSansJP-Regular.ttf";
        std::string res_font_path = "zaqro_u/" + font_path;
        
        download_font(font_path);

        font_config.MergeMode = true;
        io.Fonts->AddFontFromFileTTF(font_path.c_str(), 88.0f, &font_config, io.Fonts->GetGlyphRangesJapanese());
    }

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
