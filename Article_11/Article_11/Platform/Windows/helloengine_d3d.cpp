//����������windowsͷ�ļ�
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

//ȫ������
IDXGISwapChain			*g_pSwapchain = nullptr;	//ָ�򽻻�������
ID3D11Device			*g_pDev = nullptr;//ָ�����ǵ�D3D�豸����
ID3D11DeviceContext		*g_pDevcon = nullptr;//ָ�����ǵ�D3D�豸������

ID3D11RenderTargetView	*g_pRTView = nullptr;

ID3D11InputLayout		*g_pLayout = nullptr;//ָ�������
ID3D11VertexShader		*g_pVS = nullptr;//ָ�򶥵���ɫ��
ID3D11PixelShader		*g_pPS = nullptr;//ָ��������ɫ��

ID3D11Buffer			*g_pVBuffer = nullptr;//���㻺����

//���㻺�������ݽṹ
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

	//ȡ��ָ��˫�����ָ��
	g_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
  (LPVOID*)&pBackBuffer);

	//����Ŀ�껭��Ԥ��
	g_pDev->CreateRenderTargetView(pBackBuffer, NULL,
  &g_pRTView);
	pBackBuffer->Release();

	//��Ԥ��
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

//����һ�����غ�׼����ɫ���ĺ���
void InitPipeline()
{
	//���غͱ���������ɫ��
	ID3DBlob *VS, *PS;

	D3DReadFileToBlob(L"copy.vso", &VS);
	D3DReadFileToBlob(L"copy.pso", &PS);

	//��װ������ɫ������ɫ��Ŀ��
	g_pDev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVS);
	g_pDev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPS);

	//�趨��ɫ��Ŀ��
	g_pDevcon->VSSetShader(g_pVS, 0, 0);
	g_pDevcon->PSSetShader(g_pPS, 0, 0);

	//���������Ŀ��
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

//�����������������������Ⱦ����
void InitGraphics()
{
	//�ö���ṹ����һ��������
	VERTEX OurVertices[]=
	{
		{XMFLOAT3(0.0f,0.5f,0.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f)},
		{XMFLOAT3(0.45f,-0.5f,0.0f),XMFLOAT4(0.0f,1.0f,0.0f,1.0f)},
		{XMFLOAT3(-0.45f,-0.5f,0.0f),XMFLOAT4(0.0f,0.0f,1.0f,1.0f)}
	};


	//�������㻺��
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;//CPU��GPU���д����
	bd.ByteWidth = sizeof(VERTEX) * 3;//��С�Ƕ���ṹ��3��
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;//ʹ�ö��㻺��
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//����CPUд����

	g_pDev->CreateBuffer(&bd, NULL, &g_pVBuffer);	//��������

	//�ӻ����п�������
	D3D11_MAPPED_SUBRESOURCE ms;
	g_pDevcon->Map(g_pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);//ӳ�仺��
	memcpy(ms.pData, OurVertices, sizeof(VERTEX) * 3);//��������
	g_pDevcon->Unmap(g_pVBuffer, NULL);//�������ӳ��
}

//�������׼����ͼ����Դ����
HRESULT CreateGraphicsResources(HWND hWnd)
{
	HRESULT hr = S_OK;
	if (g_pSwapchain == nullptr)
	{
		//����һ�����ݽṹ�����뽻������Ϣ
		DXGI_SWAP_CHAIN_DESC scd;

		//�����һ�������������ݽṹ����
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

		//�����ݽṹ[scd]��佻��������
		scd.BufferCount = 1;//һ����̨����
		scd.BufferDesc.Width = SCREEN_WIDTH;
		scd.BufferDesc.Height = SCREEN_HEIGHT;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//ʹ��32λɫ��
		scd.BufferDesc.RefreshRate.Numerator = 60;//ˢ����
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//���ʹ�ý�����
		scd.OutputWindow = hWnd;//ʹ�ô���
		scd.SampleDesc.Count = 4;//������ȡ��(how many multisamples)
		scd.Windowed = TRUE;//����/ȫ��ģʽ
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//����ȫ���л�

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

		//ʹ����scd�ṹ�е���Ϣ�����豸,�豸�����ĺͽ�����
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

//����һ���������Ⱦ����
void RenderFrame()
{
	//�����̨����Ϊ����ɫ
	const FLOAT clearColor[] = {0.0f,0.2f,0.4f,1.0f};
	g_pDevcon->ClearRenderTargetView(g_pRTView, clearColor);

	//��3D��Ⱦ�Ž���̨������
	{
		//ѡ��һ�����㻺����ʾ
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		g_pDevcon->IASetVertexBuffers(0, 1, &g_pVBuffer, &stride, &offset);

		//ѡ��һ������Ҫ�õ�ԭ������
		g_pDevcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//���ƶ��㻺�浽��̨����
		g_pDevcon->Draw(3, 0);
	}

	//������̨�����ǰ̨����
	g_pSwapchain->Present(0, 0);
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

	//����䴰����ǰ���ṹ�ڵ���ֵ����
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//����Ҫ����Ϣע�봰�ڽṹ
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = _T("WindowClass1");

	//ע�ᴰ����
	RegisterClassEx(&wc);

	//�������ں�ʹ�þ�������ֵ
	hWnd = CreateWindowEx(0,
		_T("WindowClass1"),//��������
		_T("Hello,Engine![Direct 3D]"),//���ڱ���
		WS_OVERLAPPEDWINDOW,//���ڷ��
		100,//����X��
		100,//����Y��
		SCREEN_WIDTH,//���ڿ��
		SCREEN_HEIGHT,//���ڸ߶�
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
