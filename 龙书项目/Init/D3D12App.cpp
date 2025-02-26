#include "D3D12App.h"
#include "GameTime.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// 消息处理
	switch (msg) {
		// 当窗口被销毁时，终止消息循环
	case WM_DESTROY:
		PostQuitMessage(0);	// 终止消息循环，并发出WM_QUIT消息
		return 0;
	default:
		break;
	}
	// 将上面没有处理的消息转发给默认的窗口过程
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool D3D12App::InitWindow(HINSTANCE hInstance, int nShowCmd)
{
	//窗口初始化描述结构体(WNDCLASS)
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;	// 当工作区宽高改变，则重新绘制窗口
	wc.lpfnWndProc = MainWndProc;	//指定窗口过程
	wc.cbClsExtra = 0;	// 借助这两个字段来为当前应用分配额外的内存空间（这里不分配，所以置0）
	wc.cbWndExtra = 0;	// 借助这两个字段来为当前应用分配额外的内存空间（这里不分配，所以置0）
	wc.hInstance = hInstance;	// 应用程序实例句柄（由WinMain传入）
	wc.hIcon = LoadIcon(0, IDC_ARROW);	// 使用默认的应用程序图标
	wc.hCursor = LoadCursor(0, IDC_ARROW);	// 使用标准的鼠标指针样式
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// 指定了白色背景画刷句柄
	wc.lpszMenuName = 0;	// 没有菜单栏
	wc.lpszClassName = L"MainWnd";	// 窗口名
	//窗口类注册失败
	if (!RegisterClass(&wc)) {
		//消息框函数，参数1：消息框所属窗口句柄，可为NULL。参数2：消息框显示的文本信息。参数3：标题文本。参数4：消息框样式
		MessageBox(0, L"RegisterClass Failed", 0, 0);
		return 0;
	}
	RECT R;	// 裁剪矩形
	R.left = 0;
	R.top = 0;
	R.right = 1280;
	R.bottom = 720;
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);	//根据窗口的客户区大小计算窗口的大小
	int width = R.right - R.left;
	int hight = R.bottom - R.top;

	// 创建窗口,返回布尔值
	// CreateWindowW(lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
	mhMainWnd = CreateWindow(L"MainWnd", L"DX12Initialize", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, hight, 0, 0, hInstance, 0);
	// 窗口创建失败
	if (!mhMainWnd) {
		MessageBox(0, L"CreatWindow Failed", 0, 0);
		return 0;
	}
	// 窗口创建成功,则显示并更新窗口
	ShowWindow(mhMainWnd, nShowCmd);
	UpdateWindow(mhMainWnd);

	return true;
}
int D3D12App::Run()
{
	//消息循环
	//定义消息结构体
	MSG msg = { 0 };
	//每次循环开始都要重置计时器
	gt.Reset();
	//如果GetMessage函数不等于0，说明没有接受到WM_QUIT
	while (msg.message != WM_QUIT)
	{
		//如果有窗口消息就进行处理
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))//PeekMessage函数会自动填充msg结构体元素
		{
			TranslateMessage(&msg);	//键盘按键转换，将虚拟键消息转换为字符消息
			DispatchMessage(&msg);	//把消息分派给相应的窗口过程
		}
		//否则就执行动画和游戏逻辑
		else
		{
			gt.Tick();	//计算每两帧间隔时间
			if (!gt.IsStoped())//如果不是暂停状态，我们才运行游戏
			{
				CalculateFrameState();
				Draw();
			}
			//如果是暂停状态，则休眠100秒
			else
			{
				Sleep(100);
			}

		}
	}
	return (int)msg.wParam;
}
// 将接收到的消息分派给窗口过程
// 窗口过程函数

ComPtr<ID3D12Device> d3dDevice;
ComPtr<IDXGIFactory4> dxgiFactory;
// 创建设备
void D3D12App::CreateDevice() {
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory))); // IID_PPV_ARGS将传入接口转换为显式的IID和void**
	ThrowIfFailed(D3D12CreateDevice(nullptr, // 选用哪个适配器，nullptr默认为主适配器
		D3D_FEATURE_LEVEL_12_0,		// 应用程序支持的最低功能级别
		IID_PPV_ARGS(&d3dDevice)));	// 将申请到的接口(指针)返回给d3dDevice
}
// 接下来是通过设备创建围栏fence，以便之后同步CPU和GPU。
ComPtr<ID3D12Fence> fence;
void D3D12App::CreateFence() {
	ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
	// 参数1为围栏初始值. 参数2为围栏的创建标志. 参数3为将申请到的接口(指针)返回给fence
}
D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaQualityLevels;
// 设置MSAA抗锯齿属性。
// 注意：此处不使用MSAA，采样数量设置为1（即不采样）。
void D3D12App::SetMSAA() {
	msaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// UNORM是归一化处理的无符号整数
	msaaQualityLevels.SampleCount = 1;
	msaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;// 没有任何选项支持
	msaaQualityLevels.NumQualityLevels = 0;
	// 当前图形驱动对MSAA多重采样的支持（注意：第二个参数即是输入又是输出）
	ThrowIfFailed(d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaaQualityLevels, sizeof(msaaQualityLevels)));
	// NumQualityLevels在Check函数里会进行设置
	// 如果支持MSAA，则Check函数返回的NumQualityLevels > 0
	// expression为假（即为0），则终止程序运行，并打印一条出错信息
	assert(msaaQualityLevels.NumQualityLevels > 0);
}
// 创建命令队列、命令列表和命令分配器。
// 注意，我们在初始化D3D12_COMMAND_QUEUE_DESC时，
// 只初始化了两项，其他两项我们在大括号中默认初始化了。
D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};// 描述队列
ComPtr<ID3D12CommandQueue> cmdQueue;// 命令队列的COM
ComPtr<ID3D12CommandAllocator> cmdAllocator;// 命令分配器的COM 
ComPtr<ID3D12GraphicsCommandList> cmdList;// 命令列表的COM
void D3D12App::CreateCommandObject() {
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;// 指定GPU可以执行的命令缓冲区，直接命令列表未继承任何GPU状态
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;// 默认命令队列
	ThrowIfFailed(d3dDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&cmdQueue)));// 创建命令队列
	ThrowIfFailed(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator)));// 创建命令分配器 &cmdAllocator等价于cmdAllocator.GetAddressOf
	// 创建命令列表
	ThrowIfFailed(d3dDevice->CreateCommandList(0, // 掩码值为0，单GPU
		D3D12_COMMAND_LIST_TYPE_DIRECT, // 命令列表类型
		cmdAllocator.Get(),	// 命令分配器接口指针
		nullptr,	// 流水线状态对象PSO，这里不绘制，所以空指针
		IID_PPV_ARGS(&cmdList)));	// 返回创建的命令列表
	cmdList->Close();	// 重置命令列表前必须将其关闭,以确保命令列表中的所有命令被正确记录
}
// 创建交换链
// 我们通过上文中提到过的DXGI API下的IDXGIFactory接口来创建交换链。
// 这里我们还是禁用MSAA多重采样。
// 因为其设置比较麻烦，这里直接设置MSAA会出错，
// 所以count还是为1，质量为0。
// 还要注意一点，CreateSwapChain函数的第一个参数其实是命令队列接口指针，
// 不是设备接口指针，参数描述有误导。
ComPtr<IDXGISwapChain> swapChain;// 交换链描述结构体
DXGI_SWAP_CHAIN_DESC swapChainDesc;
void D3D12App::CreateSwapChain() {
	swapChain.Reset();
	swapChainDesc.BufferDesc.Width = 1280;	// 缓冲区分辨率的宽度
	swapChainDesc.BufferDesc.Height = 720;	// 缓冲区分辨率的高度
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 缓冲区的显示格式
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;	// 刷新率的分子
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;	// 刷新率的分母
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// 逐行扫描VS隔行扫描(未指定的)
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// 图像相对屏幕的拉伸（未指定的）
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 将数据渲染至后台缓冲区（即作为渲染目标）
	swapChainDesc.OutputWindow = mhMainWnd;	// 渲染窗口句柄
	swapChainDesc.SampleDesc.Count = 1;	// 多重采样数量
	swapChainDesc.SampleDesc.Quality = 0;	// 多重采样质量
	swapChainDesc.Windowed = true;	// 是否窗口化
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// 固定写法
	swapChainDesc.BufferCount = 2;	// 后台缓冲区数量（双缓冲）
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// 自适应窗口模式（自动选择最适于当前窗口尺寸的显示模式）
	// 利用DXGI接口下的工厂类创建交换链
	ThrowIfFailed(dxgiFactory->CreateSwapChain(cmdQueue.Get(), &swapChainDesc, swapChain.GetAddressOf()));
}
// 创建描述符堆
// 因为是双后台缓冲，所以我们要创建存放2个RTV的RTV堆，
// 而深度模板缓存只有一个，所以创建1个DSV的DSV堆。
// 具体过程是，
//   先填充描述符堆属性结构体，
//   然后通过设备创建描述符堆。
D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc;
ComPtr<ID3D12DescriptorHeap> rtvHeap;
D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc;
ComPtr<ID3D12DescriptorHeap> dsvHeap;
void D3D12App::CreateDescriptorHeap() {
	// 首先创建RTV堆
	rtvDescriptorHeapDesc.NumDescriptors = 2;
	rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.NodeMask = 0;
	ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvHeap)));
	// 然后创建DSV堆
	dsvDescriptorHeapDesc.NumDescriptors = 1;
	dsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescriptorHeapDesc.NodeMask = 0;
	ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(&dsvHeap)));
}
// 获取描述符大小
UINT rtvDescriptorSize;
UINT dsvDescriptorSize;
UINT cbv_srv_uavDescriptorSize;
void D3D12App::GetDescriptorSize() 
{
	// 获取描述符堆中每个描述符的大小，以确定如何跳过描述符之间的内存位置
	// UINT 返回值为该种描述符在描述符堆中所占内存字节数(增量)
	rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// UINT 
	dsvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	// UINT 
	cbv_srv_uavDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
// 创建描述符堆中渲染目标视图描述符
// CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle;
ComPtr<ID3D12Resource> swapChainBuffer[2];// 操作是渲染目标缓冲区的接口
void D3D12App::CreateRTV() {
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < 2; i++) {
		// 获得存于交换链中的后台缓冲区资源
		swapChain->GetBuffer(i, IID_PPV_ARGS(swapChainBuffer[i].GetAddressOf()));
		// 创建RTV
		d3dDevice->CreateRenderTargetView(swapChainBuffer[i].Get(),
			nullptr,	// 在交换链创建中已经定义了该资源的数据格式，所以这里指定为空指针
			rtvHeapHandle);	// 描述符句柄结构体（这里是变体，继承自CD3DX12_CPU_DESCRIPTOR_HANDLE）
		// 偏移到描述符堆中的下一个缓冲区
		rtvHeapHandle.Offset(1, rtvDescriptorSize);
	}
}
// 创建深度/模板缓冲区及其视图
// 具体过程是，
//   先在CPU中创建好DS资源，
//   然后通过CreateCommittedResource函数将DS资源提交至GPU显存中，
//   最后创建DSV将显存中的DS资源和DSV句柄联系起来。
D3D12_RESOURCE_DESC dsvResourceDesc;// 描述模板类型
ComPtr<ID3D12Resource> depthStencilBuffer;// 操作深度/模板缓冲区的接口
void D3D12App::CreateDSV() {
	// 在CPU中创建好深度模板数据资源
	// 描述纹理资源
	dsvResourceDesc.Alignment = 0;	// 指定对齐
	dsvResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 指定资源维度（类型）为TEXTURE2D
	dsvResourceDesc.DepthOrArraySize = 1;	// 纹理深度为1
	dsvResourceDesc.Width = 1280;	// 资源宽
	dsvResourceDesc.Height = 720;	// 资源高
	dsvResourceDesc.MipLevels = 1;	// MIPMAP层级数量
	dsvResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// 指定纹理布局（这里不指定）
	dsvResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	// 深度模板资源的Flag
	dsvResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 24位深度，8位模板,还有个无类型的格式DXGI_FORMAT_R24G8_TYPELESS也可以使用
	dsvResourceDesc.SampleDesc.Count = 4;	// 多重采样数量
	dsvResourceDesc.SampleDesc.Quality = msaaQualityLevels.NumQualityLevels - 1;	// 多重采样质量
	// 描述清除资源的优化值
	CD3DX12_CLEAR_VALUE optClear;	// 清除资源的优化值，提高清除操作的执行速度（CreateCommittedResource函数中传入）
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// 24位深度，8位模板,还有个无类型的格式DXGI_FORMAT_R24G8_TYPELESS也可以使用
	optClear.DepthStencil.Depth = 1;	// 初始深度值为1
	optClear.DepthStencil.Stencil = 0;	// 初始模板值为0
	// 创建一个资源和一个堆，并将资源提交至堆中（将深度模板数据提交至GPU显存中）
	CD3DX12_HEAP_PROPERTIES hpp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);// 堆所具有的属性，设置为默认堆。
	ThrowIfFailed(d3dDevice->CreateCommittedResource(&hpp,	// 堆类型为默认堆（不能写入）
		D3D12_HEAP_FLAG_NONE,	// Flag
		&dsvResourceDesc,	// 上面定义的DSV资源指针
		D3D12_RESOURCE_STATE_COMMON,	// 资源的状态为初始状态
		&optClear,	// 上面定义的优化值指针
		IID_PPV_ARGS(&depthStencilBuffer)));	// 返回深度模板资源
	// 创建DSV(必须填充DSV属性结构体，和创建渲染目标视图不同，RTV是通过句柄)
	  // D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	 // dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	 //	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	 //	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	 //	dsvDesc.Texture2D.MipSlice = 0;
	d3dDevice->CreateDepthStencilView(depthStencilBuffer.Get(),
		nullptr,	// D3D12_DEPTH_STENCIL_VIEW_DESC类型指针，可填&dsvDesc（见上注释代码），
							// 由于在创建深度模板资源时已经定义深度模板数据属性，所以这里可以指定为空指针
		dsvHeap->GetCPUDescriptorHandleForHeapStart());	// DSV句柄
}
// 标记深度/模板资源的状态
void D3D12App::resourceBarrierBuild() {
	cmdList->ResourceBarrier(1,	// Barrier屏障个数
		&CD3DX12_RESOURCE_BARRIER::Transition(depthStencilBuffer.Get(),// 视图的COM接口(?)。指定之前的深度/模板缓冲区的接口。
			D3D12_RESOURCE_STATE_COMMON,	// 转换前状态（创建时的状态，即CreateCommittedResource函数中定义的状态）
			D3D12_RESOURCE_STATE_DEPTH_WRITE));	// 转换后状态为可写入的深度图，还有一个D3D12_RESOURCE_STATE_DEPTH_READ是只可读的深度图
	// 等所有命令都进入cmdList后，将命令从命令列表传入命令队列，也就是从CPU传入GPU的过程。
	// 注意：在传入命令队列前必须关闭命令列表。
	ThrowIfFailed(cmdList->Close());	// 命令添加完后将其关闭
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };	// 声明并定义命令列表数组
	cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);	// 将命令从命令列表传至命令队列
}
// 实现围栏
int mCurrentFence = 0;// 初始CPU上的围栏点为0
void D3D12App::FlushCmdQueue() {
	mCurrentFence++;	// CPU传完命令并关闭后，将当前围栏值+1
	cmdQueue->Signal(fence.Get(), mCurrentFence);	// 当GPU处理完CPU传入的命令后，将fence接口中的围栏值+1，即fence->GetCompletedValue()+1
	if (fence->GetCompletedValue() < mCurrentFence)	// 如果小于，说明GPU没有处理完所有命令
	{
		HANDLE eventHandle = CreateEvent(nullptr, false, false, L"FenceSetDone");	// 创建事件
		fence->SetEventOnCompletion(mCurrentFence, eventHandle);// 当围栏达到mCurrentFence值（即执行到Signal（）指令修改了围栏值）时触发的eventHandle事件
		WaitForSingleObject(eventHandle, INFINITE);// 等待GPU命中围栏，激发事件（阻塞当前线程直到事件触发，注意此Enent需先设置再等待，
							   // 如果没有Set就Wait，就死锁了，Set永远不会调用，所以也就没线程可以唤醒这个线程）
		CloseHandle(eventHandle);
	}
}
// 设置视口和裁剪矩形
D3D12_VIEWPORT viewPort;
D3D12_RECT scissorRect;
void D3D12App::CreateViewPortAndScissorRect() {
	// 视口设置
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = 1280;
	viewPort.Height = 720;
	viewPort.MaxDepth = 1.0f;
	viewPort.MinDepth = 0.0f;
	// 裁剪矩形设置（矩形外的像素都将被剔除）
	// 前两个为左上点坐标，后两个为右下点坐标
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = 1280;
	scissorRect.bottom = 720;
}
// 绘制命令
UINT mCurrentBackBuffer = 0;
void D3D12App::Draw() {
	ThrowIfFailed(cmdAllocator->Reset());// 重复使用记录命令的相关内存
	ThrowIfFailed(cmdList->Reset(cmdAllocator.Get(), nullptr));// 复用命令列表及其内存
	// 接着我们将后台缓冲资源从呈现状态转换到渲染目标状态（即准备接收图像渲染）
	UINT& ref_mCurrentBackBuffer = mCurrentBackBuffer;
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuffer[ref_mCurrentBackBuffer].Get(),// 转换资源为后台缓冲区资源
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));// 从呈现到渲染目标转换

   // 接下来设置视口和裁剪矩形。
	cmdList->RSSetViewports(1, &viewPort);
	cmdList->RSSetScissorRects(1, &scissorRect);

	// 然后清除后台缓冲区和深度缓冲区，并赋值。
	// 步骤是先获得堆中描述符句柄（即地址），
	// 再通过ClearRenderTargetView函数和ClearDepthStencilView函数做清除和赋值。
	// 这里我们将RT资源背景色赋值为DarkRed（暗红）。
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(), ref_mCurrentBackBuffer, rtvDescriptorSize);
	cmdList->ClearRenderTargetView(rtvHandle, DirectX::Colors::DarkRed, 0, nullptr);// 清除RT背景色为暗红，并且不设置裁剪矩形
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->ClearDepthStencilView(dsvHandle,	// DSV描述符句柄
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,	// FLAG
		1.0f,	// 默认深度值
		0,	// 默认模板值
		0,	// 裁剪矩形数量
		nullptr);	// 裁剪矩形指针

	// 然后我们指定将要渲染的缓冲区，即指定RTV和DSV。
	cmdList->OMSetRenderTargets(1,// 待绑定的RTV数量
		&rtvHandle,	// 指向RTV数组的指针
		true,	// RTV对象在堆内存中是连续存放的
		&dsvHandle);	// 指向DSV的指针

	// 等到渲染完成，我们要将后台缓冲区的状态改成呈现状态，使其之后推到前台缓冲区显示。
	// 完了，关闭命令列表，等待传入命令队列。
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuffer[ref_mCurrentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));// 从渲染目标到呈现
	// 完成命令的记录关闭命令列表
	ThrowIfFailed(cmdList->Close());


	// 等CPU将命令都准备好后，需要将待执行的命令列表加入GPU的命令队列。
	// 使用的是ExecuteCommandLists函数。
	ID3D12CommandList* commandLists[] = { cmdList.Get() };// 声明并定义命令列表数组
	cmdQueue->ExecuteCommandLists(_countof(commandLists), commandLists);// 将命令从命令列表传至命令队列

	// 然后交换前后台缓冲区索引（这里的算法是1变0，0变1，为了让后台缓冲区索引永远为0）。
	ThrowIfFailed(swapChain->Present(0, 0));
	ref_mCurrentBackBuffer = (ref_mCurrentBackBuffer + 1) % 2;

	// 最后设置围栏值，刷新命令队列，使CPU和GPU同步，这段代码在第一篇中有详细解释，这里直接封装。
	FlushCmdQueue();


}
bool D3D12App::InitDirect3D()
{
	/*开启D3D12调试层*/
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
// 计算帧率和每帧多少毫秒
void D3D12App::CalculateFrameState() {
	static int frameCnt = 0;// 总帧数
	static float timeElapsed = 0.0f;//总时间
	frameCnt++;
	if (gt.TotalTime() - timeElapsed >= 1.0f) {
		float fps = frameCnt;// 每秒多少帧
		float mspf = 1000.0f / fps;// 每帧多少毫秒

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);
		// 将帧数显示在窗口上
		std::wstring windowText = L"D3D12Init fps: " + fpsStr + L"    " + L"mspf: " + mspfStr;
		SetWindowText(mhMainWnd, windowText.c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}


