//包含基本的windows头文件
#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>

//WindowProc 函数原型
LRESULT CALLBACK WindowProc(HWND hWnd,
					   UINT message,
					   WPARAM wParam,
					   LPARAM lParam);

//来自任何窗口程序的空指针
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	//窗口的句柄,由函数注入
	HWND hWnd;
	//这个结构中存放窗口类
	WNDCLASSEX wc;

	//在填充窗口类前将结构内的数值清零
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//将需要的信息注入窗口结构
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = _T("WindowClass1");

	//注册窗口类
	RegisterClassEx(&wc);

	//创建窗口和使用句柄里的数值
	hWnd=CreateWindowEx(0,
					_T("WindowClass1"),//窗口类名
					_T("Hello,Engine!"),//窗口标题
					WS_OVERLAPPEDWINDOW,//窗口风格
					300,//窗口X点
					300,//窗口Y点
					500,//窗口宽度
					400,//窗口高度
					NULL,//我们不需要子窗口,置空值
					NULL,//我们不使用菜单,置空值
					hInstance,//程序句柄
					NULL);//不使用多窗口,置空值

	//在屏幕上显示窗口
	ShowWindow(hWnd, nCmdShow);

	//进入主循环

	//这个结构放入窗口事件消息
	MSG msg;

	//等待队列中的下一个消息，并将结果存入'msg'
	while (GetMessage(&msg,NULL,0,0))
	{
		//转换键盘消息成为正确格式
		TranslateMessage(&msg);

		//发送消息给WindowProc函数
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//用来处理主进程发来的系统消息
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//处理相应系统消息的分支
	switch (message)
	{
		default:
		{
			//如果无需处理任何消息则默认执行该语句
			return DefWindowProc(hWnd, message, wParam, lParam);
		} break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			RECT rec = { 20,20,60,80 };
			HBRUSH brush = (HBRUSH)GetStockBrush(BLACK_BRUSH);

			FillRect(hdc, &rec, brush);

			EndPaint(hWnd, &ps);
		} break;
			//如果消息为摧毁窗口则关闭窗口
		case WM_DESTROY:
		{
			//即将关闭程序
			PostQuitMessage(0);
			return 0;
		} break;
	}
	//如果无需处理任何消息则默认执行该语句,该语句移动至默认分支当中执行↑
	/*return DefWindowProc(hWnd, message, wParam, lParam);*/
}