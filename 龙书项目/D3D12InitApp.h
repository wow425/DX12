#pragma once
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
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")


class D3D12InitApp : public D3D12App
{
public:
	D3D12InitApp() = default;
	~D3D12InitApp() = default;
private:
	virtual void Draw();
	ComPtr<ID3D12Resource> CreateDefaultBuffer(UINT64 byteSize, const void* initData, ComPtr<ID3D12Resource>& uploadBuffer);
};