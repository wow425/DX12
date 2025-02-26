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
	// �����豸->����Χ��->���ÿ����->�����������->����������->��ȡ��������С->������������->����������RSV->����Χ����ˢ���������->�����ӿںͲü�����->��������б�������
	void CreateDevice();// �����豸
	void CreateFence();// ����Χ��
	void GetDescriptorSize();// ��ȡ��������С
	void SetMSAA();// ����MSAA
	void CreateCommandObject();// ����������У��б�������
	void CreateSwapChain();// ����������
	void CreateDescriptorHeap();// ������������
	void CreateRTV();// ����������
	void CreateDSV();
	void resourceBarrierBuild();// ����Χ��ˢ��������С�
	void CreateViewPortAndScissorRect(); // �����ӿںͲü����Ρ�
	void FlushCmdQueue();// ��������б������С�
	void CalculateFrameState(); // ����֡��
protected:
	HWND mhMainWnd = 0; //ĳ�����ڵľ����ShowWindow��UpdateWindow������Ҫ���ô˾��
//���ڹ��̺���������,HWND�������ھ��

	//ָ��ӿںͱ�������
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

	//GameTime��ʵ������
	GameTime gt;

	UINT mCurrentBackBuffer = 0;
};