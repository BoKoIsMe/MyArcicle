//包含基本的windows头文件
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdint.h>

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

const uint32_t SCREEN_WIDTH = 960;
const uint32_t SCREEN_HEIGHT = 480;

//全局声明
IDXGISwapChain			*g_pSwapchain = nullptr;	//指向交换链界面
ID3D11Device			*g_pDev = nullptr;//指向我们的D3D设备界面
ID3D11DeviceContext		*g_pDevcon = nullptr;//指向我们的D3D设备上下文

ID3D11RenderTargetView	*g_pRTView = nullptr;

ID3D11InputLayout		*g_pLayout = nullptr;//指向输入层
ID3D11VertexShader		*g_pVS = nullptr;//指向顶点着色器
ID3D11PixelShader		*g_pPS = nullptr;//指向像素着色器

ID3D11Buffer			*g_pVBuffer = nullptr;//顶点缓冲器

//顶点缓冲区数据结构
struct VERTEX
{
	XMFLOAT3 Position;
	XMFLOAT4 Color;
};

template<class T>
inline void SafeRelease(T **ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != nullptr)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = nullptr;
	}
}

void CreateRenderTarget()
{
	HRESULT hr;
	ID3D11Texture2D *pBackBuffer;

	//取得指向双缓冲的指针
	g_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
  (LPVOID*)&pBackBuffer);

	//创建目标画布预览
	g_pDev->CreateRenderTargetView(pBackBuffer, NULL,
  &g_pRTView);
	pBackBuffer->Release();

	//绑定预览
	g_pDevcon->OMSetRenderTargets(1, &g_pRTView, NULL);
}

void SetViewPort()
{
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;

	g_pDevcon->RSSetViewports(1, &viewport);
}

//这是一个加载和准备着色器的函数
void InitPipeline()
{
	//加载和编译两个着色器
	ID3DBlob *VS, *PS;

	D3DReadFileToBlob(L"copy.vso", &VS);
	D3DReadFileToBlob(L"copy.pso", &PS);

	//封装所有着色器进着色器目标
	g_pDev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVS);
	g_pDev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPS);

	//设定着色器目标
	g_pDevcon->VSSetShader(g_pVS, 0, 0);
	g_pDevcon->PSSetShader(g_pPS, 0, 0);

	//创建输入层目标
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	g_pDev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &g_pLayout);
	g_pDevcon->IASetInputLayout(g_pLayout);

	VS->Release();
	PS->Release();
}

//这个函数用来创建和塑造渲染画布
void InitGraphics()
{
	//用顶点结构创建一个三角形
	VERTEX OurVertices[]=
	{
		{XMFLOAT3(0.0f,0.5f,0.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f)},
		{XMFLOAT3(0.45f,-0.5f,0.0f),XMFLOAT4(0.0f,1.0f,0.0f,1.0f)},
		{XMFLOAT3(-0.45f,-0.5f,0.0f),XMFLOAT4(0.0f,0.0f,1.0f,1.0f)}
	};


	//创建顶点缓冲
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;//CPU与GPU间的写访问
	bd.ByteWidth = sizeof(VERTEX) * 3;//大小是顶点结构的3倍
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;//使用顶点缓存
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//允许CPU写缓存

	g_pDev->CreateBuffer(&bd, NULL, &g_pVBuffer);	//建立缓存

	//从缓存中拷贝顶点
	D3D11_MAPPED_SUBRESOURCE ms;
	g_pDevcon->Map(g_pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);//映射缓存
	memcpy(ms.pData, OurVertices, sizeof(VERTEX) * 3);//拷贝数据
	g_pDevcon->Unmap(g_pVBuffer, NULL);//解除缓存映射
}

//这个函数准备好图形资源备用
HRESULT CreateGraphicsResources(HWND hWnd)
{
	HRESULT hr = S_OK;
	if (g_pSwapchain == nullptr)
	{
		//建立一个数据结构并放入交换链信息
		DXGI_SWAP_CHAIN_DESC scd;

		//清除上一句所声明的数据结构内容
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

		//向数据结构[scd]填充交换链描述
		scd.BufferCount = 1;//一个后台缓存
		scd.BufferDesc.Width = SCREEN_WIDTH;
		scd.BufferDesc.Height = SCREEN_HEIGHT;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//使用32位色彩
		scd.BufferDesc.RefreshRate.Numerator = 60;//刷新率
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//如何使用交换链
		scd.OutputWindow = hWnd;//使用窗口
		scd.SampleDesc.Count = 4;//多少重取样(how many multisamples)
		scd.Windowed = TRUE;//窗口/全屏模式
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//允许全屏切换

		const D3D_FEATURE_LEVEL FeatureLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};
		D3D_FEATURE_LEVEL FeatureLevelSupported;

		HRESULT hr = S_OK;

		//使用在scd结构中的信息创建设备,设备上下文和交换链
		hr = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			0,
			FeatureLevels,
			_countof(FeatureLevels),
			D3D11_SDK_VERSION,
			&scd,
			&g_pSwapchain,
			&g_pDev,
			&FeatureLevelSupported,
			&g_pDevcon);
		if (hr == E_INVALIDARG)
		{
			hr = D3D11CreateDeviceAndSwapChain(NULL,
				D3D_DRIVER_TYPE_HARDWARE,
				NULL,
				0,
				&FeatureLevelSupported,
				1,
				D3D11_SDK_VERSION,
				&scd,
				&g_pSwapchain,
				&g_pDev,
				NULL,
				&g_pDevcon);
		}
		if (hr == S_OK)
		{
			CreateRenderTarget();
			SetViewPort();
			InitPipeline();
			InitGraphics();
		}
	}
	return hr;
}

void DiscardGraphicsResources()
{
	SafeRelease(&g_pLayout);
	SafeRelease(&g_pVS);
	SafeRelease(&g_pPS);
	SafeRelease(&g_pVBuffer);
	SafeRelease(&g_pSwapchain);
	SafeRelease(&g_pRTView);
	SafeRelease(&g_pDev);
	SafeRelease(&g_pDevcon);
}

//这是一个单框架渲染函数
void RenderFrame()
{
	//清除后台缓存为深蓝色
	const FLOAT clearColor[] = {0.0f,0.2f,0.4f,1.0f};
	g_pDevcon->ClearRenderTargetView(g_pRTView, clearColor);

	//将3D渲染放进后台缓存这
	{
		//选择一个顶点缓存显示
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		g_pDevcon->IASetVertexBuffers(0, 1, &g_pVBuffer, &stride, &offset);

		//选择一个我们要用的原生类型
		g_pDevcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//绘制顶点缓存到后台缓存
		g_pDevcon->Draw(3, 0);
	}

	//交换后台缓存和前台缓存
	g_pSwapchain->Present(0, 0);
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

	//在填充窗口类前将结构内的数值清零
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//将需要的信息注入窗口结构
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = _T("WindowClass1");

	//注册窗口类
	RegisterClassEx(&wc);

	//创建窗口和使用句柄里的数值
	hWnd = CreateWindowEx(0,
		_T("WindowClass1"),//窗口类名
		_T("Hello,Engine![Direct 3D]"),//窗口标题
		WS_OVERLAPPEDWINDOW,//窗口风格
		100,//窗口X点
		100,//窗口Y点
		SCREEN_WIDTH,//窗口宽度
		SCREEN_HEIGHT,//窗口高度
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
			wasHandled = true;
		}break;

	case WM_PAINT:
	{
		result = CreateGraphicsResources(hWnd);
		RenderFrame();
	}
		wasHandled = true;
		break;
	case WM_SIZE:
	{
		if (g_pSwapchain != nullptr)
		{
			DiscardGraphicsResources();
			g_pSwapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		}
	}
	wasHandled = true;
	break;

	case WM_DESTROY:
	{
		DiscardGraphicsResources();

		PostQuitMessage(0);
	}
	wasHandled = true;
	break;

	case WM_DISPLAYCHANGE:
	{
		InvalidateRect(hWnd, nullptr, false);
	}
	wasHandled = true;
	break;
	}
	if (!wasHandled) { result = DefWindowProc(hWnd, message, wParam, lParam); }
	return result;
}
