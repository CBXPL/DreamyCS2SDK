#include "Menu.h"
#include "../../deps/ImGui/imgui.h"
#include "../Variables.h"
#include "../Obfuscation.h"

extern ImFont* g_MenuFont;

static bool g_bOpen = true;

void Menu::Render()
{
    if (!g_bOpen)
        return;

    ImGui::PushFont(g_MenuFont);

    ImGui::Begin(XorStr("DreamySDK"), &g_bOpen, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text(XorStr("Welcome to DreamySDK :D"));
    ImGui::Text(XorStr("You have hook, interfaces logging in the console"));
    ImGui::Text(XorStr("For more information look at the src code!"));

    ImGui::Separator();
    ImGui::Checkbox(XorStr("ESP"), &Vars::g_bEsp);

    ImGui::End();

    ImGui::PopFont();
}

bool Menu::IsOpen()
{
    return g_bOpen;
}

void Menu::Toggle()
{
    g_bOpen = !g_bOpen;
}
