#include "graphic_util.h"

HWND CreateStaticWindow(const LPCTSTR lpWindowName)
{
	return CreateWindow(_T("STATIC"), lpWindowName, WS_POPUP, 0, 0, 2, 2, HWND_MESSAGE, nullptr, nullptr, nullptr);
}
