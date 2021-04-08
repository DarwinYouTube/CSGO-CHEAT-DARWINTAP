#define _CRT_SECURE_NO_WARNINGS
#include "includes.h"
#include "color.h"

typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
static EndScene oEndScene = NULL;
LPDIRECT3DDEVICE9 pDevice = nullptr;

WNDPROC oWndProc;
static HWND window = NULL;

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
    io.Fonts->AddFontFromFileTTF(xorstr("C:\\Windows\\Fonts\\Arial.ttf"), 20, NULL, io.Fonts->GetGlyphRangesCyrillic());
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(pDevice);
}

// Функций
bool espbox = false;
bool cornerbox = false;
bool fillbox = false;
bool glow = false;
bool aimbot = false;
bool triggerbot = false;
bool bunnyhop = false;
bool antiflash = false;
bool radarhack = false;
bool awpcrosshair = false;
bool thirdperson = false;
bool fovchanger = false;
bool rcs = false;

// Значения
int aimbotBone = 9;
float aimbotRCS = 1.f;
float aimbotSmoothing = 1.f;
float rcs_amount = 1.f;
int fov = 90;

// Коробка настройка
float boxwidth = 0.5f;
int boxThickness = 2;
float popcorn = 0.5f;

bool init = false;
bool menu = true;
bool antialias_all = true;

Vec3 CalcAngle(Vec3 src, Vec3 dst) {
    float PI = 3.14159f;

    Vec3 angle;
    Vec3 a = { dst.x - src.x, dst.y - src.y, dst.z - src.z };
    float ah = sqrt(a.x * a.x + a.y * a.y);
    angle.x = atan2(-a.z, ah) * 180 / PI;
    angle.y = atan2(a.y, a.x) * 180 / PI;
    angle.z = 0;

    return angle;
}

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

            for (int x = 1; x < 32; x++) {

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
                                    boxThickness, antialias_all, FLOAT4TOD3DCOLOR(Colors::boxColor)
                                );

                                if (fillbox) {
                                    DrawFilledRect(
                                        ScreenPosition.x - (((ScreenPosition.y - HeadPosition.y) * boxwidth) / 2),
                                        HeadPosition.y,
                                        (ScreenPosition.y - HeadPosition.y) * boxwidth,
                                        ScreenPosition.y - HeadPosition.y, D3DCOLOR_ARGB(42, 0, 0, 0));
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (cornerbox) {

        uintptr_t localPlayer = *(uintptr_t*)(BaseAddress + dwLocalPlayer);
        if (localPlayer != NULL) {

            int localTeam = *(int*)(localPlayer + m_iTeamNum);

            Vec2 ScreenPosition;
            Vec2 HeadPosition;

            for (int x = 1; x < 32; x++) {

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
                                CornerBox(
                                    ScreenPosition.x - (((ScreenPosition.y - HeadPosition.y) * boxwidth) / 2),
                                    HeadPosition.y,
                                    (ScreenPosition.y - HeadPosition.y) * boxwidth,
                                    ScreenPosition.y - HeadPosition.y,
                                    boxThickness, popcorn / 2, antialias_all, FLOAT4TOD3DCOLOR(Colors::cornerColor)
                                );

                                if (fillbox) {
                                    DrawFilledRect(
                                        ScreenPosition.x - (((ScreenPosition.y - HeadPosition.y) * boxwidth) / 2),
                                        HeadPosition.y,
                                        (ScreenPosition.y - HeadPosition.y) * boxwidth,
                                        ScreenPosition.y - HeadPosition.y, D3DCOLOR_ARGB(42, 0, 0, 0));
                                }
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

            for (int x = 1; x < 32; x++) {
                int Entity = *(int*)(BaseAddress + dwEntityList + x * 0x10);
                if (Entity != NULL) {

                    int entityTeam = *(int*)(Entity + m_iTeamNum);
                    int glowIndex = *(int*)(Entity + m_iGlowIndex);

                    if (entityTeam != localTeam) {

                        *(float*)(GlowObjectManager + glowIndex * 0x38 + 0x4) = Colors::glowColor[0]; // R
                        *(float*)(GlowObjectManager + glowIndex * 0x38 + 0x8) = Colors::glowColor[1]; // G
                        *(float*)(GlowObjectManager + glowIndex * 0x38 + 0xC) = Colors::glowColor[2]; // B
                        *(float*)(GlowObjectManager + glowIndex * 0x38 + 0x10) = 1.f; // A  
                    }
                    *(bool*)(GlowObjectManager + glowIndex * 0x38 + 0x24) = true;
                    *(bool*)(GlowObjectManager + glowIndex * 0x38 + 0x25) = false;
                }
            }
        }
    }

    if (aimbot) {
        float closesEnt = 99999;
        uintptr_t bestEntity = NULL;

        uintptr_t localPlayer = *(uintptr_t*)(BaseAddress + dwLocalPlayer);
        Vec3* viewAngles = (Vec3*)(*(uintptr_t*)(baseEngine + dwClientState) + dwClientState_ViewAngles);
        if (localPlayer != NULL) {

            for (int i = 1; i < 16; i++)
            {
                uintptr_t Entity = *(uintptr_t*)(BaseAddress + dwEntityList + (i * 0x10));
                if (Entity != NULL) {

                    Vec3 LocalHeadPos;
                    Vec3 EntityBonePos;

                    uintptr_t EBonematrixBase = *(uintptr_t*)(Entity + m_dwBoneMatrix);
                    BoneMatrix_t EntityBone = *(BoneMatrix_t*)(EBonematrixBase + (sizeof(EntityBone) * aimbotBone));
                    EntityBonePos = { EntityBone.x, EntityBone.y, EntityBone.z };

                    uintptr_t LBonematrixBase = *(uintptr_t*)(localPlayer + m_dwBoneMatrix);
                    BoneMatrix_t LocalBone = *(BoneMatrix_t*)(LBonematrixBase + (sizeof(LocalBone) * 9));
                    LocalHeadPos = { LocalBone.x, LocalBone.y, LocalBone.z };

                    Vec3 TempAngles = CalcAngle(LocalHeadPos, EntityBonePos);
                    Vec2 ScreenPosition;
                    float vMatrix[16];
                    memcpy(&vMatrix, (PBYTE*)(BaseAddress + dwViewMatrix), sizeof(vMatrix));
                    if (WorldTooScreen(EntityBonePos, ScreenPosition, vMatrix, screenX, screenY)) {
                        float dist = sqrt(powf((screenX / 2 - ScreenPosition.x), 2) + powf((screenY / 2 - ScreenPosition.y), 2));

                        if (bestEntity == Entity || dist < closesEnt) {

                            int localTeam = *(int*)(localPlayer + m_iTeamNum);
                            int entityTeam = *(int*)(Entity + m_iTeamNum);

                            if (entityTeam != localTeam) {
                                bestEntity = Entity;
                                closesEnt = dist;

                                if (*(int*)(Entity + m_iHealth) > 0) {
                                    Vec3* punchAngleOffs = (Vec3*)(localPlayer + m_aimPunchAngle);
                                    Vec3 punchAngle = *punchAngleOffs * (aimbotRCS * 2);
                                    TempAngles = TempAngles - punchAngle;

                                    Vec3 CurrAngles = *viewAngles;
                                    Vec3 Delta = TempAngles - CurrAngles;
                                    Vec3 AimAngle = CurrAngles + (Delta / (aimbotSmoothing * 50));
                                    AimAngle.normalize();
                                    if (GetAsyncKeyState(VK_LBUTTON) && !GetAsyncKeyState(VK_RBUTTON)) {
                                        *viewAngles = AimAngle;
                                    }
                                }
                            }
                        }
                    }
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

                *(DWORD*)(BaseAddress + dwForceJump) = 5;
                *(DWORD*)(BaseAddress + dwForceJump) = 4;
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

            for (int x = 1; x < 32; x++) {

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
        DrawFilledRect(screenX / 2 - 2, screenY / 2 - 2, 4, 4, FLOAT4TOD3DCOLOR(Colors::crosshairColor));
    }

    if (thirdperson) {

        int localPlayer = *(int*)(BaseAddress + dwLocalPlayer);
        if (localPlayer != NULL) {

            *(int*)(localPlayer + m_iObserverMode) = 1;

        }
    }

    if (fovchanger) {
        
        int localPlayer = *(int*)(BaseAddress + dwLocalPlayer);
        if (localPlayer != NULL) {

            int iFov = *(int*)(localPlayer + m_iDefaultFOV);
            *(int*)(localPlayer + m_iDefaultFOV) = fov;
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
    }

    // Open Menu
    if (GetAsyncKeyState(VK_HOME) & 1) {
        menu = !menu;
    }

    if (menu) {

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        auto flags = ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
        static int ttab;

        ImGui::Begin(xorstr("DarwinTap"), nullptr, flags);

        if (ImGui::Button(xorstr(u8"Аимбот"), ImVec2(100.f, 0.f))) {
            ttab = 1;
        }
        ImGui::SameLine(0.f, 2.f);
        if (ImGui::Button(xorstr(u8"Визуалы"), ImVec2(100.f, 0.f))) {
            ttab = 2;
        }
        ImGui::SameLine(0.f, 2.f);
        if (ImGui::Button(xorstr(u8"Другое"), ImVec2(100.f, 0.f))) {
            ttab = 3;
        }
        ImGui::SameLine(0.f, 2.f);
        if (ImGui::Button(xorstr(u8"Цвета"), ImVec2(100.f, 0.f))) {
            ttab = 4;
        }

        if (ttab == 1) {
            ImGui::Checkbox(xorstr(u8"Аимбот"), &aimbot);
            if (aimbot) {
                ImGui::SliderFloat(xorstr(u8"Аимбот RCS"), &aimbotRCS, 0.f, 1.f);
                ImGui::SliderFloat(xorstr(u8"Аимбот Smoothing"), &aimbotSmoothing, 0.f, 1.f);
                if (ImGui::Button(xorstr(u8"Голова"))) {
                    aimbotBone = 9;
                }
                ImGui::SameLine();
                if (ImGui::Button(xorstr(u8"Шея"))) {
                    aimbotBone = 3;
                }
            }
            ImGui::Checkbox(xorstr(u8"ТриггерБот"), &triggerbot);
            ImGui::Checkbox(xorstr(u8"Контроль отдачи"), &rcs);
        }
        else if (ttab == 2) {
            ImGui::Checkbox(xorstr(u8"Коробка"), &espbox);
            ImGui::Checkbox(xorstr(u8"Угловая Коробка"), &cornerbox);
            ImGui::Checkbox(xorstr(u8"Темная Коробка"), &fillbox);
            ImGui::Checkbox(xorstr(u8"Обводка"), &glow);
            ImGui::Checkbox(xorstr(u8"Прицел"), &awpcrosshair);
        }
        else if (ttab == 3) {
            ImGui::Checkbox(xorstr(u8"Баннихоп"), &bunnyhop);
            ImGui::Checkbox(xorstr(u8"Анти Флешка"), &antiflash);
            ImGui::Checkbox(xorstr(u8"Враги на Радаре"), &radarhack);
            ImGui::Checkbox(xorstr(u8"Вид от 3 лица"), &thirdperson);
            ImGui::Checkbox(xorstr(u8"Изменить FOV"), &fovchanger);
            if (fovchanger) {
                ImGui::SliderInt(xorstr(u8"Значение FOV"), &fov, -180, 180);
            }
            if (ImGui::Button(xorstr(u8"Загрузить Конфиг Дарвина"))) {
                espbox = true;
                glow = true;
                bunnyhop = true;
                radarhack = true;
                antiflash = true;
                rcs = true;
                awpcrosshair = true;
            }
        }
        else if (ttab == 4) {
            ImGui::ColorEdit3(xorstr(u8"Цвет Коробки"), Colors::boxColor);
            ImGui::ColorEdit3(xorstr(u8"Угл.Коробка"), Colors::cornerColor);
            ImGui::ColorEdit3(xorstr(u8"Цвет Обводки"), Colors::glowColor);
            ImGui::ColorEdit3(xorstr(u8"Цвет Прицела"), Colors::crosshairColor);
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