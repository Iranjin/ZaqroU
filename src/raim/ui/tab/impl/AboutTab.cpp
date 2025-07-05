#include "AboutTab.h"

#include <utils/common.h>
#include <constants.h>

#include <imgui.h>

#include <format>


AboutTab::AboutTab(RaimUI *raim_ui)
    : IRaimTab(raim_ui, "About")
{
}

void AboutTab::Update()
{
    ImGui::Text("Zaqro U");
    ImGui::Separator();

    ImGui::Text("Build Date: %s %s", __DATE__, __TIME__);
    ImGui::Text("Author: Iranjin");
    ImGui::Text("License: GPL-3.0");

    ImGui::Spacing();
    ImGui::Text("For more information, visit:");
    ImGui::Bullet(); ImGui::TextLinkOpenURL("https://github.com/Iranjin/ZaqroU");

    ImGui::Spacing();
    ImGui::Text("Libraries Used:");
    ImGui::Bullet(); ImGui::TextLinkOpenURL("GLFW", "https://www.glfw.org/");
    ImGui::Bullet(); ImGui::TextLinkOpenURL("OpenGL", "https://www.opengl.org/");
    ImGui::Bullet(); ImGui::TextLinkOpenURL("GLAD", "https://github.com/Dav1dde/glad");
    ImGui::Bullet(); ImGui::TextLinkOpenURL("ImGui", "https://github.com/ocornut/imgui");
    ImGui::Bullet(); ImGui::TextLinkOpenURL("ImGuiColorTextEdit", "https://github.com/BalazsJako/ImGuiColorTextEdit");
    ImGui::Bullet(); ImGui::TextLinkOpenURL("imgui_markdown", "https://github.com/enkisoftware/imgui_markdown");
    ImGui::Bullet(); ImGui::TextLinkOpenURL("cURL", "https://curl.se/libcurl/");
    ImGui::Bullet(); ImGui::TextLinkOpenURL("LZ4", "https://github.com/lz4/lz4");
    ImGui::Bullet(); ImGui::TextLinkOpenURL("nlohmann/json", "https://github.com/nlohmann/json");
    ImGui::Bullet(); ImGui::TextLinkOpenURL("tinyxml2", "https://github.com/leethomason/tinyxml2");
    ImGui::Bullet(); ImGui::TextLinkOpenURL("tinyfiledialogs", "https://sourceforge.net/projects/tinyfiledialogs/");
    ImGui::Bullet(); ImGui::TextLinkOpenURL("Boost.Asio", "https://www.boost.org/doc/libs/release/libs/asio/");
}
