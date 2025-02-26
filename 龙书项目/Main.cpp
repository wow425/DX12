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




HWND mhMainWnd = 0;	//某个窗口的句柄，ShowWindow和UpdateWindow函数均要调用此句柄
GameTime gt;
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// try-catch.try块包含可能抛出异常的代码，目的是执行主程序逻辑。catch块是当try块丢出异常时，自动跳转到相应的catch块，以便捕获异常
// 
// try 块内的代码尝试初始化并运行 Direct3D 12 应用。如果出现问题（例如资源加载失败、设备创建失败等），就会抛出一个 DxException。
// 如果发生 DxException，控制流跳转到 catch 块，在该块中会显示一个包含异常详细信息的消息框，并终止程序。
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
