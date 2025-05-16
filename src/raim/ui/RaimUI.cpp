#include "RaimUI.h"

#include <iostream>
#include <string>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <utils/TCPGecko.h>
#include <utils/TitleIdParser.h>
#include <utils/Config.h>
#include "../Raim.h"
#include "style/RaimUI_Theme.h"
#include "tab/RaimTabManager.h"
#include "NotificationManager.h"


RaimUI::RaimUI(Raim *appInstance)
    : mRaim(appInstance)
{
    mUITheme = new RaimUI_Theme(); // NOTE: 必ずRaimTabManagerよりも前に前に生成すること
    mTabManager = new RaimTabManager(this);

    // mTitleIdParser = new TitleIdParser("zaqro_u/Titles.xml");
    // mTitleIdParser->load();

    mNotificationManager = new NotificationManager();
}

RaimUI::~RaimUI()
{
    delete mTabManager;
    delete mUITheme;

    // delete mTitleIdParser;

    delete mNotificationManager;
}

void RaimUI::Init()
{
    std::string themeName = getConfig()->get("theme", std::string("dark"));
    const IRaimUITheme* theme = mUITheme->FromName(themeName);
    theme->Apply();
}

void RaimUI::Update()
{
    if (!mInitialized)
    {
        Init();
        mInitialized = true;
    }
    
    ImGuiViewport *viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    MainUI();
}

void RaimUI::MainUI()
{
    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("FullScreenWindow", nullptr, window_flags);
    ImGui::PopStyleVar(2);
    
    ImGui::SetWindowFontScale(getConfig()->get("font_scale", 1.0f));

    ImVec2 available = ImGui::GetContentRegionAvail();
    float buttonHeight = ImGui::GetFrameHeightWithSpacing() * 1.1f;
    float tabHeight = available.y - buttonHeight;

    ImGui::BeginChild("TabArea", ImVec2(0, tabHeight), false, ImGuiWindowFlags_None);
    try
    {
        mTabManager->Update();
    }
    catch (const std::exception &e)
    {
        mErrorMessage = e.what();
        mShowErrorPopup = true;
        ImGui::OpenPopup("Error");
    }
    ImGui::EndChild();

    try
    {
        static char ipBuffer[64] = "192.168.";

        std::shared_ptr<Config> config = getConfig();

        static bool entryBoxInitialized = false;
        if (!entryBoxInitialized)
        {
            if (config->contains("ip_address"))
            {
                std::string ipAddress = config->get("ip_address", std::string(ipBuffer));
                strncpy(ipBuffer, ipAddress.c_str(), sizeof(ipBuffer) - 1);
                ipBuffer[sizeof(ipBuffer) - 1] = '\0'; // 念のため null 終端を保証
            }
            entryBoxInitialized = true;
        }

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetFrameHeight() * 1.3f);

        float totalWidth = ImGui::GetContentRegionAvail().x;
        float buttonWidth = 100.0f;
        float inputWidth = totalWidth - buttonWidth - ImGui::GetStyle().ItemSpacing.x;

        std::shared_ptr<TCPGecko> tcp = getRaim()->getTCPGecko();
        bool isConnected = tcp->is_connected();

        auto connect = [&]()
        {
            tcp->connect(ipBuffer);
            config->set("ip_address", std::string(ipBuffer));
            config->save();
            GetNotificationManager()->AddNotification("TCPGecko", 
                std::format("Connected to: {}\nServer version: {}", std::string(ipBuffer), tcp->get_server_version()));
        };

        auto disconnect = [&]()
        {
            tcp->disconnect();
            GetNotificationManager()->AddNotification("TCPGecko", 
                std::format("Disconnected from: {}", std::string(ipBuffer)));
        };

        // InputText
        ImGui::PushItemWidth(inputWidth);
        if (ImGui::InputText("##IP Address", ipBuffer, IM_ARRAYSIZE(ipBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (!isConnected)
                connect();
        }
        ImGui::PopItemWidth();

        // 同じ行にボタン
        ImGui::SameLine();

        if (ImGui::Button(!isConnected ? "Connect" : "Disconnect", ImVec2(buttonWidth, 0)))
        {
            if (!isConnected)
                connect();
            else
                disconnect();
        }
    }
    catch (const std::exception &e)
    {
        mErrorMessage = e.what();
        mShowErrorPopup = true;
        ImGui::OpenPopup("Error");
    }

    if (mShowErrorPopup)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("An error occurred:");
            
            static char errorBuffer[2048];
            strncpy(errorBuffer, mErrorMessage.c_str(), sizeof(errorBuffer));
            errorBuffer[sizeof(errorBuffer) - 1] = '\0'; // null-terminate just in case

            ImGui::InputTextMultiline("##ErrorMessage", errorBuffer, sizeof(errorBuffer),
                                      ImVec2(400, 150), ImGuiInputTextFlags_ReadOnly);

            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 windowEnd = ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y);

            ImVec2 mousePos = ImGui::GetIO().MouseClickedPos[0];

            bool clickedOutside =
                (mousePos.x < windowPos.x || mousePos.x > windowEnd.x ||
                mousePos.y < windowPos.y || mousePos.y > windowEnd.y) &&
                ImGui::IsMouseClicked(0);

            if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvail().x, 0)) || clickedOutside)
            {
                ImGui::CloseCurrentPopup();
                mShowErrorPopup = false;
            }

            ImGui::EndPopup();
        }
    }

    mNotificationManager->Update();

    ImGui::End();
}

std::shared_ptr<Config> RaimUI::getConfig() { return mRaim->getConfig(); }
