//����������windowsͷ�ļ�
#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>

//WindowProc ����ԭ��
LRESULT CALLBACK WindowProc(HWND hWnd,
					   UINT message,
					   WPARAM wParam,
					   LPARAM lParam);

//�����κδ��ڳ���Ŀ�ָ��
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	//���ڵľ��,�ɺ���ע��
	HWND hWnd;
	//����ṹ�д�Ŵ�����
	WNDCLASSEX wc;

	//����䴰����ǰ���ṹ�ڵ���ֵ����
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//����Ҫ����Ϣע�봰�ڽṹ
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = _T("WindowClass1");

	//ע�ᴰ����
	RegisterClassEx(&wc);

	//�������ں�ʹ�þ�������ֵ
	hWnd=CreateWindowEx(0,
					_T("WindowClass1"),//��������
					_T("Hello,Engine!"),//���ڱ���
					WS_OVERLAPPEDWINDOW,//���ڷ��
					300,//����X��
					300,//����Y��
					500,//���ڿ��
					400,//���ڸ߶�
					NULL,//���ǲ���Ҫ�Ӵ���,�ÿ�ֵ
					NULL,//���ǲ�ʹ�ò˵�,�ÿ�ֵ
					hInstance,//������
					NULL);//��ʹ�öര��,�ÿ�ֵ

	//����Ļ����ʾ����
	ShowWindow(hWnd, nCmdShow);

	//������ѭ��

	//����ṹ���봰���¼���Ϣ
	MSG msg;

	//�ȴ������е���һ����Ϣ�������������'msg'
	while (GetMessage(&msg,NULL,0,0))
	{
		//ת��������Ϣ��Ϊ��ȷ��ʽ
		TranslateMessage(&msg);

		//������Ϣ��WindowProc����
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//�������������̷�����ϵͳ��Ϣ
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//������Ӧϵͳ��Ϣ�ķ�֧
	switch (message)
	{
		//�����ϢΪ�ݻٴ�����رմ���
	case WM_DESTROY:
	{
		//�����رճ���
		PostQuitMessage(0);
		return 0;
	} 
	default:
	{
		//������账���κ���Ϣ��Ĭ��ִ�и����
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
		break;
	}
}