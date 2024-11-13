//////////////////////////////////////////////////////////////////////////////////////////////
/*

 __     __                             _    _   _     _   _
 \ \   / /                            | |  | | | |   (_) | |
  \ \_/ /   _   _   _ __     ___      | |  | | | |_   _  | |  ___
   \   /   | | | | | '_ \   / _ \     | |  | | | __| | | | | / __|
    | |    | |_| | | | | | | (_) |    | |__| | | |_  | | | | \__ \
    |_|     \__,_| |_| |_|  \___/      \____/   \__| |_| |_| |___/


- Main.h

*/
//////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#pragma warning(disable : 4129 4101)
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////////////////
static const char* AppClass = "Yuno Utils";
static const char* AppName = "Yuno Utils";

static std::string folderName = "Yuno Utils";

static int switchTabs = 1;
static int WindowWidth = 950;
static int WindowHeight = 490;

bool TimerRes = false;
//////////////////////////////////////////////////////////////////////////////////////////////
static HWND hwnd = NULL;
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};
static ImFont* DefaultFont = nullptr;
//////////////////////////////////////////////////////////////////////////////////////////////
namespace fs = std::filesystem;
//////////////////////////////////////////////////////////////////////////////////////////////
void getConfigFolder() {

    std::string fullPath = "C:\\" + folderName;
    std::string secondaryPath = "C:\\" + folderName + "\\Apps";

    if (!fs::exists(fullPath) and !fs::exists(secondaryPath)) {
        std::cout << "folder don't exist, creating..." << std::endl;

        try {
            fs::create_directory(fullPath);
            fs::create_directory(secondaryPath);
        }
        catch (const fs::filesystem_error& e) {
        }
    }

    return;
}
//////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL) return E_FAIL;

    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0) return E_FAIL;

    return S_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}
//////////////////////////////////////////////////////////////////////////////////////////////
void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL) IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}
//////////////////////////////////////////////////////////////////////////////////////////////
#define WM_TRAYICON (WM_USER + 1)
NOTIFYICONDATA nid;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_TRAYICON:
        switch (LOWORD(lParam)) {
        case WM_LBUTTONUP:
            ShowWindow(hWnd, SW_RESTORE);
            SetForegroundWindow(hWnd);
            break;
        }
        break;
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) return 0;// Disable ALT application menu
        break;
    case WM_NCHITTEST:
    {
        ImVec2 Shit = ImGui::GetMousePos();
        if (Shit.y < 25 && Shit.x < WindowWidth - 25)
        {
            LRESULT hit = DefWindowProc(hWnd, msg, wParam, lParam);
            if (hit == HTCLIENT) hit = HTCAPTION;
            return hit;
        }
        else break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////
bool IsProcessRunning(const wchar_t* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return false;
    }

    do {
        if (_wcsicmp(pe32.szExeFile, processName) == 0) {
            CloseHandle(hSnapshot);
            return true;
        }

    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void style() {
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.95f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);

    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.77f, 0.52f, 2.35f, 1.0f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(12.00f, 10.00f);
    style.FramePadding = ImVec2(5.00f, 1.00f);
    style.CellPadding = ImVec2(6.00f, 6.00f);
    style.ItemSpacing = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
    style.IndentSpacing = 25;
    style.ScrollbarSize = 0;
    style.GrabMinSize = 10;
    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;
    style.WindowRounding = 7;
    style.ChildRounding = 4;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ScrollbarRounding = 9;
    style.GrabRounding = 3;
    style.LogSliderDeadzone = 4;
    style.TabRounding = 4;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void disableNvidiaTelemetry() {
    system("if exist \"%ProgramFiles%\\NVIDIA Corporation\\Installer2\\InstallerCore\\NVI2.DLL\" ("
        "rundll32 \"%PROGRAMFILES%\\NVIDIA Corporation\\Installer2\\InstallerCore\\NVI2.DLL\",UninstallPackage NvTelemetryContainer "
        "rundll32 \"%PROGRAMFILES%\\NVIDIA Corporation\\Installer2\\InstallerCore\\NVI2.DLL\",UninstallPackage NvTelemetry"
        ")");

    // Exclui arquivos residuais de telemetria da NVIDIA
    system("del /s %SystemRoot%\\System32\\DriverStore\\FileRepository\\NvTelemetry*.dll");
    system("rmdir /s /q \"%ProgramFiles(x86)%\\NVIDIA Corporation\\NvTelemetry\" 2>nul");
    system("rmdir /s /q \"%ProgramFiles%\\NVIDIA Corporation\\NvTelemetry\" 2>nul");

    // Opta por não participar da telemetria da NVIDIA
    system("reg add \"HKLM\\SOFTWARE\\NVIDIA Corporation\\NvControlPanel2\\Client\" /v \"OptInOrOutPreference\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKLM\\SOFTWARE\\NVIDIA Corporation\\Global\\FTS\" /v \"EnableRID44231\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKLM\\SOFTWARE\\NVIDIA Corporation\\Global\\FTS\" /v \"EnableRID64640\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKLM\\SOFTWARE\\NVIDIA Corporation\\Global\\FTS\" /v \"EnableRID66610\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKLM\\SYSTEM\\CurrentControlSet\\Services\\nvlddmkm\\Global\\Startup\" /v \"SendTelemetryData\" /t REG_DWORD /d 0 /f");

    // Desativa o serviço de contêiner de telemetria da NVIDIA
    system("PowerShell -ExecutionPolicy Unrestricted -Command \"$serviceName = 'NvTelemetryContainer'; Write-Host '^'\"'Disabling service: '^'\"'$serviceName'^'\"'.'^'\"'; <# -- 1. Skip if service does not exist #>; $service = Get-Service -Name $serviceName -ErrorAction SilentlyContinue; if(!$service) {; Write-Host '^'\"'Service '^'\"'$serviceName'^'\"' could not be not found, no need to disable it.'^'\"'; Exit 0; }; <# -- 2. Stop if running #>; if ($service.Status -eq [System.ServiceProcess.ServiceControllerStatus]::Running) {; Write-Host '^'\"'`'^'\"'$serviceName'^'\"' is running, stopping it.'^'\"'; try {; Stop-Service -Name '^'\"'$serviceName'^'\"' -Force -ErrorAction Stop; Write-Host '^'\"'Stopped '^'\"'$serviceName'^'\"' successfully.'^'\"'; } catch {; Write-Warning '^'\"'Could not stop '^'\"'$serviceName'^'\"'', it will be stopped after reboot: $_'^'\"'; }; } else {; Write-Host '^'\"'`'^'\"'$serviceName'^'\"' is not running, no need to stop.'^'\"'; }; <# -- 3. Skip if already disabled #>; $startupType = $service.StartType <# Does not work before .NET 4.6.1 #>; if(!$startupType) {; $startupType = (Get-WmiObject -Query '^'\"'Select StartMode From Win32_Service Where Name=''$serviceName'''^'\"' -ErrorAction Ignore).StartMode; if(!$startupType) {; $startupType = (Get-WmiObject -Class Win32_Service -Property StartMode -Filter '^'\"'Name=''$serviceName'''^'\"' -ErrorAction Ignore).StartMode; }; }; if($startupType -eq 'Disabled') {; Write-Host '^'\"'$serviceName is already disabled, no further action is needed'^'\"'; }; <# -- 4. Disable service #>; try {; Set-Service -Name '^'\"'$serviceName'^'\"' -StartupType Disabled -Confirm:$false -ErrorAction Stop; Write-Host '^'\"'Disabled '^'\"'$serviceName'^'\"' successfully.'^'\"'; } catch {; Write-Error '^'\"'Could not disable '^'\"'$serviceName'^'\"'': $_'^'\"'; }\"");

    // Desativa os serviços de telemetria da NVIDIA
    system("schtasks /change /TN NvTmMon_{B2FE1952-0186-46C3-BAEC-A80AA35AC5B8} /DISABLE");
    system("schtasks /change /TN NvTmRep_{B2FE1952-0186-46C3-BAEC-A80AA35AC5B8} /DISABLE");
    system("schtasks /change /TN NvTmRepOnLogon_{B2FE1952-0186-46C3-BAEC-A80AA35AC5B8} /DISABLE");

    return;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void disableCortana() {
    // Não permitir o Cortana
    system("reg add \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\Windows Search\" /v \"AllowCortana\" /t REG_DWORD /d 0 /f");

    // Não permitir a experiência do Cortana
    system("reg add \"HKLM\\SOFTWARE\\Microsoft\\PolicyManager\\default\\Experience\\AllowCortana\" /v \"value\" /t REG_DWORD /d 0 /f");

    // Não permitir a busca e o Cortana de pesquisar em fontes na nuvem como OneDrive e SharePoint
    system("reg add \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\Windows Search\" /v \"AllowCloudSearch\" /t REG_DWORD /d 0 /f");

    // Desativar a interação de fala do Cortana enquanto o sistema está bloqueado
    system("reg add \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\Windows Search\" /v \"AllowCortanaAboveLock\" /t REG_DWORD /d 0 /f");

    // Não permitir o consentimento do Cortana
    system("reg add \"HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Search\" /v \"CortanaConsent\" /t REG_DWORD /d 0 /f");

    // Não permitir que o Cortana seja ativado
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search\" /v \"CanCortanaBeEnabled\" /t REG_DWORD /d 0 /f");

    // Desativar o Cortana (resultados de pesquisa na Internet no menu Iniciar)
    system("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search\" /v \"CortanaEnabled\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search\" /v \"CortanaEnabled\" /t REG_DWORD /d 0 /f");

    // Remover o ícone da barra de tarefas do Cortana
    system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced /v \"ShowCortanaButton\" /t REG_DWORD /d 0 /f");

    // Desativar o Cortana no modo ambiente
    system("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search\" /v \"CortanaInAmbientMode\" /t REG_DWORD /d 0 /f");

    // Impedir o Cortana de exibir histórico
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search\" /v \"HistoryViewEnabled\" /t REG_DWORD /d 0 /f");

    // Impedir o Cortana de usar histórico do dispositivo
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search\" /v \"DeviceHistoryEnabled\" /t REG_DWORD /d 0 /f");

    // Desativar a ativação de voz "Hey Cortana"
    system("reg add \"HKCU\\Software\\Microsoft\\Speech_OneCore\\Preferences\" /v \"VoiceActivationOn\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKLM\\Software\\Microsoft\\Speech_OneCore\\Preferences\" /v \"VoiceActivationDefaultOn\" /t REG_DWORD /d 0 /f");

    // Desativar a escuta do Cortana para comandos no Windows key + C
    system("reg add \"HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Search\" /v \"VoiceShortcut\" /t REG_DWORD /d 0 /f");

    // Desativar o uso do Cortana mesmo quando o dispositivo está bloqueado
    system("reg add \"HKCU\\Software\\Microsoft\\Speech_OneCore\\Preferences\" /v \"VoiceActivationEnableAboveLockscreen\" /t REG_DWORD /d 0 /f");

    // Desativar a atualização automática de dados de fala
    system("reg add \"HKCU\\Software\\Microsoft\\Speech_OneCore\\Preferences\" /v \"ModelDownloadAllowed\" /t REG_DWORD /d 0 /f");

    // Desativar o suporte de voz do Cortana durante a configuração do Windows
    system("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\OOBE\" /v \"DisableVoice\" /t REG_DWORD /d 1 /f");

    return;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void disableOffice() {
    // Desativar o log do Microsoft Office
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\15.0\\Outlook\\Options\\Mail\" /v \"EnableLogging\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\16.0\\Outlook\\Options\\Mail\" /v \"EnableLogging\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\15.0\\Outlook\\Options\\Calendar\" /v \"EnableCalendarLogging\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\16.0\\Outlook\\Options\\Calendar\" /v \"EnableCalendarLogging\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\15.0\\Word\\Options\" /v \"EnableLogging\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\16.0\\Word\\Options\" /v \"EnableLogging\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\SOFTWARE\\Policies\\Microsoft\\Office\\15.0\\OSM\" /v \"EnableLogging\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\SOFTWARE\\Policies\\Microsoft\\Office\\16.0\\OSM\" /v \"EnableLogging\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\SOFTWARE\\Policies\\Microsoft\\Office\\15.0\\OSM\" /v \"EnableUpload\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\SOFTWARE\\Policies\\Microsoft\\Office\\16.0\\OSM\" /v \"EnableUpload\" /t REG_DWORD /d 0 /f");

    // Desativar a telemetria do cliente do Microsoft Office
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\Common\\ClientTelemetry\" /v \"DisableTelemetry\" /t REG_DWORD /d 1 /f");
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\16.0\\Common\\ClientTelemetry\" /v \"DisableTelemetry\" /t REG_DWORD /d 1 /f");
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\Common\\ClientTelemetry\" /v \"VerboseLogging\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\16.0\\Common\\ClientTelemetry\" /v \"VerboseLogging\" /t REG_DWORD /d 0 /f");

    // Desativar o Programa de Melhoria da Experiência do Cliente
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\15.0\\Common\" /v \"QMEnable\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\16.0\\Common\" /v \"QMEnable\" /t REG_DWORD /d 0 /f");

    // Desativar o feedback
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\15.0\\Common\\Feedback\" /v \"Enabled\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Office\\16.0\\Common\\Feedback\" /v \"Enabled\" /t REG_DWORD /d 0 /f");

    // Desativar o agente de telemetria do Office
    system("schtasks /change /TN \"Microsoft\\Office\\OfficeTelemetryAgentFallBack\" /DISABLE");
    system("schtasks /change /TN \"Microsoft\\Office\\OfficeTelemetryAgentFallBack2016\" /DISABLE");
    system("schtasks /change /TN \"Microsoft\\Office\\OfficeTelemetryAgentLogOn\" /DISABLE");
    system("schtasks /change /TN \"Microsoft\\Office\\OfficeTelemetryAgentLogOn2016\" /DISABLE");

    // Desativar o Subscription Heartbeat
    system("schtasks /change /TN \"Microsoft\\Office\\Office 15 Subscription Heartbeat\" /DISABLE");
    system("schtasks /change /TN \"Microsoft\\Office\\Office 16 Subscription Heartbeat\" /DISABLE");

    return;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void ccCleaner() {
    system("reg add \"HKCU\\Software\\Piriform\\CCleaner\" /v \"Monitoring\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\Software\\Piriform\\CCleaner\" /v \"HelpImproveCCleaner\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\Software\\Piriform\\CCleaner\" /v \"SystemMonitoring\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\Software\\Piriform\\CCleaner\" /v \"UpdateAuto\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\Software\\Piriform\\CCleaner\" /v \"UpdateCheck\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKCU\\Software\\Piriform\\CCleaner\" /v \"CheckTrialOffer\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKLM\\Software\\Piriform\\CCleaner\" /v \"(Cfg)HealthCheck\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKLM\\Software\\Piriform\\CCleaner\" /v \"(Cfg)QuickClean\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKLM\\Software\\Piriform\\CCleaner\" /v \"(Cfg)QuickCleanIpm\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKLM\\Software\\Piriform\\CCleaner\" /v \"(Cfg)GetIpmForTrial\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKLM\\Software\\Piriform\\CCleaner\" /v \"(Cfg)SoftwareUpdater\" /t REG_DWORD /d 0 /f");
    system("reg add \"HKLM\\Software\\Piriform\\CCleaner\" /v \"(Cfg)SoftwareUpdaterIpm\" /t REG_DWORD /d 0 /f");
    return;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void disableXboxLive() {
    // Xbox Live Auth Manager
    system("PowerShell -ExecutionPolicy Unrestricted -Command \"$serviceName = 'XblAuthManager'; Write-Host '^''Disabling service: ^''$serviceName^''.'^''; $service = Get-Service -Name $serviceName -ErrorAction SilentlyContinue; if(!$service) {; Write-Host '^''Service ^''$serviceName^'' could not be not found, no need to disable it.'^''; Exit 0; }; if ($service.Status -eq [System.ServiceProcess.ServiceControllerStatus]::Running) {; Write-Host '^''^''$serviceName^'' is running, stopping it.'^''; try {; Stop-Service -Name '^''$serviceName^'' -Force -ErrorAction Stop; Write-Host '^''Stopped ^''$serviceName^'' successfully.'^''; } catch {; Write-Warning '^''Could not stop ^''$serviceName^'', it will be stopped after reboot: $_'^''; }; } else {; Write-Host '^''^''$serviceName^'' is not running, no need to stop.'^''; }; $startupType = $service.StartType; if($startupType -eq 'Disabled') {; Write-Host '^''$serviceName is already disabled, no further action is needed'^''; }; try {; Set-Service -Name '^''$serviceName^'' -StartupType Disabled -Confirm:$false -ErrorAction Stop; Write-Host '^''Disabled ^''$serviceName^'' successfully.'^''; } catch {; Write-Error '^''Could not disable ^''$serviceName^'': $_'^''; }\"");

    // Xbox Live Game Save
    system("PowerShell -ExecutionPolicy Unrestricted -Command \"$serviceName = 'XblGameSave'; Write-Host '^''Disabling service: ^''$serviceName^''.'^''; $service = Get-Service -Name $serviceName -ErrorAction SilentlyContinue; if(!$service) {; Write-Host '^''Service ^''$serviceName^'' could not be not found, no need to disable it.'^''; Exit 0; }; if ($service.Status -eq [System.ServiceProcess.ServiceControllerStatus]::Running) {; Write-Host '^''^''$serviceName^'' is running, stopping it.'^''; try {; Stop-Service -Name '^''$serviceName^'' -Force -ErrorAction Stop; Write-Host '^''Stopped ^''$serviceName^'' successfully.'^''; } catch {; Write-Warning '^''Could not stop ^''$serviceName^'', it will be stopped after reboot: $_'^''; }; } else {; Write-Host '^''^''$serviceName^'' is not running, no need to stop.'^''; }; $startupType = $service.StartType; if($startupType -eq 'Disabled') {; Write-Host '^''$serviceName is already disabled, no further action is needed'^''; }; try {; Set-Service -Name '^''$serviceName^'' -StartupType Disabled -Confirm:$false -ErrorAction Stop; Write-Host '^''Disabled ^''$serviceName^'' successfully.'^''; } catch {; Write-Error '^''Could not disable ^''$serviceName^'': $_'^''; }\"");

    // Xbox Live Networking Service
    system("PowerShell -ExecutionPolicy Unrestricted -Command \"$serviceName = 'XboxNetApiSvc'; Write-Host '^''Disabling service: ^''$serviceName^''.'^''; $service = Get-Service -Name $serviceName -ErrorAction SilentlyContinue; if(!$service) {; Write-Host '^''Service ^''$serviceName^'' could not be not found, no need to disable it.'^''; Exit 0; }; if ($service.Status -eq [System.ServiceProcess.ServiceControllerStatus]::Running) {; Write-Host '^''^''$serviceName^'' is running, stopping it.'^''; try {; Stop-Service -Name '^''$serviceName^'' -Force -ErrorAction Stop; Write-Host '^''Stopped ^''$serviceName^'' successfully.'^''; } catch {; Write-Warning '^''Could not stop ^''$serviceName^'', it will be stopped after reboot: $_'^''; }; } else {; Write-Host '^''^''$serviceName^'' is not running, no need to stop.'^''; }; $startupType = $service.StartType; if($startupType -eq 'Disabled') {; Write-Host '^''$serviceName is already disabled, no further action is needed'^''; }; try {; Set-Service -Name '^''$serviceName^'' -StartupType Disabled -Confirm:$false -ErrorAction Stop; Write-Host '^''Disabled ^''$serviceName^'' successfully.'^''; } catch {; Write-Error '^''Could not disable ^''$serviceName^'': $_'^''; }\"");
    return;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void toggleGameMode(int change) {
    std::string commandOne = "reg add \"HKCU\\SOFTWARE\\Microsoft\\GameBar\" /v \"AllowAutoGameMode\" /t REG_DWORD /d " + std::to_string(change) + " /f";
    std::string commandTwo = "reg add \"HKCU\\SOFTWARE\\Microsoft\\GameBar\" /v \"AutoGameModeEnabled\" /t REG_DWORD /d " + std::to_string(change) + " /f";
    system(commandOne.c_str());
    system(commandTwo.c_str());
   return;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void toggleSmartScreen(int change) {
    std::string smartScreenStatus = (change == 0) ? "Off" : "On";

    std::string commandOne = "reg add \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\System\" /v \"EnablingSmartScreen\" /t REG_DWORD /d " + std::to_string(change) + " /f";
    std::string commandTwo = "reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\" /v \"SmartScreenEnabled\" /t REG_SZ /d \"" + smartScreenStatus + "\" /f";
    std::string commandThree = "reg add \"HKU\\!USER_SID!\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppHost\" /v \"EnablingWebContentEvaluation\" /t REG_DWORD /d " + std::to_string(change) + " /f";

    system(commandOne.c_str());
    system(commandTwo.c_str());
    system(commandThree.c_str());

    return;
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void disableWindowsDefender() {
    system("reg add \"HKLM\SOFTWARE\Policies\Microsoft\Windows Defender\" / v \"DisableAntiSpyware\" / t REG_DWORD / d 1 / f");
}
//////////////////////////////////////////////////////////////////////////////////////////////
void disableUpdates() {
    system("PowerShell -ExecutionPolicy Unrestricted -Command \"$serviceName = 'wuauserv'; Write-Host '^\"Disabling service: ^\"$serviceName^\".'^\"; $service = Get-Service -Name $serviceName -ErrorAction SilentlyContinue; if(!$service) {; Write-Host '^\"Service ^\"$serviceName^\" could not be not found, no need to disable it.'^\"; Exit 0; }; if ($service.Status -eq [System.ServiceProcess.ServiceControllerStatus]::Running) {; Write-Host '^\"^\"$serviceName^\" is running, stopping it.'^\"; try {; Stop-Service -Name '^\"$serviceName^\"' -Force -ErrorAction Stop; Write-Host '^\"Stopped ^\"$serviceName^\" successfully.'^\"; } catch {; Write-Warning '^\"Could not stop ^\"$serviceName^\"^\", it will be stopped after reboot: $_'^\"; }; } else {; Write-Host '^\"^\"$serviceName^\" is not running, no need to stop.'^\"; }; $startupType = $service.StartType; if(!$startupType) {; $startupType = (Get-WmiObject -Query '^\"Select StartMode From Win32_Service Where Name=''$serviceName''^\"' -ErrorAction Ignore).StartMode; if(!$startupType) {; $startupType = (Get-WmiObject -Class Win32_Service -Property StartMode -Filter '^\"Name=''$serviceName''^\"' -ErrorAction Ignore).StartMode; }; }; if($startupType -eq 'Disabled') {; Write-Host '^\"$serviceName is already disabled, no further action is needed'^\"; }; try {; Set-Service -Name '^\"$serviceName^\"' -StartupType Disabled -Confirm:$false -ErrorAction Stop; Write-Host '^\"Disabled ^\"$serviceName^\" successfully.'^\"; } catch {; Write-Error '^\"Could not disable ^\"$serviceName^\": $_'^\"; }\"");

    // Disable "Update Orchestrator Service" (`UsoSvc`) service
    system("PowerShell -ExecutionPolicy Unrestricted -Command \"$serviceName = 'UsoSvc'; Write-Host '^\"Disabling service: ^\"$serviceName^\".'^\"; $service = Get-Service -Name $serviceName -ErrorAction SilentlyContinue; if(!$service) {; Write-Host '^\"Service ^\"$serviceName^\" could not be not found, no need to disable it.'^\"; Exit 0; }; if ($service.Status -eq [System.ServiceProcess.ServiceControllerStatus]::Running) {; Write-Host '^\"^\"$serviceName^\" is running, stopping it.'^\"; try {; Stop-Service -Name '^\"$serviceName^\"' -Force -ErrorAction Stop; Write-Host '^\"Stopped ^\"$serviceName^\" successfully.'^\"; } catch {; Write-Warning '^\"Could not stop ^\"$serviceName^\"^\", it will be stopped after reboot: $_'^\"; }; } else {; Write-Host '^\"^\"$serviceName^\" is not running, no need to stop.'^\"; }; $startupType = $service.StartType; if(!$startupType) {; $startupType = (Get-WmiObject -Query '^\"Select StartMode From Win32_Service Where Name=''$serviceName''^\"' -ErrorAction Ignore).StartMode; if(!$startupType) {; $startupType = (Get-WmiObject -Class Win32_Service -Property StartMode -Filter '^\"Name=''$serviceName''^\"' -ErrorAction Ignore).StartMode; }; }; if($startupType -eq 'Disabled') {; Write-Host '^\"$serviceName is already disabled, no further action is needed'^\"; }; try {; Set-Service -Name '^\"$serviceName^\"' -StartupType Disabled -Confirm:$false -ErrorAction Stop; Write-Host '^\"Disabled ^\"$serviceName^\" successfully.'^\"; } catch {; Write-Error '^\"Could not disable ^\"$serviceName^\": $_'^\"; }\"");

    // Disable "Windows Update Medic Service" (`WaaSMedicSvc`) service
    system("PowerShell -ExecutionPolicy Unrestricted -Command \"$serviceQuery = 'WaaSMedicSvc'; $service = Get-Service -Name $serviceQuery -ErrorAction SilentlyContinue; if(!$service) {; Write-Host '^\"Service query ^\"$serviceQuery^\" did not yield any results, no need to disable it.'^\"; Exit 0; }; $serviceName = $service.Name; Write-Host '^\"Disabling service: ^\"$serviceName^\".'^\"; if ($service.Status -eq [System.ServiceProcess.ServiceControllerStatus]::Running) {; Write-Host '^\"^\"$serviceName^\" is running, trying to stop it.'^\"; try {; Stop-Service -Name '^\"$serviceName^\"' -Force -ErrorAction Stop; Write-Host '^\"Stopped ^\"$serviceName^\" successfully.'^\"; } catch {; Write-Warning '^\"Could not stop ^\"$serviceName^\"^\", it will be stopped after reboot: $_'^\"; }; } else {; Write-Host '^\"^\"$serviceName^\" is not running, no need to stop.'^\"; }; $registryKey = '^\"HKLM:\\SYSTEM\\CurrentControlSet\\Services\\$serviceName^\"'; if(!(Test-Path $registryKey)) {; Write-Host '^\"^\"$registryKey^\" is not found in registry, cannot enable it.'^\"; Exit 0; }; if( $(Get-ItemProperty -Path '^\"$registryKey^\"').Start -eq 4) {; Write-Host '^\"^\"$serviceName^\" is already disabled from start, no further action is needed.'^\"; Exit 0; }; try {; Set-ItemProperty $registryKey -Name Start -Value 4 -Force -ErrorAction Stop; Write-Host '^\"Disabled ^\"$serviceName^\" successfully.'^\"; } catch {; Write-Error '^\"Could not disable ^\"$serviceName^\": $_'^\"; }\"");

    // Disable Automatic Updates (AU) feature
    system("reg add \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU\" /v \"NoAutoUpdate\" /t \"REG_DWORD\" /d \"1\" /f");

    // Disable automatic installation of Windows updates without user consent
    system("reg add \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU\" /v \"AUOptions\" /t \"REG_DWORD\" /d \"2\" /f");

    // Disable automatic daily installation of Windows updates
    system("reg delete \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU\" /v \"ScheduledInstallDay\" /f 2>nul");

    // Disable scheduled automatic updates
    system("reg delete \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU\" /v \"ScheduledInstallTime\" /f 2>nul");

}
//////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {
    NTSTATUS NTAPI NtSetTimerResolution(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution);
    NTSTATUS NTAPI NtQueryTimerResolution(PULONG MinimumResolution, PULONG MaximumResolution, PULONG CurrentResolution);
}
//////////////////////////////////////////////////////////////////////////////////////////////
ULONG minRes, maxRes, currentRes;
ULONG newResolution = 5000;
//////////////////////////////////////////////////////////////////////////////////////////////
int getCurrentRes() {


    if (NtQueryTimerResolution(&minRes, &maxRes, &currentRes) == 0) {
        std::cout << "Current: " << currentRes << "  Min: " << minRes << "  Max: " << maxRes << std::endl;
    }
    else {
        return 1;
    }

    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
int setNormalRes() {


    if (NtSetTimerResolution(0, FALSE, &currentRes) == 0) {
        std::cout << "Changed to: " << currentRes << std::endl;
    }
    else {
        return 1;
    }

    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
int setMinimiumResolution() {
    if (NtSetTimerResolution(newResolution, TRUE, &currentRes) == 0) {
        std::cout << "Changed to:: " << currentRes << std::endl;
    }
    else {
        return 1;
    }

    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////