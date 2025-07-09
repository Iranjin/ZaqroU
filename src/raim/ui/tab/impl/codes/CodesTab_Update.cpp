#include "CodesTab.h"

#include <imgui.h>

#include <utils/common.h>


void CodesTab::Update()
{
    m_code_frame_editor.SetPalette(default_editor_color());
    
    ImGui::BeginChild("MainScrollArea");
    
    ImVec2 available = ImGui::GetContentRegionAvail();
    CodesTable(available);
    ControlsFrame(available);

    ImGui::EndChild();

    m_selected_entry_updated = false;
}

void CodesTab::UpdateBackground()
{
    {
        TextEditor::Palette palette = default_editor_color();

        ImGuiStyle &style = ImGui::GetStyle();
        ImVec4 *colors = style.Colors;
        palette[(size_t) TextEditor::PaletteIndex::Background] = ImColor(colors[ImGuiCol_FrameBg]);
        
        m_code_window_editor.SetPalette(palette);
    }

    CodeWindow();
}
