#include <Windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <windowsx.h>
#include "D:/CS Self-Study/Computer_Graphics/DX12/DX12_DragonBook/Source/Resource/Common/d3dx12.h"
#include <comdef.h>
#include "Init/D3D12App.h"
#include "Init/GameTime.h"
#include "Init/ToolFunc.h"
#include "D3D12InitApp.h"
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")




HWND mhMainWnd = 0;	//ĳ�����ڵľ����ShowWindow��UpdateWindow������Ҫ���ô˾��
GameTime gt;
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// try-catch.try����������׳��쳣�Ĵ��룬Ŀ����ִ���������߼���catch���ǵ�try�鶪���쳣ʱ���Զ���ת����Ӧ��catch�飬�Ա㲶���쳣
// 
// try ���ڵĴ��볢�Գ�ʼ�������� Direct3D 12 Ӧ�á�����������⣨������Դ����ʧ�ܡ��豸����ʧ�ܵȣ����ͻ��׳�һ�� DxException��
// ������� DxException����������ת�� catch �飬�ڸÿ��л���ʾһ�������쳣��ϸ��Ϣ����Ϣ�򣬲���ֹ����
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	try
	{
		D3D12InitApp theApp;
		if (!theApp.Init(hInstance, nShowCmd))
			return 0;

		return theApp.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}
