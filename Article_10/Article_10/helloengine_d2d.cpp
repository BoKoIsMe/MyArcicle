//包含基本的windows头文件
#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>

#include <d2d1.h>

ID2D1Factory			*pFactory = nullptr;
ID2D1HwndRenderTarget	*pRenderTarget = nullptr;
ID2D1SolidColorBrush	*pLightSlateGrayBrush = nullptr;
ID2D1SolidColorBrush	*pCornflowerBlueBrush = nullptr;

template<class T>
inline void SafeRelease(T **ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != nullptr)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = nullptr;
	}
}

HRESULT CreateGraphicsResources(HWND hWnd)
{
	HRESULT hr = S_OK;
	if (pRenderTarget == nullptr)
	{
		RECT rc;
		GetClientRect(hWnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hWnd, size),
			&pRenderTarget);
		if (SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSlateGray), &pLightSlateGrayBrush);

		}

		if (SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &pCornflowerBlueBrush);

		}
	}
	return hr;
}

void DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pLightSlateGrayBrush);
	SafeRelease(&pCornflowerBlueBrush);
}


//WindowProc 函数原型
LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

//接收来自任何窗口程序的空指针
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	//窗口的句柄,由函数注入
	HWND hWnd;
	//这个结构中存放窗口类
	WNDCLASSEX wc;

	//初始化COM
	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) return -1;

	//在填充窗口类前将结构内的数值清零
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//将需要的信息注入窗口结构
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	//wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = _T("WindowClass1");

	//注册窗口类
	RegisterClassEx(&wc);

	//创建窗口和使用句柄里的数值
	hWnd = CreateWindowEx(0,
		_T("WindowClass1"),//窗口类名
		//_T("Hello,Engine!"),//窗口标题
		_T("Hello,Engine![Direct 2D]"),//窗口标题
		WS_OVERLAPPEDWINDOW,//窗口风格
		100,//窗口X点
		100,//窗口Y点
		960,//窗口宽度
		540,//窗口高度
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
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		//转换键盘消息成为正确格式
		TranslateMessage(&msg);

		//发送消息给WindowProc函数
		DispatchMessage(&msg);
	}
	//反初始化COM
	CoUninitialize();

	//发送WM_QUIT消息给窗口
	return msg.wParam;
}

//用来处理主进程发来的系统消息
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	bool wasHandled = false;

	//处理相应系统消息的分支
	switch (message)
	{
	case WM_CREATE:
		{
			if (FAILED(D2D1CreateFactory(
				D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
			{
				result = -1;//CreateWindowEx函数调用失败
			}
			wasHandled = true;
			result = 1;
		}break;

	case WM_PAINT:
		{
			HRESULT hr = CreateGraphicsResources(hWnd);
			if (SUCCEEDED(hr))
			{
				PAINTSTRUCT ps;
				BeginPaint(hWnd, &ps);

				//开始建立GPU绘制命令
				pRenderTarget->BeginDraw();

				//清除在背景中的颜色
				pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

				//取回绘制区域尺寸
				D2D1_SIZE_F rtSize = pRenderTarget->GetSize();

				//绘制背景方格阵列
				int width = static_cast<int>(rtSize.width);
				int height = static_cast<int>(rtSize.height);

				for (int x = 0; x < width; x += 10)
				{
					pRenderTarget->DrawLine(
						D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
						D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
						pLightSlateGrayBrush,
						0.5f
					);
				}

			for (int y = 0; y < height; y += 10)
			{
				pRenderTarget->DrawLine(
					D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
					D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
					pLightSlateGrayBrush,
					0.5f
				);
			}

			//绘制两个矩形
			D2D1_RECT_F rectangle1 = D2D1::RectF(
				rtSize.width / 2 - 50.0f,
				rtSize.height / 2 - 50.0f,
				rtSize.width / 2 + 50.0f,
				rtSize.height / 2 + 50.0f
			);

			D2D1_RECT_F rectangle2 = D2D1::RectF(
				rtSize.width / 2 - 100.0f,
				rtSize.height / 2 - 100.0f,
				rtSize.width / 2 + 100.0f,
				rtSize.height / 2 + 100.0f
			);

			//绘制并填充矩形
			pRenderTarget->FillRectangle(&rectangle1, pLightSlateGrayBrush);

			//绘制矩形轮廓线
			pRenderTarget->DrawRectangle(&rectangle2, pCornflowerBlueBrush);

			//结束GPU绘制命令构建
			hr = pRenderTarget->EndDraw();
			if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
			{
				DiscardGraphicsResources();
			}
			EndPaint(hWnd, &ps);
		}
		} 
		wasHandled = true;
		break;
	case WM_SIZE:
	{
		if (pRenderTarget != nullptr)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

			pRenderTarget->Resize(size);
		}
	}
	wasHandled = true;
	break;

	case WM_DESTROY:
	{
		DiscardGraphicsResources();
		if (pFactory) { pFactory->Release(); pFactory = nullptr; }

		PostQuitMessage(0);
		result = 1;
	}
	wasHandled = true;
	break;

	case WM_DISPLAYCHANGE:
	{
		InvalidateRect(hWnd, nullptr, false);
	}
	wasHandled = true;
	break;
	default:
	{
		//如果无需处理任何消息则默认执行该语句
		//return DefWindowProc(hWnd, message, wParam, lParam);//这行注释掉，用下面一行代替
		if (!wasHandled) { result = DefWindowProc(hWnd, message, wParam, lParam); }
		return result;
	} break;
	}
	if (!wasHandled) { result = DefWindowProc(hWnd, message, wParam, lParam); }
	return result;
}
