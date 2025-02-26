#pragma once
#include "ToolFunc.h"
#include "GameTime.h"
#include "D:/CS Self-Study/Computer_Graphics/DX12/DX12_DragonBook/Source/Resource/Common/d3dx12.h"
using namespace Microsoft::WRL;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class D3D12App
{
protected:
	D3D12App() = default;
	virtual ~D3D12App() = default;
public:
	int Run();
	bool Init(HINSTANCE hInstance, int nShowCmd);
	bool InitWindow(HINSTANCE hInstance, int nShowCmd);
	bool InitDirect3D();
	virtual void Draw();
	// 创建设备->创建围栏->设置抗锯齿->创建命令队列->创建交换链->获取描述符大小->创建描述符堆->创建描述符RSV->设置围栏并刷新命令队列->设置视口和裁剪矩阵->将命令从列表传至队列
	void CreateDevice();// 创建设备
	void CreateFence();// 创建围栏
	void GetDescriptorSize();// 获取描述符大小
	void SetMSAA();// 设置MSAA
	void CreateCommandObject();// 创建命令队列，列表，分配器
	void CreateSwapChain();// 创建交换链
	void CreateDescriptorHeap();// 创建描述符堆
	void CreateRTV();// 创建描述符
	void CreateDSV();
	void resourceBarrierBuild();// 设置围栏刷新命令队列。
	void CreateViewPortAndScissorRect(); // 设置视口和裁剪矩形。
	void FlushCmdQueue();// 将命令从列表传至队列。
	void CalculateFrameState(); // 计算帧数
protected:
	HWND mhMainWnd = 0; //某个窗口的句柄，ShowWindow和UpdateWindow函数均要调用此句柄
//窗口过程函数的声明,HWND是主窗口句柄

	//指针接口和变量声明
	ComPtr<ID3D12Device> d3dDevice; 
	ComPtr<IDXGIFactory4> dxgiFactory;
	ComPtr<ID3D12Fence> fence;
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12Resource> depthStencilBuffer;
	ComPtr<ID3D12Resource> swapChainBuffer[2];
	ComPtr<IDXGISwapChain> swapChain;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;

	D3D12_VIEWPORT viewPort;
	D3D12_RECT scissorRect;

	UINT rtvDescriptorSize = 0;
	UINT dsvDescriptorSize = 0;
	UINT cbv_srv_uavDescriptorSize = 0;

	//GameTime类实例声明
	GameTime gt;

	UINT mCurrentBackBuffer = 0;
};