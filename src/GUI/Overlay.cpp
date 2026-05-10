#include "Overlay.h"
#include "../../deps/ImGui/imgui.h"
#include "../Obfuscation.h"

void Overlay::Watermark()
{
    ImDrawList* draw = ImGui::GetForegroundDrawList();
    draw->AddText(
        ImGui::GetFont(), 18.0f,
        ImVec2(15, 10),
        ImColor(0, 255, 200),
        XorStr("DreamySDK v0.1")
    );
}
