#include "esp.h"
#include "../../deps/ImGui/imgui.h"
#include "../Variables.h"
#include "../Offsets.h"
#include <windows.h>

static uintptr_t g_Client = 0;
static float g_ViewMatrix[4][4];

bool ESP::WorldToScreen(const Vector3& world, Vector3& screen)
{
    float w = g_ViewMatrix[3][0] * world.x + g_ViewMatrix[3][1] * world.y + g_ViewMatrix[3][2] * world.z + g_ViewMatrix[3][3];
    if (w < 0.001f) return false;

    float invW = 1.f / w;
    float x = (g_ViewMatrix[0][0] * world.x + g_ViewMatrix[0][1] * world.y + g_ViewMatrix[0][2] * world.z + g_ViewMatrix[0][3]) * invW;
    float y = (g_ViewMatrix[1][0] * world.x + g_ViewMatrix[1][1] * world.y + g_ViewMatrix[1][2] * world.z + g_ViewMatrix[1][3]) * invW;

    ImVec2 size = ImGui::GetIO().DisplaySize;
    screen.x = (size.x * 0.5f) + (x * size.x * 0.5f);
    screen.y = (size.y * 0.5f) - (y * size.y * 0.5f);
    return true;
}

void ESP::Draw3DBox(const Vector3& origin, float w, float h, uint32_t color)
{
    Vector3 corners[8] = {
        {origin.x - w, origin.y - w, origin.z},
        {origin.x + w, origin.y - w, origin.z},
        {origin.x - w, origin.y + w, origin.z},
        {origin.x + w, origin.y + w, origin.z},
        {origin.x - w, origin.y - w, origin.z + h},
        {origin.x + w, origin.y - w, origin.z + h},
        {origin.x - w, origin.y + w, origin.z + h},
        {origin.x + w, origin.y + w, origin.z + h},
    };

    Vector3 screen[8];
    for (int i = 0; i < 8; i++)
        if (!WorldToScreen(corners[i], screen[i]))
            return;

    ImDrawList* draw = ImGui::GetBackgroundDrawList();

    struct { int a, b; } edges[12] = {
        {0,1},{1,3},{3,2},{2,0},
        {4,5},{5,7},{7,6},{6,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    for (auto& e : edges)
    {
        ImVec2 a(screen[e.a].x, screen[e.a].y);
        ImVec2 b(screen[e.b].x, screen[e.b].y);
        draw->AddLine(a, b, color, 1.5f);
    }
}

void ESP::Render()
{
    if (!Vars::g_bEsp)
        return;

    if (!g_Client) {
        g_Client = (uintptr_t)GetModuleHandleA("client.dll");
        if (!g_Client) return;
    }

    using namespace dreamy_offsets::offsets;
    using namespace client_dll;

    uintptr_t vmAddr = g_Client + client_dll::dwViewMatrix;
    memcpy(&g_ViewMatrix, (void*)vmAddr, sizeof(g_ViewMatrix));

    uintptr_t entityList = *(uintptr_t*)(g_Client + client_dll::dwEntityList);
    if (!entityList || entityList < 0x1000000) return;

    uintptr_t localController = *(uintptr_t*)(g_Client + client_dll::dwLocalPlayerController);
    if (!localController || localController < 0x1000000) return;

    uint32_t localHandle = *(uint32_t*)(localController + client_offsets::m_hPlayerPawn);
    bool localValid = (localHandle != 0 && localHandle != 0xFFFFFFFF);

    uintptr_t localPawn = 0;
    int localTeam = 0;
    if (localValid) {
        uintptr_t entry = *(uintptr_t*)(entityList + 0x10 + 8 * ((localHandle & 0x7FFF) >> 9));
        if (entry) {
            localPawn = *(uintptr_t*)(entry + 0x70 * (localHandle & 0x1FF));
            if (localPawn && localPawn >= 0x1000000)
                localTeam = (int) * (uint8_t*)(localPawn + client_offsets::m_iTeamNum);
        }
    }

    for (int i = 0; i < 64; i++)
    {
        uintptr_t listEntry = *(uintptr_t*)(entityList + 0x10 + 8 * ((i & 0x7FFF) >> 9));
        if (!listEntry) continue;

        uintptr_t controller = *(uintptr_t*)(listEntry + 0x70 * (i & 0x1FF));
        if (!controller || controller < 0x1000000) continue;

        uint32_t pawnHandle = *(uint32_t*)(controller + client_offsets::m_hPlayerPawn);
        if (pawnHandle == 0 || pawnHandle == 0xFFFFFFFF) continue;
        if (localValid && pawnHandle == localHandle) continue;

        uintptr_t pawnEntry = *(uintptr_t*)(entityList + 0x10 + 8 * ((pawnHandle & 0x7FFF) >> 9));
        if (!pawnEntry) continue;

        uintptr_t pawn = *(uintptr_t*)(pawnEntry + 0x70 * (pawnHandle & 0x1FF));
        if (!pawn || pawn < 0x1000000) continue;

        int team = (int) * (uint8_t*)(pawn + client_offsets::m_iTeamNum);
        if (team != 2 && team != 3) continue;

        int health = *(int*)(pawn + client_offsets::m_iHealth);
        if (health <= 0 || health > 100) continue;

        Vector3 origin = *(Vector3*)(pawn + client_offsets::m_vOldOrigin);

        uint32_t color;
        if (team == localTeam)
            color = ImColor(0, 220, 0);
        else
            color = ImColor(220, 40, 40);

        Draw3DBox(origin, 16.f, 72.f, color);
    }
}
