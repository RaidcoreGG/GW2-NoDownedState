#pragma comment(lib, "winmm.lib")
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>
#include <D3D9.h>
#include "arcdps_structs.h"
#include "imgui\imgui.h"
#include <tchar.h>

/* proto/globals */
arcdps_exports arc_exports;
char* arcvers;
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversionstr, void* imguicontext, IDirect3DDevice9* id3dd9);
extern "C" __declspec(dllexport) void* get_release_addr();
arcdps_exports* mod_init();
uintptr_t mod_release();
uintptr_t mod_wnd(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
uintptr_t mod_combat(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision);
uintptr_t mod_options(); /* id3dd9::present callback, appending to the end of options window in arcdps, fn() */

bool enable_nodownedstate = false;
HWND gw2Handle;

/* export -- arcdps looks for this exported function and calls the address it returns on client load */
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversionstr, void* imguicontext, IDirect3DDevice9* id3dd9)
{
	arcvers = arcversionstr;
	ImGui::SetCurrentContext((ImGuiContext*)imguicontext);
	return mod_init;
}

/* export -- arcdps looks for this exported function and calls the address it returns on client exit */
extern "C" __declspec(dllexport) void* get_release_addr()
{
	arcvers = 0;
	return mod_release;
}

/* initialize mod -- return table that arcdps will use for callbacks */
arcdps_exports* mod_init()
{
	/* demo */
	//AllocConsole();

	/* big buffer */
	//char buff[4096];
	//char* p = &buff[0];
	//p += _snprintf(p, 400, "==== mod_init ====\n");
	//p += _snprintf(p, 400, "arcdps: %s\n", arcvers);

	/* print */
	//DWORD written = 0;
	//HANDLE hnd = GetStdHandle(STD_OUTPUT_HANDLE);
	//WriteConsoleA(hnd, &buff[0], (DWORD)(p - &buff[0]), &written, 0);

	/* for arcdps */
	memset(&arc_exports, 0, sizeof(arcdps_exports));
	arc_exports.sig = 0x2241D010;
	arc_exports.size = sizeof(arcdps_exports);
	arc_exports.out_name = "No Downed State (by DeltaxHunter)";
	arc_exports.out_build = "2020-10-05";
	arc_exports.wnd_nofilter = mod_wnd;
	arc_exports.combat_local = mod_combat;
	arc_exports.options_end = mod_options;
	//arc_exports.size = (uintptr_t)"error message if you decide to not load, sig must be 0";

	return &arc_exports;
}

/* release mod -- return ignored */
uintptr_t mod_release()
{
	//FreeConsole();
	return 0;
}

/* window callback -- return is assigned to umsg (return zero to not be processed by arcdps or game) */
uintptr_t mod_wnd(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	gw2Handle = hWnd;
	return uMsg;
}

uintptr_t mod_options()
{
	ImGui::Checkbox("No Downed State", &enable_nodownedstate);
	return 0;
}

/* combat callback -- may be called asynchronously. return ignored */
/* one participant will be party/squad, or minion of. no spawn statechange events. despawn statechange only on marked boss npcs */
uintptr_t mod_combat(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision)
{
	/* big buffer */
	//char buff[4096];
	//char* p = &buff[0];

	//p += _snprintf(p, 400, "-id: %u\n", id);
	//p += _snprintf(p, 400, "-revision: %u\n", revision);

	if (enable_nodownedstate)
	{
		if (ev) {

			if (dst->self == 1)
			{
				if(ev->skillid == 770)
				{
					INPUT ip;

					ip.type = INPUT_KEYBOARD;
					ip.ki.time = 0;
					ip.ki.wVk = 0;
					ip.ki.dwExtraInfo = 0;

					ip.ki.wScan = 0x1c;

					ip.ki.dwFlags = KEYEVENTF_SCANCODE;
					SendInput(1, &ip, sizeof(INPUT));

					Sleep(5);

					keybd_event(VK_DIVIDE, 0x8f, 0, 0);
					keybd_event(VK_DIVIDE, 0x8f, KEYEVENTF_KEYUP, 0);
					keybd_event(0x47, 0x8f, 0, 0);
					keybd_event(0x47, 0x8f, KEYEVENTF_KEYUP, 0);
					keybd_event(0x47, 0x8f, 0, 0);
					keybd_event(0x47, 0x8f, KEYEVENTF_KEYUP, 0);

					Sleep(5);

					ip.ki.dwFlags = KEYEVENTF_SCANCODE;
					SendInput(1, &ip, sizeof(INPUT));

					ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
					SendInput(1, &ip, sizeof(INPUT));
				}
			}
		}
	}

	/* print */
	//DWORD written = 0;
	//HANDLE hnd = GetStdHandle(STD_OUTPUT_HANDLE);
	//WriteConsoleA(hnd, &buff[0], (DWORD)(p - &buff[0]), &written, 0);
	return 0;
}