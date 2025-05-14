#pragma once

#include <imgui.h>


typedef int ImGuiAlign;

enum ImGuiAlign_ : int
{
    ImGuiAlign_Left = 0,
    ImGuiAlign_Center = 1,
    ImGuiAlign_Right = 2
};

namespace ImGui
{

void Align(float width, ImGuiAlign alignment);
void Align(const char *label, ImGuiAlign alignment);

bool CheckboxFilled(const char *label, bool *value);
    
} // namespace ImGui
