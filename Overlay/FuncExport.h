// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� OVERLAY_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ���Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// OVERLAY_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef OVERLAY_EXPORTS
#define OVERLAY_API __declspec(dllexport)
#else
#define OVERLAY_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

OVERLAY_API bool InstallHook(DWORD dwProcId, DWORD dwThreadId);
OVERLAY_API LRESULT CALLBACK HookProc(int, WPARAM, LPARAM);

#ifdef __cplusplus
}
#endif
