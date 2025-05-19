#include "RaimUI.h"

#include <iostream>
#include <string>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <utils/tcp_gecko/TCPGecko.h>
#include <utils/TitleIdParser.h>
#include <utils/StrUtils.h>
#include <utils/Config.h>
#include "../Raim.h"
#include "style/RaimUI_Theme.h"
#include "tab/RaimTabManager.h"
#include "NotificationManager.h"


RaimUI::RaimUI(Raim *app_instance)
    : m_raim(app_instance)
{
    m_ui_theme = new RaimUI_Theme();
    m_tab_manager = new RaimTabManager(this);
    m_notif_manager = new NotificationManager();
}

RaimUI::~RaimUI()
{
    delete m_tab_manager;
    delete m_ui_theme;
    delete m_notif_manager;
}

void RaimUI::Init()
{
    std::string themeName = get_config()->get_nested("appearance.theme", std::string("dark"));
    const IRaimUITheme *theme = m_ui_theme->from_name(themeName);
    theme->apply();
}

void RaimUI::Update()
{
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
    
    ImGui::SetWindowFontScale(get_config()->get_nested("appearance.font_scale", 1.0f));

    ImVec2 available = ImGui::GetContentRegionAvail();
    float button_height = ImGui::GetFrameHeightWithSpacing() * 1.1f;
    float tab_height = available.y - button_height;

    ImGui::BeginChild("TabArea", ImVec2(0, tab_height), false, ImGuiWindowFlags_None);
    try
    {
        m_tab_manager->Update();
    }
    catch (const std::exception &e)
    {
        m_error_message = e.what();
        m_show_error_popup = true;
        ImGui::OpenPopup("Error");
    }
    ImGui::EndChild();

    std::shared_ptr<TCPGecko> tcp = get_raim()->get_tcp_gecko();
    std::shared_ptr<Config> config = get_config();

    try
    {
        static char ip_buffer[64] = "192.168.";

        static bool entry_box_initialized = false;
        if (!entry_box_initialized)
        {
            if (config->contains_nested("connection.ip_address"))
            {
                std::string ip_address = config->get_nested("connection.ip_address", std::string(ip_buffer));
                strncpy(ip_buffer, ip_address.c_str(), sizeof(ip_buffer) - 1);
                ip_buffer[sizeof(ip_buffer) - 1] = '\0';
            }
            entry_box_initialized = true;
        }

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetFrameHeight() * 1.3f);

        float total_width = ImGui::GetContentRegionAvail().x;
        float button_width = 100.0f;
        float input_width = total_width - button_width - ImGui::GetStyle().ItemSpacing.x;

        bool is_connected = tcp->is_connected();

        auto connect = [&]() {
            try
            {
                tcp->connect(ip_buffer);
                config->set_nested("connection.ip_address", std::string(ip_buffer));
                config->save();
                get_notification_manager()->AddNotification("TCPGecko", 
                    std::format("Connected to: {}", std::string(ip_buffer)));
            }
            catch(const std::exception &e)
            {
                get_notification_manager()->AddErrorNotification("TCPGecko", e.what());
            }
        };

        auto disconnect = [&]() {
            tcp->disconnect();
            get_notification_manager()->AddNotification("TCPGecko", 
                std::format("Disconnected from: {}", std::string(ip_buffer)));
        };

        ImGui::PushItemWidth(input_width);
        if (ImGui::InputText("##IP Address", ip_buffer, IM_ARRAYSIZE(ip_buffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (!is_connected)
                connect();
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::BeginDisabled(!m_is_disconnect_allowed && is_connected);
        if (ImGui::Button(!is_connected ? "Connect" : "Disconnect", ImVec2(button_width, 0)))
        {
            if (!is_connected)
                connect();
            else
                disconnect();
        }
        ImGui::EndDisabled();
    }
    catch (const std::exception &e)
    {
        m_error_message = e.what();
        m_show_error_popup = true;
        ImGui::OpenPopup("Error");
    }

    if (m_show_error_popup)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("An error occurred:");
            
            static char error_buffer[2048];
            strncpy(error_buffer, m_error_message.c_str(), sizeof(error_buffer));
            error_buffer[sizeof(error_buffer) - 1] = '\0'; // null-terminate just in case

            ImGui::InputTextMultiline("##ErrorMessage", error_buffer, sizeof(error_buffer),
                                      ImVec2(400, 150), ImGuiInputTextFlags_ReadOnly);

            ImVec2 window_pos = ImGui::GetWindowPos();
            ImVec2 window_size = ImGui::GetWindowSize();
            ImVec2 window_end = ImVec2(window_pos.x + window_size.x, window_pos.y + window_size.y);

            ImVec2 mouse_pos = ImGui::GetIO().MouseClickedPos[0];

            bool clickedOutside = (mouse_pos.x < window_pos.x || mouse_pos.x > window_end.x ||
                                   mouse_pos.y < window_pos.y || mouse_pos.y > window_end.y) &&
                ImGui::IsMouseClicked(ImGuiKey_MouseLeft);

            if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvail().x, 0)) || clickedOutside)
            {
                ImGui::CloseCurrentPopup();
                m_show_error_popup = false;
            }

            ImGui::EndPopup();
        }
    }

    m_notif_manager->Update();

    ImGui::End();
}

std::shared_ptr<Config> RaimUI::get_config() const { return m_raim->get_config(); }
