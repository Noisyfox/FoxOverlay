#include <Windows.h>
#include "../Overlay/FuncExport.h"
#include <tchar.h>
#include <cstdio>


int main(int argc, char* argv[])
{
	printf("Enter process id: ");
	DWORD dwProcId;
	scanf_s("%lu", &dwProcId);

	InstallHook(dwProcId, 0);
	MessageBox(nullptr, _T("Hooked"), _T(""), MB_OK);

	return 0;
}
