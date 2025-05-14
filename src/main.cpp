#include <filesystem>
#include <vector>

#include "imgui.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "utils/downloader.h"
#include "raim/Raim.h"


extern bool WaitEvents;

Raim *appInstance = nullptr;


int main()
{
    if (!glfwInit())
        return -1;

    // OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1200, 600, "Zaqro U", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1.0); // VSync

    // Initialize
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    { // Load fonts
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
            
            std::vector<uint8_t> data;
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

    appInstance = new Raim(window);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        if (WaitEvents)
            glfwWaitEvents();
        else
            glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        appInstance->Update();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    delete appInstance;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
