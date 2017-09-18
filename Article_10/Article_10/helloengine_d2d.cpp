//����������windowsͷ�ļ�
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


//WindowProc ����ԭ��
LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

//���������κδ��ڳ���Ŀ�ָ��
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	//���ڵľ��,�ɺ���ע��
	HWND hWnd;
	//����ṹ�д�Ŵ�����
	WNDCLASSEX wc;

	//��ʼ��COM
	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) return -1;

	//����䴰����ǰ���ṹ�ڵ���ֵ����
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//����Ҫ����Ϣע�봰�ڽṹ
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	//wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = _T("WindowClass1");

	//ע�ᴰ����
	RegisterClassEx(&wc);

	//�������ں�ʹ�þ�������ֵ
	hWnd = CreateWindowEx(0,
		_T("WindowClass1"),//��������
		//_T("Hello,Engine!"),//���ڱ���
		_T("Hello,Engine![Direct 2D]"),//���ڱ���
		WS_OVERLAPPEDWINDOW,//���ڷ��
		100,//����X��
		100,//����Y��
		960,//���ڿ���
		540,//���ڸ߶�
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
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		//ת��������Ϣ��Ϊ��ȷ��ʽ
		TranslateMessage(&msg);

		//������Ϣ��WindowProc����
		DispatchMessage(&msg);
	}
	//����ʼ��COM
	CoUninitialize();

	//����WM_QUIT��Ϣ������
	return msg.wParam;
}

//�������������̷�����ϵͳ��Ϣ
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	bool wasHandled = false;

	//������Ӧϵͳ��Ϣ�ķ�֧
	switch (message)
	{
	case WM_CREATE:
		{
			if (FAILED(D2D1CreateFactory(
				D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
			{
				result = -1;//CreateWindowEx��������ʧ��
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

				//��ʼ����GPU��������
				pRenderTarget->BeginDraw();

				//����ڱ����е���ɫ
				pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

				//ȡ�ػ�������ߴ�
				D2D1_SIZE_F rtSize = pRenderTarget->GetSize();

				//���Ʊ�����������
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

			//������������
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

			//���Ʋ�������
			pRenderTarget->FillRectangle(&rectangle1, pLightSlateGrayBrush);

			//���ƾ���������
			pRenderTarget->DrawRectangle(&rectangle2, pCornflowerBlueBrush);

			//����GPU���������
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
		//������账���κ���Ϣ��Ĭ��ִ�и����
		//return DefWindowProc(hWnd, message, wParam, lParam);//����ע�͵���������һ�д���
		if (!wasHandled) { result = DefWindowProc(hWnd, message, wParam, lParam); }
		return result;
	} break;
	}
	if (!wasHandled) { result = DefWindowProc(hWnd, message, wParam, lParam); }
	return result;
}