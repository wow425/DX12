#include "D3D12App.h"
#include "GameTime.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// ��Ϣ����
	switch (msg) {
		// �����ڱ�����ʱ����ֹ��Ϣѭ��
	case WM_DESTROY:
		PostQuitMessage(0);	// ��ֹ��Ϣѭ����������WM_QUIT��Ϣ
		return 0;
	default:
		break;
	}
	// ������û�д������Ϣת����Ĭ�ϵĴ��ڹ���
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool D3D12App::InitWindow(HINSTANCE hInstance, int nShowCmd)
{
	//���ڳ�ʼ�������ṹ��(WNDCLASS)
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;	// ����������߸ı䣬�����»��ƴ���
	wc.lpfnWndProc = MainWndProc;	//ָ�����ڹ���
	wc.cbClsExtra = 0;	// �����������ֶ���Ϊ��ǰӦ�÷��������ڴ�ռ䣨���ﲻ���䣬������0��
	wc.cbWndExtra = 0;	// �����������ֶ���Ϊ��ǰӦ�÷��������ڴ�ռ䣨���ﲻ���䣬������0��
	wc.hInstance = hInstance;	// Ӧ�ó���ʵ���������WinMain���룩
	wc.hIcon = LoadIcon(0, IDC_ARROW);	// ʹ��Ĭ�ϵ�Ӧ�ó���ͼ��
	wc.hCursor = LoadCursor(0, IDC_ARROW);	// ʹ�ñ�׼�����ָ����ʽ
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// ָ���˰�ɫ������ˢ���
	wc.lpszMenuName = 0;	// û�в˵���
	wc.lpszClassName = L"MainWnd";	// ������
	//������ע��ʧ��
	if (!RegisterClass(&wc)) {
		//��Ϣ����������1����Ϣ���������ھ������ΪNULL������2����Ϣ����ʾ���ı���Ϣ������3�������ı�������4����Ϣ����ʽ
		MessageBox(0, L"RegisterClass Failed", 0, 0);
		return 0;
	}
	RECT R;	// �ü�����
	R.left = 0;
	R.top = 0;
	R.right = 1280;
	R.bottom = 720;
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);	//���ݴ��ڵĿͻ�����С���㴰�ڵĴ�С
	int width = R.right - R.left;
	int hight = R.bottom - R.top;

	// ��������,���ز���ֵ
	// CreateWindowW(lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
	mhMainWnd = CreateWindow(L"MainWnd", L"DX12Initialize", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, hight, 0, 0, hInstance, 0);
	// ���ڴ���ʧ��
	if (!mhMainWnd) {
		MessageBox(0, L"CreatWindow Failed", 0, 0);
		return 0;
	}
	// ���ڴ����ɹ�,����ʾ�����´���
	ShowWindow(mhMainWnd, nShowCmd);
	UpdateWindow(mhMainWnd);

	return true;
}
int D3D12App::Run()
{
	//��Ϣѭ��
	//������Ϣ�ṹ��
	MSG msg = { 0 };
	//ÿ��ѭ����ʼ��Ҫ���ü�ʱ��
	gt.Reset();
	//���GetMessage����������0��˵��û�н��ܵ�WM_QUIT
	while (msg.message != WM_QUIT)
	{
		//����д�����Ϣ�ͽ��д���
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))//PeekMessage�������Զ����msg�ṹ��Ԫ��
		{
			TranslateMessage(&msg);	//���̰���ת�������������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg);	//����Ϣ���ɸ���Ӧ�Ĵ��ڹ���
		}
		//�����ִ�ж�������Ϸ�߼�
		else
		{
			gt.Tick();	//����ÿ��֡���ʱ��
			if (!gt.IsStoped())//���������ͣ״̬�����ǲ�������Ϸ
			{
				CalculateFrameState();
				Draw();
			}
			//�������ͣ״̬��������100��
			else
			{
				Sleep(100);
			}

		}
	}
	return (int)msg.wParam;
}
// �����յ�����Ϣ���ɸ����ڹ���
// ���ڹ��̺���

ComPtr<ID3D12Device> d3dDevice;
ComPtr<IDXGIFactory4> dxgiFactory;
// �����豸
void D3D12App::CreateDevice() {
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory))); // IID_PPV_ARGS������ӿ�ת��Ϊ��ʽ��IID��void**
	ThrowIfFailed(D3D12CreateDevice(nullptr, // ѡ���ĸ���������nullptrĬ��Ϊ��������
		D3D_FEATURE_LEVEL_12_0,		// Ӧ�ó���֧�ֵ���͹��ܼ���
		IID_PPV_ARGS(&d3dDevice)));	// �����뵽�Ľӿ�(ָ��)���ظ�d3dDevice
}
// ��������ͨ���豸����Χ��fence���Ա�֮��ͬ��CPU��GPU��
ComPtr<ID3D12Fence> fence;
void D3D12App::CreateFence() {
	ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
	// ����1ΪΧ����ʼֵ. ����2ΪΧ���Ĵ�����־. ����3Ϊ�����뵽�Ľӿ�(ָ��)���ظ�fence
}
D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaQualityLevels;
// ����MSAA��������ԡ�
// ע�⣺�˴���ʹ��MSAA��������������Ϊ1��������������
void D3D12App::SetMSAA() {
	msaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// UNORM�ǹ�һ��������޷�������
	msaaQualityLevels.SampleCount = 1;
	msaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;// û���κ�ѡ��֧��
	msaaQualityLevels.NumQualityLevels = 0;
	// ��ǰͼ��������MSAA���ز�����֧�֣�ע�⣺�ڶ������������������������
	ThrowIfFailed(d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaaQualityLevels, sizeof(msaaQualityLevels)));
	// NumQualityLevels��Check��������������
	// ���֧��MSAA����Check�������ص�NumQualityLevels > 0
	// expressionΪ�٣���Ϊ0��������ֹ�������У�����ӡһ��������Ϣ
	assert(msaaQualityLevels.NumQualityLevels > 0);
}
// ����������С������б�������������
// ע�⣬�����ڳ�ʼ��D3D12_COMMAND_QUEUE_DESCʱ��
// ֻ��ʼ��������������������ڴ�������Ĭ�ϳ�ʼ���ˡ�
D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};// ��������
ComPtr<ID3D12CommandQueue> cmdQueue;// ������е�COM
ComPtr<ID3D12CommandAllocator> cmdAllocator;// �����������COM 
ComPtr<ID3D12GraphicsCommandList> cmdList;// �����б��COM
void D3D12App::CreateCommandObject() {
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;// ָ��GPU����ִ�е����������ֱ�������б�δ�̳��κ�GPU״̬
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;// Ĭ���������
	ThrowIfFailed(d3dDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&cmdQueue)));// �����������
	ThrowIfFailed(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator)));// ������������� &cmdAllocator�ȼ���cmdAllocator.GetAddressOf
	// ���������б�
	ThrowIfFailed(d3dDevice->CreateCommandList(0, // ����ֵΪ0����GPU
		D3D12_COMMAND_LIST_TYPE_DIRECT, // �����б�����
		cmdAllocator.Get(),	// ����������ӿ�ָ��
		nullptr,	// ��ˮ��״̬����PSO�����ﲻ���ƣ����Կ�ָ��
		IID_PPV_ARGS(&cmdList)));	// ���ش����������б�
	cmdList->Close();	// ���������б�ǰ���뽫��ر�,��ȷ�������б��е����������ȷ��¼
}
// ����������
// ����ͨ���������ᵽ����DXGI API�µ�IDXGIFactory�ӿ���������������
// �������ǻ��ǽ���MSAA���ز�����
// ��Ϊ�����ñȽ��鷳������ֱ������MSAA�����
// ����count����Ϊ1������Ϊ0��
// ��Ҫע��һ�㣬CreateSwapChain�����ĵ�һ��������ʵ��������нӿ�ָ�룬
// �����豸�ӿ�ָ�룬�����������󵼡�
ComPtr<IDXGISwapChain> swapChain;// �����������ṹ��
DXGI_SWAP_CHAIN_DESC swapChainDesc;
void D3D12App::CreateSwapChain() {
	swapChain.Reset();
	swapChainDesc.BufferDesc.Width = 1280;	// �������ֱ��ʵĿ��
	swapChainDesc.BufferDesc.Height = 720;	// �������ֱ��ʵĸ߶�
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// ����������ʾ��ʽ
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;	// ˢ���ʵķ���
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;	// ˢ���ʵķ�ĸ
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// ����ɨ��VS����ɨ��(δָ����)
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// ͼ�������Ļ�����죨δָ���ģ�
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// ��������Ⱦ����̨������������Ϊ��ȾĿ�꣩
	swapChainDesc.OutputWindow = mhMainWnd;	// ��Ⱦ���ھ��
	swapChainDesc.SampleDesc.Count = 1;	// ���ز�������
	swapChainDesc.SampleDesc.Quality = 0;	// ���ز�������
	swapChainDesc.Windowed = true;	// �Ƿ񴰿ڻ�
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// �̶�д��
	swapChainDesc.BufferCount = 2;	// ��̨������������˫���壩
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// ����Ӧ����ģʽ���Զ�ѡ�������ڵ�ǰ���ڳߴ����ʾģʽ��
	// ����DXGI�ӿ��µĹ����ഴ��������
	ThrowIfFailed(dxgiFactory->CreateSwapChain(cmdQueue.Get(), &swapChainDesc, swapChain.GetAddressOf()));
}
// ������������
// ��Ϊ��˫��̨���壬��������Ҫ�������2��RTV��RTV�ѣ�
// �����ģ�建��ֻ��һ�������Դ���1��DSV��DSV�ѡ�
// ��������ǣ�
//   ����������������Խṹ�壬
//   Ȼ��ͨ���豸�����������ѡ�
D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc;
ComPtr<ID3D12DescriptorHeap> rtvHeap;
D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc;
ComPtr<ID3D12DescriptorHeap> dsvHeap;
void D3D12App::CreateDescriptorHeap() {
	// ���ȴ���RTV��
	rtvDescriptorHeapDesc.NumDescriptors = 2;
	rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.NodeMask = 0;
	ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvHeap)));
	// Ȼ�󴴽�DSV��
	dsvDescriptorHeapDesc.NumDescriptors = 1;
	dsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescriptorHeapDesc.NodeMask = 0;
	ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(&dsvHeap)));
}
// ��ȡ��������С
UINT rtvDescriptorSize;
UINT dsvDescriptorSize;
UINT cbv_srv_uavDescriptorSize;
void D3D12App::GetDescriptorSize() 
{
	// ��ȡ����������ÿ���������Ĵ�С����ȷ���������������֮����ڴ�λ��
	// UINT ����ֵΪ������������������������ռ�ڴ��ֽ���(����)
	rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// UINT 
	dsvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	// UINT 
	cbv_srv_uavDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
// ����������������ȾĿ����ͼ������
// CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle;
ComPtr<ID3D12Resource> swapChainBuffer[2];// ��������ȾĿ�껺�����Ľӿ�
void D3D12App::CreateRTV() {
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < 2; i++) {
		// ��ô��ڽ������еĺ�̨��������Դ
		swapChain->GetBuffer(i, IID_PPV_ARGS(swapChainBuffer[i].GetAddressOf()));
		// ����RTV
		d3dDevice->CreateRenderTargetView(swapChainBuffer[i].Get(),
			nullptr,	// �ڽ������������Ѿ������˸���Դ�����ݸ�ʽ����������ָ��Ϊ��ָ��
			rtvHeapHandle);	// ����������ṹ�壨�����Ǳ��壬�̳���CD3DX12_CPU_DESCRIPTOR_HANDLE��
		// ƫ�Ƶ����������е���һ��������
		rtvHeapHandle.Offset(1, rtvDescriptorSize);
	}
}
// �������/ģ�建����������ͼ
// ��������ǣ�
//   ����CPU�д�����DS��Դ��
//   Ȼ��ͨ��CreateCommittedResource������DS��Դ�ύ��GPU�Դ��У�
//   ��󴴽�DSV���Դ��е�DS��Դ��DSV�����ϵ������
D3D12_RESOURCE_DESC dsvResourceDesc;// ����ģ������
ComPtr<ID3D12Resource> depthStencilBuffer;// �������/ģ�建�����Ľӿ�
void D3D12App::CreateDSV() {
	// ��CPU�д��������ģ��������Դ
	// ����������Դ
	dsvResourceDesc.Alignment = 0;	// ָ������
	dsvResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// ָ����Դά�ȣ����ͣ�ΪTEXTURE2D
	dsvResourceDesc.DepthOrArraySize = 1;	// �������Ϊ1
	dsvResourceDesc.Width = 1280;	// ��Դ��
	dsvResourceDesc.Height = 720;	// ��Դ��
	dsvResourceDesc.MipLevels = 1;	// MIPMAP�㼶����
	dsvResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// ָ�������֣����ﲻָ����
	dsvResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	// ���ģ����Դ��Flag
	dsvResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 24λ��ȣ�8λģ��,���и������͵ĸ�ʽDXGI_FORMAT_R24G8_TYPELESSҲ����ʹ��
	dsvResourceDesc.SampleDesc.Count = 4;	// ���ز�������
	dsvResourceDesc.SampleDesc.Quality = msaaQualityLevels.NumQualityLevels - 1;	// ���ز�������
	// ���������Դ���Ż�ֵ
	CD3DX12_CLEAR_VALUE optClear;	// �����Դ���Ż�ֵ��������������ִ���ٶȣ�CreateCommittedResource�����д��룩
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// 24λ��ȣ�8λģ��,���и������͵ĸ�ʽDXGI_FORMAT_R24G8_TYPELESSҲ����ʹ��
	optClear.DepthStencil.Depth = 1;	// ��ʼ���ֵΪ1
	optClear.DepthStencil.Stencil = 0;	// ��ʼģ��ֵΪ0
	// ����һ����Դ��һ���ѣ�������Դ�ύ�����У������ģ�������ύ��GPU�Դ��У�
	CD3DX12_HEAP_PROPERTIES hpp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);// �������е����ԣ�����ΪĬ�϶ѡ�
	ThrowIfFailed(d3dDevice->CreateCommittedResource(&hpp,	// ������ΪĬ�϶ѣ�����д�룩
		D3D12_HEAP_FLAG_NONE,	// Flag
		&dsvResourceDesc,	// ���涨���DSV��Դָ��
		D3D12_RESOURCE_STATE_COMMON,	// ��Դ��״̬Ϊ��ʼ״̬
		&optClear,	// ���涨����Ż�ֵָ��
		IID_PPV_ARGS(&depthStencilBuffer)));	// �������ģ����Դ
	// ����DSV(�������DSV���Խṹ�壬�ʹ�����ȾĿ����ͼ��ͬ��RTV��ͨ�����)
	  // D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	 // dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	 //	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	 //	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	 //	dsvDesc.Texture2D.MipSlice = 0;
	d3dDevice->CreateDepthStencilView(depthStencilBuffer.Get(),
		nullptr,	// D3D12_DEPTH_STENCIL_VIEW_DESC����ָ�룬����&dsvDesc������ע�ʹ��룩��
							// �����ڴ������ģ����Դʱ�Ѿ��������ģ���������ԣ������������ָ��Ϊ��ָ��
		dsvHeap->GetCPUDescriptorHandleForHeapStart());	// DSV���
}
// ������/ģ����Դ��״̬
void D3D12App::resourceBarrierBuild() {
	cmdList->ResourceBarrier(1,	// Barrier���ϸ���
		&CD3DX12_RESOURCE_BARRIER::Transition(depthStencilBuffer.Get(),// ��ͼ��COM�ӿ�(?)��ָ��֮ǰ�����/ģ�建�����Ľӿڡ�
			D3D12_RESOURCE_STATE_COMMON,	// ת��ǰ״̬������ʱ��״̬����CreateCommittedResource�����ж����״̬��
			D3D12_RESOURCE_STATE_DEPTH_WRITE));	// ת����״̬Ϊ��д������ͼ������һ��D3D12_RESOURCE_STATE_DEPTH_READ��ֻ�ɶ������ͼ
	// �������������cmdList�󣬽�����������б���������У�Ҳ���Ǵ�CPU����GPU�Ĺ��̡�
	// ע�⣺�ڴ����������ǰ����ر������б�
	ThrowIfFailed(cmdList->Close());	// ������������ر�
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };	// ���������������б�����
	cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);	// ������������б����������
}
// ʵ��Χ��
int mCurrentFence = 0;// ��ʼCPU�ϵ�Χ����Ϊ0
void D3D12App::FlushCmdQueue() {
	mCurrentFence++;	// CPU��������رպ󣬽���ǰΧ��ֵ+1
	cmdQueue->Signal(fence.Get(), mCurrentFence);	// ��GPU������CPU���������󣬽�fence�ӿ��е�Χ��ֵ+1����fence->GetCompletedValue()+1
	if (fence->GetCompletedValue() < mCurrentFence)	// ���С�ڣ�˵��GPUû�д�������������
	{
		HANDLE eventHandle = CreateEvent(nullptr, false, false, L"FenceSetDone");	// �����¼�
		fence->SetEventOnCompletion(mCurrentFence, eventHandle);// ��Χ���ﵽmCurrentFenceֵ����ִ�е�Signal����ָ���޸���Χ��ֵ��ʱ������eventHandle�¼�
		WaitForSingleObject(eventHandle, INFINITE);// �ȴ�GPU����Χ���������¼���������ǰ�߳�ֱ���¼�������ע���Enent���������ٵȴ���
							   // ���û��Set��Wait���������ˣ�Set��Զ������ã�����Ҳ��û�߳̿��Ի�������̣߳�
		CloseHandle(eventHandle);
	}
}
// �����ӿںͲü�����
D3D12_VIEWPORT viewPort;
D3D12_RECT scissorRect;
void D3D12App::CreateViewPortAndScissorRect() {
	// �ӿ�����
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = 1280;
	viewPort.Height = 720;
	viewPort.MaxDepth = 1.0f;
	viewPort.MinDepth = 0.0f;
	// �ü��������ã�����������ض������޳���
	// ǰ����Ϊ���ϵ����꣬������Ϊ���µ�����
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = 1280;
	scissorRect.bottom = 720;
}
// ��������
UINT mCurrentBackBuffer = 0;
void D3D12App::Draw() {
	ThrowIfFailed(cmdAllocator->Reset());// �ظ�ʹ�ü�¼���������ڴ�
	ThrowIfFailed(cmdList->Reset(cmdAllocator.Get(), nullptr));// ���������б����ڴ�
	// �������ǽ���̨������Դ�ӳ���״̬ת������ȾĿ��״̬����׼������ͼ����Ⱦ��
	UINT& ref_mCurrentBackBuffer = mCurrentBackBuffer;
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuffer[ref_mCurrentBackBuffer].Get(),// ת����ԴΪ��̨��������Դ
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));// �ӳ��ֵ���ȾĿ��ת��

   // �����������ӿںͲü����Ρ�
	cmdList->RSSetViewports(1, &viewPort);
	cmdList->RSSetScissorRects(1, &scissorRect);

	// Ȼ�������̨����������Ȼ�����������ֵ��
	// �������Ȼ�ö������������������ַ����
	// ��ͨ��ClearRenderTargetView������ClearDepthStencilView����������͸�ֵ��
	// �������ǽ�RT��Դ����ɫ��ֵΪDarkRed�����죩��
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(), ref_mCurrentBackBuffer, rtvDescriptorSize);
	cmdList->ClearRenderTargetView(rtvHandle, DirectX::Colors::DarkRed, 0, nullptr);// ���RT����ɫΪ���죬���Ҳ����òü�����
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->ClearDepthStencilView(dsvHandle,	// DSV���������
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,	// FLAG
		1.0f,	// Ĭ�����ֵ
		0,	// Ĭ��ģ��ֵ
		0,	// �ü���������
		nullptr);	// �ü�����ָ��

	// Ȼ������ָ����Ҫ��Ⱦ�Ļ���������ָ��RTV��DSV��
	cmdList->OMSetRenderTargets(1,// ���󶨵�RTV����
		&rtvHandle,	// ָ��RTV�����ָ��
		true,	// RTV�����ڶ��ڴ�����������ŵ�
		&dsvHandle);	// ָ��DSV��ָ��

	// �ȵ���Ⱦ��ɣ�����Ҫ����̨��������״̬�ĳɳ���״̬��ʹ��֮���Ƶ�ǰ̨��������ʾ��
	// ���ˣ��ر������б��ȴ�����������С�
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuffer[ref_mCurrentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));// ����ȾĿ�굽����
	// �������ļ�¼�ر������б�
	ThrowIfFailed(cmdList->Close());


	// ��CPU�����׼���ú���Ҫ����ִ�е������б����GPU��������С�
	// ʹ�õ���ExecuteCommandLists������
	ID3D12CommandList* commandLists[] = { cmdList.Get() };// ���������������б�����
	cmdQueue->ExecuteCommandLists(_countof(commandLists), commandLists);// ������������б����������

	// Ȼ�󽻻�ǰ��̨������������������㷨��1��0��0��1��Ϊ���ú�̨������������ԶΪ0����
	ThrowIfFailed(swapChain->Present(0, 0));
	ref_mCurrentBackBuffer = (ref_mCurrentBackBuffer + 1) % 2;

	// �������Χ��ֵ��ˢ��������У�ʹCPU��GPUͬ������δ����ڵ�һƪ������ϸ���ͣ�����ֱ�ӷ�װ��
	FlushCmdQueue();


}
bool D3D12App::InitDirect3D()
{
	/*����D3D12���Բ�*/
#if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	CreateDevice();
	CreateFence();
	GetDescriptorSize();
	SetMSAA();
	CreateCommandObject();
	CreateSwapChain();
	CreateDescriptorHeap();
	CreateRTV();
	CreateDSV();
	CreateViewPortAndScissorRect();

	return true;
}
bool D3D12App::Init(HINSTANCE hInstance, int nShowCmd)
{
	if (!InitWindow(hInstance, nShowCmd))
	{
		return false;
	}
	else if (!InitDirect3D())
	{
		return false;
	}
	else
	{
		return true;
	}
}
// ����֡�ʺ�ÿ֡���ٺ���
void D3D12App::CalculateFrameState() {
	static int frameCnt = 0;// ��֡��
	static float timeElapsed = 0.0f;//��ʱ��
	frameCnt++;
	if (gt.TotalTime() - timeElapsed >= 1.0f) {
		float fps = frameCnt;// ÿ�����֡
		float mspf = 1000.0f / fps;// ÿ֡���ٺ���

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);
		// ��֡����ʾ�ڴ�����
		std::wstring windowText = L"D3D12Init fps: " + fpsStr + L"    " + L"mspf: " + mspfStr;
		SetWindowText(mhMainWnd, windowText.c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}


