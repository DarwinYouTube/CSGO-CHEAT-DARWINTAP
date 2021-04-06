#include <Windows.h>
#include <thread>
#include <chrono>
#include <ctime>
#include <math.h>

#include <d3d9.h>
#include <d3dx9.h>
#include "drawing.h"

#include "kiero.h"
#include "xorstr.h"
#include "offsets.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "vector.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern LPDIRECT3DDEVICE9 pDevice;