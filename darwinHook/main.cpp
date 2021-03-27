#define _CRT_SECURE_NO_WARNINGS
#include "includes.h"

typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
static EndScene oEndScene = NULL;
LPDIRECT3DDEVICE9 pDevice = nullptr;

WNDPROC oWndProc;
static HWND window = NULL;
ColorStr BoxColor;

int screenX = GetSystemMetrics(SM_CXSCREEN);
int screenY = GetSystemMetrics(SM_CYSCREEN);

uintptr_t BaseAddress;
uintptr_t baseEngine;
int* iShotsFired;
Vec3* aimRecoilPunch;
Vec3 oPunch{ 0,0,0 };
Vec3* viewAngles;

bool WorldTooScreen(Vec3 pos, Vec2& screen, float matrix[16], int width, int height) {
    Vec4 clipCoords;
    clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
    clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
    clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
    clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

    if (clipCoords.w < 0.1f) {
        return false;
    }

    Vec3 NDC;
    NDC.x = clipCoords.x / clipCoords.w;
    NDC.y = clipCoords.y / clipCoords.w;
    NDC.z = clipCoords.z / clipCoords.w;

    screen.x = (width / 2 * NDC.x) + (NDC.x + width / 2);
    screen.y = -(height / 2 * NDC.y) + (NDC.y + height / 2);

    return true;
}

Vec2 GetBonePosition(uintptr_t Entity, int bone) {
    uintptr_t BoneMatrix_Base = *(uintptr_t*)(Entity + m_dwBoneMatrix);
    BoneMatrix_t Bone = *(BoneMatrix_t*)(BoneMatrix_Base + sizeof(Bone) * bone);
    Vec3 Location = { Bone.x, Bone.y, Bone.z };
    Vec2 ScreenCoords;
    float vMatrix[16];
    memcpy(&vMatrix, (PBYTE*)(BaseAddress + dwViewMatrix), sizeof(vMatrix));
    if (WorldTooScreen(Location, ScreenCoords, vMatrix, screenX, screenY)) {

        return ScreenCoords;
    }

    return { 0, 0 };
}

void InitImGui(LPDIRECT3DDEVICE9 pDevice)
{
    ImGui::CreateContext();
    ImGui::myStyleColor();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    io.Fonts->AddFontFromFileTTF(xorstr("C:\\Windows\\Fonts\\Arial.ttf"), 20, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(pDevice);
}

// Функций
bool espbox = false;
bool glow = false;
bool triggerbot = false;
bool bunnyhop = false;
bool antiflash = false;
bool radarhack = false;
bool awpcrosshair = false;
bool thirdperson = false;
bool rcs = false;

// Значения
float rcs_amount = 0;

// Коробка настройка
float boxwidth = 0.5f;
int boxThickness = 2;
int boxAlpha = 255;


bool init = false;
bool menu = true;
bool antialias_all = true;

long __stdcall hkEndScene(LPDIRECT3DDEVICE9 o_pDevice)
{
    if (!pDevice) {
        pDevice = o_pDevice;
    }

    if (!init)
    {
        BaseAddress = (DWORD)GetModuleHandle(xorstr("client.dll"));
        baseEngine = (DWORD)GetModuleHandle(xorstr("engine.dll"));
        InitImGui(pDevice);
        init = true;
    }

    // Function
    if (espbox) {

        uintptr_t localPlayer = *(uintptr_t*)(BaseAddress + dwLocalPlayer);
        if (localPlayer != NULL) {

            int localTeam = *(int*)(localPlayer + m_iTeamNum);

            Vec2 ScreenPosition;
            Vec2 HeadPosition;

            for (int x = 0; x < 32; x++) {

                uintptr_t Entity = *(uintptr_t*)(BaseAddress + dwEntityList + x * 0x10);
                if (Entity != NULL) {

                    int entityTeam = *(int*)(Entity + m_iTeamNum);
                    int entityHealth = *(int*)(Entity + m_iHealth);

                    // Делаем что бы была вражеская команда
                    if (entityTeam != localTeam && entityHealth > 0 && entityHealth <= 100) {

                        Vec3 EntityLocation = *(Vec3*)(Entity + m_vecOrigin);
                        float vMatrix[16];
                        memcpy(&vMatrix, (PBYTE*)(BaseAddress + dwViewMatrix), sizeof(vMatrix));
                        if (WorldTooScreen(EntityLocation, ScreenPosition, vMatrix, screenX, screenY)) {

                            uintptr_t BoneMatrix_Base = *(uintptr_t*)(Entity + m_dwBoneMatrix);
                            BoneMatrix_t Bone = *(BoneMatrix_t*)(BoneMatrix_Base + sizeof(Bone) * 9);
                            Vec3 Location = { Bone.x, Bone.y, Bone.z + 10 };
                            Vec2 ScreenCoords;
                            float vMatrix[16];
                            memcpy(&vMatrix, (PBYTE*)(BaseAddress + dwViewMatrix), sizeof(vMatrix));
                            if (WorldTooScreen(Location, ScreenCoords, vMatrix, screenX, screenY))
                            {

                                HeadPosition = ScreenCoords;
                                DrawBox(
                                    ScreenPosition.x - (((ScreenPosition.y - HeadPosition.y) * boxwidth) / 2),
                                    HeadPosition.y,
                                    (ScreenPosition.y - HeadPosition.y) * boxwidth,
                                    ScreenPosition.y - HeadPosition.y,
                                    boxThickness, antialias_all, D3DCOLOR_ARGB(boxAlpha, BoxColor.r, BoxColor.g, BoxColor.b)

                                );
                            }
                        }
                    }
                }
            }
        }
    }
          
                    



       


    if (glow) {
        int localPlayer = *(int*)(BaseAddress + dwLocalPlayer);
        int GlowObjectManager = *(int*)(BaseAddress + dwGlowObjectManager);

        if (localPlayer != NULL) {

            int localTeam = *(int*)(localPlayer + m_iTeamNum);

            for (int x = 0; x < 32; x++) {
                int Entity = *(int*)(BaseAddress + dwEntityList + x * 0x10);
                if (Entity != NULL) {

                    int entityTeam = *(int*)(Entity + m_iTeamNum);
                    int glowIndex = *(int*)(Entity + m_iGlowIndex);

                    if (entityTeam != localTeam) {

                        *(float*)(GlowObjectManager + glowIndex * 0x38 + 0x4) = 0.f;
                        *(float*)(GlowObjectManager + glowIndex * 0x38 + 0x8) = 1.f;
                        *(float*)(GlowObjectManager + glowIndex * 0x38 + 0xC) = 1.f;
                        *(float*)(GlowObjectManager + glowIndex * 0x38 + 0x10) = 1.9f;
                    }
                    *(bool*)(GlowObjectManager + glowIndex * 0x38 + 0x24) = true;
                    *(bool*)(GlowObjectManager + glowIndex * 0x38 + 0x25) = false;
                }
            }
        }
    }

    if (triggerbot) {

        int localPlayer = *(int*)(BaseAddress + dwLocalPlayer);
        if (localPlayer != NULL) {

            int crosshairID = *(int*)(localPlayer + m_iCrosshairId);
            int localTeam = *(int*)(localPlayer + m_iTeamNum);

            if (crosshairID > 0 && crosshairID < 32) {

                int Entity = *(int*)(BaseAddress + dwEntityList + (crosshairID - 1) * 0x10);
                if (Entity != NULL) {

                    int entityHealth = *(int*)(Entity + m_iHealth);
                    int entityTeam = *(int*)(Entity + m_iTeamNum);

                    if (entityTeam != localTeam && entityHealth > 0 && entityHealth <= 100) {

                        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    }
                }
            }

        }
    }

    if (bunnyhop) {

        int localPlayer = *(int*)(BaseAddress + dwLocalPlayer);
        if (localPlayer != NULL) {

            DWORD flags = *(DWORD*)(localPlayer + m_fFlags);

            if (GetAsyncKeyState(VK_SPACE) && flags & 1 << 0) {

                *(DWORD*)(BaseAddress + dwForceJump) = 6;
            }
        }
    }

    if (antiflash) {

        int localPlayer = *(int*)(BaseAddress + dwLocalPlayer);
        if (localPlayer != NULL) {

            int flashDur = 0;

            flashDur = *(int*)(localPlayer + m_flFlashDuration);
            if (flashDur > 0) {
                *(int*)(localPlayer + m_flFlashDuration) = 0;
            }
        }
    }

    if (radarhack) {
        
        int localPlayer = *(int*)(BaseAddress + dwLocalPlayer);
        if (localPlayer != NULL) {

            int localTeam = *(int*)(localPlayer + m_iTeamNum);

            for (int x = 0; x < 32; x++) {

                int Entity = *(int*)(BaseAddress + dwEntityList + (x - 1) * 0x10);

                if (Entity != NULL) {
                    int entityTeam = *(int*)(Entity + m_iTeamNum);
                    int entityDormant = *(int*)(Entity + m_bDormant);

                    if (entityTeam != localTeam && !entityDormant) {

                        *(int*)(Entity + m_bSpotted) = 1;
                    }
                }
            }
        }
    }

    if (awpcrosshair) {
        DrawFilledRect(screenX / 2 - 2, screenY / 2 - 2, 4, 4, D3DCOLOR_ARGB(255, 255, 255, 255));
    }

    if (thirdperson) {

        int localPlayer = *(int*)(BaseAddress + dwLocalPlayer);
        if (localPlayer != NULL) {

            *(int*)(localPlayer + m_iObserverMode) = 1;

        }
    }

    if (rcs) {

        int localPlayer = *(int*)(BaseAddress + dwLocalPlayer);
        if (localPlayer != NULL) {

            iShotsFired = (int*)(localPlayer + m_iShotsFired);
            aimRecoilPunch = (Vec3*)(localPlayer + m_aimPunchAngle);
            viewAngles = (Vec3*)(*(uintptr_t*)(baseEngine + dwClientState) + dwClientState_ViewAngles);

            Vec3 punchAngle = *aimRecoilPunch * (rcs_amount * 2);
            if (*iShotsFired > 1 && GetAsyncKeyState(VK_LBUTTON)) {
                Vec3 newAngle = *viewAngles + oPunch - punchAngle;
                newAngle.normalize();

                *viewAngles = newAngle;
            }
            oPunch = punchAngle;
        }
    }
    
         

    // Unhook
    if (GetAsyncKeyState(VK_END)) {
        kiero::shutdown();
        return 0;
    }

    // Open Menu
    if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
        menu = !menu;
    }

    if (menu) {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin(xorstr("DarwinTap"));
        ImGui::Checkbox(xorstr(u8"Коробка"), &espbox);
        if (espbox) {
            ImGui::SliderInt("R", &BoxColor.r, 0, 255);
            ImGui::SliderInt("G", &BoxColor.g, 0, 255);
            ImGui::SliderInt("B", &BoxColor.b, 0, 255);
            ImGui::SliderFloat(xorstr(u8"Ширина Коробки"), &boxwidth, 0.00f, 1.00f);
        }
        ImGui::Checkbox(xorstr(u8"Обводка"), &glow);
        ImGui::Checkbox(xorstr(u8"ТриггерБот"), &triggerbot);
        ImGui::Checkbox(xorstr(u8"Баннихоп"), &bunnyhop);
        ImGui::Checkbox(xorstr(u8"Анти Флешка"), &antiflash);
        ImGui::Checkbox(xorstr(u8"Враги на Радаре"), &radarhack);
        ImGui::Checkbox(xorstr(u8"Авп Прицел"), &awpcrosshair);
        ImGui::Checkbox(xorstr(u8"Вид от 3 лица"), &thirdperson);
        ImGui::Spacing();
        ImGui::Checkbox(xorstr(u8"Контроль отдачи"), &rcs);
        if (rcs) {
            ImGui::SliderFloat(xorstr(u8"Значение отдачи"), &rcs_amount, 0, 1);
        }
        ImGui::End();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    return oEndScene(pDevice);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
    DWORD wndProcId;
    GetWindowThreadProcessId(handle, &wndProcId);

    if (GetCurrentProcessId() != wndProcId)
        return TRUE; 

    window = handle;
    return FALSE; 
}

HWND GetProcessWindow()
{
    window = NULL;
    EnumWindows(EnumWindowsCallback, NULL);
    return window;
}


int mainThread()
{
    if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success) {
        
            kiero::bind(42, (void**)&oEndScene, hkEndScene);
            window = GetProcessWindow();
            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);
        }
    return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{

    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)mainThread, NULL, 0, NULL));
    }
    return TRUE;
}