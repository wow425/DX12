#include "D3D12InitApp.h"
void D3D12InitApp::Draw()
{
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
	// ���ｫRT��Դ����ɫ��ֵΪDarkRed�����죩��
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
// ����ṹ��
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};
// ʵ��������ṹ�岢���
array<Vertex, 8> vertices =
{
	Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
	Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
	Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
	Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
	Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
	Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
	Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
	Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
};
// �����ϴ��ѣ�CPUд�롣�ϴ����еĶ������ݴ���Ĭ�϶ѣ�Ĭ�϶�ֻ�ܱ�GPU���ʡ�
ComPtr<ID3D12Resource> D3D12InitApp::CreateDefaultBuffer(UINT64 byteSize, const void* initData, ComPtr<ID3D12Resource>& uploadBuffer)
{
	//�����ϴ��ѣ������ǣ�д��CPU�ڴ����ݣ��������Ĭ�϶�
	ThrowIfFailed(d3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�����ϴ������͵Ķ�
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),//����Ĺ��캯��������byteSize��������ΪĬ��ֵ������д
		D3D12_RESOURCE_STATE_GENERIC_READ,	//�ϴ��������Դ��Ҫ���Ƹ�Ĭ�϶ѣ������ǿɶ�״̬
		nullptr,	//�������ģ����Դ������ָ���Ż�ֵ
		IID_PPV_ARGS(&uploadBuffer)));
}