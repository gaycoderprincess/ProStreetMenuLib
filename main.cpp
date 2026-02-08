#include <windows.h>
#include <d3d9.h>
#include <mutex>
#include <toml++/toml.hpp>

#include "nya_dx9_hookbase.h"
#include "nya_commonhooklib.h"
#include "nfsps.h"

#include "include/chloemenulib.h"

void DisableKeyboardInput(bool disable) {
	NyaHooks::InputBlockerHook::bInputsBlocked = disable;
}

void UpdateD3DProperties() {
	auto& gD3DDevice = *(IDirect3DDevice9***)0xAC6ED4;

	g_pd3dDevice = *gD3DDevice;
	ghWnd = *(HWND*)0xAC6ED8;

	// todo this can prolly be done better
	RECT rect;
	GetWindowRect(ghWnd, &rect);
	nResX = rect.right - rect.left;
	nResY = rect.bottom - rect.top;
}

const char* versionString = "NFS ProStreet Menu Lib 1.00";

#include "menulib.h"

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			if (NyaHookLib::GetEntryPoint() != 0x16AA080 && NyaHookLib::GetEntryPoint() != 0x428C25) {
				MessageBoxA(nullptr, "Unsupported game version! Make sure you're using v1.1 (.exe size of 3765248 or 28739656 bytes)", "nya?!~", MB_ICONERROR);
				return TRUE;
			}

			if (!InitAndLoadConfig("NFSPSMenuLib_gcp.toml")) return TRUE;

			NyaHooks::PlaceD3DHooks();
			NyaHooks::D3DEndSceneHook::aFunctions.push_back(D3DHookMain);
			NyaHooks::D3DResetHook::aFunctions.push_back(OnD3DReset);
			NyaHooks::WndProcHook::Init();
			NyaHooks::WndProcHook::aFunctions.push_back(WndProcHook);
			NyaHooks::InputBlockerHook::Init();
		} break;
		default:
			break;
	}
	return TRUE;
}