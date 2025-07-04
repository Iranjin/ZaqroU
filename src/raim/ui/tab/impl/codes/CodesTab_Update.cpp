#include "CodesTab.h"

#include <imgui.h>

#include <utils/common.h>


void CodesTab::Update()
{
    apply_editor_color(m_code_frame_editor);
    
    ImGui::BeginChild("MainScrollArea");
    
    ImVec2 available = ImGui::GetContentRegionAvail();
    CodesTable(available);
    ControlsFrame(available);

    ImGui::EndChild();

    m_selected_entry_updated = false;
}

void CodesTab::UpdateBackground()
{
    apply_editor_color(m_code_window_editor);
    {
        ImGuiStyle &style = ImGui::GetStyle();
        ImVec4 *colors = style.Colors;
        TextEditor::Palette palette = m_code_window_editor.GetPalette();
        palette[(size_t) TextEditor::PaletteIndex::Background] = ImColor(colors[ImGuiCol_FrameBg]);
        m_code_window_editor.SetPalette(palette);
    }
    CodeWindow();
}
