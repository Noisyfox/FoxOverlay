// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <tchar.h>
#include "Overlay.h"

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		Overlay::prepareInstance();

		Overlay::instance()->setModuleHandle(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if (Overlay::instance()->shutdown())
		{
#ifdef _DEBUG
			MessageBox(nullptr, _T("DLL_PROCESS_DETACH"), _T(""), MB_OK);
#endif
		}
		Overlay::freeInstance();
		break;
	}
	return TRUE;
}
