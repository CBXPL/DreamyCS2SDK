#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "deps/MinHook.h"
#include "deps/ImGui/kiero.h"
#include "deps/ImGui/imgui.h"
#include "src/Offsets.h"
#include "deps/ImGui/imgui_impl_win32.h"
#include "deps/ImGui/imgui_impl_dx11.h"
#include "src/GUI/Menu.h"
#include "src/GUI/Overlay.h"
#include "src/Features/esp.h"
#include "src/SDK/Interfaces.h"
#include "src/SDK/Hooks.h"
#include <d3d11.h>
#include <dxgi.h>
#include <iostream>
#include "src/Obfuscation.h"

ImFont* g_MenuFont = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall hkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef HRESULT(__stdcall* Present_t)(IDXGISwapChain*, UINT, UINT);
Present_t oPresent = nullptr;

HWND g_hWindow = nullptr;
WNDPROC oWndProc = nullptr;
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dContext = nullptr;

void CreateRenderTarget(IDXGISwapChain* pSwapChain, ID3D11RenderTargetView** ppRTV)
{
    ID3D11Texture2D* backBuffer = nullptr;
    if (SUCCEEDED(pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))) && backBuffer)
    {
        g_pd3dDevice->CreateRenderTargetView(backBuffer, nullptr, ppRTV);
        backBuffer->Release();
    }
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    static bool init = false;
    if (!init)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pd3dDevice)))
        {
            g_pd3dDevice->GetImmediateContext(&g_pd3dContext);

            DXGI_SWAP_CHAIN_DESC desc;
            pSwapChain->GetDesc(&desc);
            g_hWindow = desc.OutputWindow;

            oWndProc = (WNDPROC)SetWindowLongPtrW(g_hWindow, GWLP_WNDPROC, (LONG_PTR)hkWndProc);

            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

            ImFont* font = io.Fonts->AddFontFromFileTTF(XorStr("C:\\Windows\\Fonts\\verdana.ttf"), 24.0f);
            if (!font)
                font = io.Fonts->AddFontFromFileTTF(XorStr("C:\\Windows\\Fonts\\Verdana.ttf"), 24.0f);
            if (!font)
                font = io.Fonts->AddFontDefault();

            g_MenuFont = io.Fonts->AddFontFromFileTTF(XorStr("C:\\Windows\\Fonts\\verdana.ttf"), 16.0f);
            if (!g_MenuFont)
                g_MenuFont = io.Fonts->AddFontFromFileTTF(XorStr("C:\\Windows\\Fonts\\Verdana.ttf"), 16.0f);
            if (!g_MenuFont)
                g_MenuFont = font;

            ImGui_ImplWin32_Init(g_hWindow);
            ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dContext);

            init = true;
        }
    }

    if (init)
    {
        ID3D11RenderTargetView* rtv = nullptr;
        CreateRenderTarget(pSwapChain, &rtv);
        if (rtv)
        {
            g_pd3dContext->OMSetRenderTargets(1, &rtv, nullptr);

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            Overlay::Watermark();
            ESP::Render();

            if (Menu::IsOpen())
                Menu::Render();

            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            rtv->Release();
        }
    }

    return oPresent(pSwapChain, SyncInterval, Flags);
}

LRESULT __stdcall hkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_KEYDOWN && wParam == VK_INSERT)
        Menu::Toggle();

    if (Menu::IsOpen() && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    return CallWindowProcW(oWndProc, hWnd, msg, wParam, lParam);
}

DWORD WINAPI InitThread(LPVOID)
{
    AllocConsole();
    freopen(XorStr("CONOUT$"), "w", stdout);
    SetConsoleTitleA(XorStr("DreamySDK"));

    LoadLibraryA(XorStr("d3d11.dll"));

    if (MH_Initialize() != MH_OK)
    {
        std::cout << XorStr("[Init] MH_Initialize failed!") << std::endl;
        return 1;
    }

    dreamy_offsets::Initialize();
    Interfaces::Initialize();
    Hooks::Initialize();

    for (int i = 0; i < 5; i++)
    {
        if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
        {
            void* presentAddress = (void*)kiero::getMethodsTable()[8];
            if (MH_CreateHook(presentAddress, hkPresent, (void**)&oPresent) == MH_OK &&
                MH_EnableHook(presentAddress) == MH_OK)
                return 0;
        }
        Sleep(500);
    }

    return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, InitThread, nullptr, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        if (g_hWindow && oWndProc)
            SetWindowLongPtrW(g_hWindow, GWLP_WNDPROC, (LONG_PTR)oWndProc);
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        break;
    }
    return TRUE;
}
