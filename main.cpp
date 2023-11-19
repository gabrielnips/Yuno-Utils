#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#define DIRECTINPUT_VERSION 0x0800
#include "main.h"
#include <windows.h>
#include <ShlObj.h>
#include <iostream> 
#include <cstdlib>
#include <tlhelp32.h>

#include <windows.h>
#include <shellapi.h>
#include <iostream>

int CountProcessInstances(const std::wstring& process_name) {
    int count = 0;

    HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (handle == INVALID_HANDLE_VALUE) {
        return count;
    }

    PROCESSENTRY32 process_entry;
    process_entry.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(handle, &process_entry)) {
        CloseHandle(handle);
        return count;
    }

    do {
        std::wstring current_process(process_entry.szExeFile);
        if (current_process == process_name) {
            count++;
        }
    } while (Process32Next(handle, &process_entry));

    CloseHandle(handle);
    return count;
}

bool SetStickyKeysFlag(const wchar_t* newValue) {

    LONG result = RegSetKeyValueW(
        HKEY_CURRENT_USER,
        L"Control Panel\\Accessibility\\StickyKeys",
        L"Flags",
        REG_SZ,
        newValue,
        static_cast<DWORD>((wcslen(newValue) + 1) * sizeof(wchar_t)) 
    );
    return 0;
}

void Principais() {
    ImGui::BeginChild(48, ImVec2(300, 449), true);
    ImGui::Text("Explorer");
    if (ImGui::Button("Kill", ImVec2(100, 25))) {
        system("taskkill /f /im explorer.exe");
    }
    ImGui::SameLine();
    if (ImGui::Button("Open", ImVec2(100, 25))) {
        system("taskkill /f /im explorer.exe");
    }
    ImGui::Separator();
    ImGui::Text("Temporary Files");
    if (ImGui::Button("Clean", ImVec2(100, 25))) {
        system("DEL /S /Q \"%TMP%\\*.*\"");
        system("DEL /S /Q \"%TEMP%\\*.*\"");
        system("DEL /S /Q \"%WINDIR%\\Temp\\*.*\"");
        system("DEL /S /Q \"%USERPROFILE%\\Local Settings\\Temp\\*.*\"");
        system("DEL /S /Q \"%LOCALAPPDATA%\\Temp\\*.*\"");
    }
    ImGui::Separator();
    ImGui::Text("GameBar");
    if (ImGui::Button("Kill", ImVec2(100, 25))) {
        system("taskkill /f /im GameBar.exe");
        system("taskkill /f /im GameBarFTServer.exe");
    }
    ImGui::Separator();
    ImGui::Text("Sticky Keys | Teclas De Aderencia");
    if (ImGui::Button("Off", ImVec2(100, 25))) {
        SetStickyKeysFlag(L"0");
    }
    ImGui::SameLine();
    if (ImGui::Button("On", ImVec2(100, 25))) {
        SetStickyKeysFlag(L"1");
    }
    ImGui::Separator();
    ImGui::Text("Disable");
    if (ImGui::Button("Nvidia Telemetry", ImVec2(150, 25))) {
        disableNvidiaTelemetry();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cortana", ImVec2(100, 25))) {
        disableCortana();
    }
    if (ImGui::Button("Microsoft Office", ImVec2(150, 25))) {
        disableOffice();
    }
    ImGui::SameLine();
    if (ImGui::Button("Xbox Live", ImVec2(100, 25))) {
        disableXboxLive();
    }
    if (ImGui::Button("CCleaner Monitoring", ImVec2(150, 25))) {
        ccCleaner();
    }
    ImGui::SameLine();
    if (ImGui::Button("Updates", ImVec2(100, 25))) {
        disableUpdates();
    }
    if (ImGui::Button("Windows Defender", ImVec2(150, 25))) {
        disableWindowsDefender();
    }

    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild(51, ImVec2(300, 449), true);
 
    ImGui::EndChild();
}

void TimerRes() {
    ImGui::BeginChild(48, ImVec2(300, 449), true);

    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild(51, ImVec2(300, 449), true);

    ImGui::EndChild();
}
void Creditos() {
    ImGui::BeginChild(48, ImVec2(300, 449), true);
    ImGui::Text("Credits");
    ImGui::Separator();
    ImGui::Text("Dev/Creator: Gabriel Nips");
    ImGui::Separator();
    ImGui::TextWrapped("Introducing a desktop program designed to assist and optimize your Windows experience while gaming. This application leverages advanced techniques to enhance system performance and streamline your gaming sessions. With just a click, it helps you achieve a smoother and more efficient Windows environment, ensuring an optimal gaming experience.");
    if (ImGui::Button("Github", ImVec2(100, 25))) {
        ShellExecute(NULL, L"open", L"https://github.com/gabrielnips", NULL, NULL, SW_SHOWNORMAL);
    }
    ImGui::EndChild();
}


int main(int, char**)
{
    if (CountProcessInstances(L"Yuno Utils.exe") > 1) {
        MessageBox(NULL, L"Another Instance Is Running.", L"Yuno Utils - Error", MB_OK | MB_ICONEXCLAMATION);
        return 1;
    }
  

    RECT desktop;
    GetWindowRect(GetDesktopWindow(), &desktop);
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"Yuno Utils", NULL };
    RegisterClassEx(&wc);

    hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, L"Yuno Utils", L"Yuno Utils", WS_POPUP, (desktop.right / 2) - (WindowWidth / 2), (desktop.bottom / 2) - (WindowHeight / 2), WindowWidth, WindowHeight, 0, 0, wc.hInstance, 0);

    //SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, ULW_COLORKEY);

    if (CreateDeviceD3D(hwnd) < 0)
    {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    io.LogFilename = NULL;
    DefaultFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Jura-VariableFont_wght.ttf", 16.5f, NULL, io.Fonts->GetGlyphRangesCyrillic());  
    style();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    static bool open = true;
    DWORD dwFlag = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        if (!open) ExitProcess(EXIT_SUCCESS);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
        {
            ImGui::Begin(" ", &open, dwFlag);
            ImVec2 contentRegion = ImGui::GetContentRegionAvail();
            ImVec2 contentPos = ImVec2(contentRegion.x * 0.5f, contentRegion.y * 0.5f);

            ImGui::AlignTextToFramePadding();


            TitleText("Yuno Utils", ImVec4(1.77f, 0.52f, 2.35f, 1.0f)); 
            if (ImGui::Button("Main", ImVec2(198, 30)))
                switchTabs = 1;
            ImGui::SameLine(0.0, 2.0f);
            if (ImGui::Button("Timer Resolution", ImVec2(198, 30)))
                switchTabs = 2;
            ImGui::SameLine(0.0, 2.0f);
            if (ImGui::Button("Credits", ImVec2(198, 30)))
                switchTabs = 3;

            switch (switchTabs) {
            case 1:
                Principais();
                break;
            case 2:
                TimerRes();
                break;
            case 3:
                Creditos();
                break;
            }

        }
        ImGui::End();

        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}