/*

 __     __                             _    _   _     _   _
 \ \   / /                            | |  | | | |   (_) | |
  \ \_/ /   _   _   _ __     ___      | |  | | | |_   _  | |  ___
   \   /   | | | | | '_ \   / _ \     | |  | | | __| | | | | / __|
    | |    | |_| | | | | | | (_) |    | |__| | | |_  | | | | \__ \
    |_|     \__,_| |_| |_|  \___/      \____/   \__| |_| |_| |___/


- Main.cpp

*/
//////////////////////////////////////////////////////////////////////////////////////////////
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#pragma warning(disable : 4244)
#define DIRECTINPUT_VERSION 0x0800
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////////////////
void MinimizeToTray(HWND hwnd) {
    NOTIFYICONDATA nid;
    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    lstrcpy(nid.szTip, _T("Yuno Utils"));
    Shell_NotifyIcon(NIM_ADD, &nid);
    ShowWindow(hwnd, SW_HIDE);
}
//////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////
void Principais() {

    ImGui::BeginChild(48, ImVec2(300, 449), true);

    ImGui::Text("Explorer");
    if (IsProcessRunning(L"explorer.exe")) {
        if (ImGui::Button("Kill", ImVec2(275, 20))) {
            system("taskkill /f /im explorer.exe");
        }
    }
    else {
        if (ImGui::Button("Open", ImVec2(275, 20))) {
            ShellExecute(NULL, L"open", L"explorer.exe", NULL, NULL, SW_SHOWNORMAL);
        }
    }

    ImGui::Separator();
    ImGui::Text("Temporary Files");
    if (ImGui::Button("Clean", ImVec2(275, 20))) {

        system("DEL /S /Q \"%TMP%\\*.*\"");
        system("DEL /S /Q \"%TEMP%\\*.*\"");
        system("DEL /S /Q \"%WINDIR%\\Temp\\*.*\"");
        system("DEL /S /Q \"%USERPROFILE%\\Local Settings\\Temp\\*.*\"");
        system("DEL /S /Q \"%LOCALAPPDATA%\\Temp\\*.*\"");
    }


    ImGui::Separator();
    ImGui::Text("Game Mode");
    if (ImGui::Button("Disable", ImVec2(135, 20))) {
        toggleGameMode(0);
    }

    ImGui::SameLine();
    if (ImGui::Button("Enable", ImVec2(135, 20))) {
        toggleGameMode(1);
    }

    ImGui::Separator();
    ImGui::Text("SmartScreen");
    if (ImGui::Button("Disable", ImVec2(135, 20))) {
        toggleSmartScreen(0);
    }

    ImGui::SameLine();
    if (ImGui::Button("Enable", ImVec2(135, 20))) {
        toggleSmartScreen(1);
    }

    ImGui::Separator();
    ImGui::Text("Sticky Keys | Teclas De Aderencia");
    if (ImGui::Button("Off", ImVec2(135, 20))) {
        SetStickyKeysFlag(L"0");
    }

    ImGui::SameLine();
    if (ImGui::Button("On", ImVec2(135, 20))) {
        SetStickyKeysFlag(L"1");
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild(51, ImVec2(300, 449), true);

    ImGui::Text("Disable");
    ImGui::Separator();
    if (ImGui::Button("Nvidia Telemetry", ImVec2(275, 20))) {
        disableNvidiaTelemetry();
    }

    if (ImGui::Button("Cortana", ImVec2(275, 20))) {
        disableCortana();
    }

    if (ImGui::Button("Microsoft Office", ImVec2(275, 20))) {
        disableOffice();
    }

    if (ImGui::Button("Xbox Live", ImVec2(275, 20))) {
        disableXboxLive();
    }

    if (ImGui::Button("CCleaner Monitoring", ImVec2(275, 20))) {
        ccCleaner();
    }

    if (ImGui::Button("Updates", ImVec2(275, 20))) {
        disableUpdates();
    }

    if (ImGui::Button("Windows Defender", ImVec2(275, 20))) {
        disableWindowsDefender();
    }

    if (ImGui::Button("GameBar Kill", ImVec2(275, 20))) {
        system("taskkill /f /im GameBar.exe");
        system("taskkill /f /im GameBarFTServer.exe");
    }
    ImGui::EndChild();
    ImGui::SameLine();

    ImGui::BeginGroup();

    ImGui::BeginChild(54, ImVec2(300, 270), true);
    ImGui::Text("Credits");
    ImGui::Separator();
    ImGui::Text("Dev/Creator: Anderson Niprus");
    ImGui::Separator();
    ImGui::TextWrapped("Introducing a desktop program designed to assist and optimize your Windows experience while gaming. This application leverages advanced techniques to enhance system performance and streamline your gaming sessions. With just a click, it helps you achieve a smoother and more efficient Windows environment, ensuring an optimal gaming experience.");
    if (ImGui::Button("Github", ImVec2(275, 25))) {
        ShellExecute(NULL, L"open", L"https://github.com/gabrielnips", NULL, NULL, SW_SHOWNORMAL);
    }
    if (ImGui::Button("Minimize to a tray", ImVec2(275, 25))) {
        MinimizeToTray(hwnd);
    }
    ImGui::EndChild();

    ImGui::Spacing();

    ImGui::BeginChild(58, ImVec2(300, 166), true);
    ImGui::Text("Timer Resolution");
    ImGui::Separator();
    ImGui::Text("Max Res: %.3fms", static_cast<double>(minRes) / 10000);
    ImGui::Text("Min Res: %.3fms", static_cast<double>(maxRes) / 10000);
    ImGui::Text("Current Res: %.3fms", static_cast<double>(currentRes) / 10000);
    ImGui::Separator();
    ImGui::Checkbox("Enable", &TimerRes);
    if (TimerRes) {
        setMinimiumResolution();
    }
    else {
        setNormalRes();
    }

    ImGui::EndChild();
    ImGui::EndGroup();

}
//////////////////////////////////////////////////////////////////////////////////////////////
int main(int, char**)
{
    getConfigFolder();
    getCurrentRes();
    setNormalRes();
    if (CountProcessInstances(L"Yuno Utils.exe") > 1) {
        MessageBox(NULL, L"Another Instance Is Running.", L"Yuno Utils - Error", MB_OK | MB_ICONEXCLAMATION);
        return 1;
    }

    RECT desktop;
    GetWindowRect(GetDesktopWindow(), &desktop);
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"Yuno Utils", NULL };
    RegisterClassEx(&wc);

    hwnd = CreateWindowEx(WS_EX_LAYERED, L"Yuno Utils", L"Yuno Utils", WS_POPUP, (desktop.right / 2) - (WindowWidth / 2), (desktop.bottom / 2) - (WindowHeight / 2), WindowWidth, WindowHeight, 0, 0, wc.hInstance, 0);

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
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    io.LogFilename = NULL;
    io.Fonts->AddFontDefault();
    ImFont* font_body = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 15.5f, NULL, io.Fonts->GetGlyphRangesDefault());
    IM_ASSERT(body != NULL);

    style();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    static bool open = true;
    DWORD dwFlag = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;
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
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin(" ", &open, dwFlag);


            ImGui::PushFont(font_body);

            ImVec4 titlebackgroundColor = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, titlebackgroundColor);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
            ImGui::BeginChild(1, ImVec2(1050, 20), true);
            ImGui::PopStyleColor();
            ImGui::PopStyleVar(2);


            float contentPosY = ImGui::GetContentRegionAvail().y / 10.0f;
            ImGui::SetCursorPosY(contentPosY);

            ImVec4 textColor = ImVec4(1.77f, 0.52f, 2.35f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
            float textWidth = ImGui::CalcTextSize("Yuno Utils - v1.1").x;
            float textPosX = ImGui::GetContentRegionAvail().x * 0.45f - textWidth * 0.45f;
            ImGui::SetCursorPosX(textPosX);
            ImGui::Text("Yuno Utils - v1.1");
            ImGui::PopStyleColor();
            ImGui::SameLine();
            float textPos = ImGui::GetContentRegionAvail().x / 0.56f;
            ImGui::SetCursorPosX(textPos);
            /*
            if (ImGui::Button("-", ImVec2(17, 17))) {
                MinimizeToTray(hwnd);
            }
            ImGui::SameLine();*/
            if (ImGui::Button("x", ImVec2(17, 17))) {
                exit(0);
            }
            ImGui::EndChild();

            float mousePos = ImGui::GetContentRegionAvail().x / 50.0f;
            ImGui::SetCursorPosX(mousePos);
            Principais();

            ImGui::PopFont();
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
//////////////////////////////////////////////////////////////////////////////////////////////