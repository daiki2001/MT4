#include "./Header/DirectXInit.h"
#include <string>

DirectXInit::DirectXInit() :
	Win32API(),
	clearColor{ 0, 0, 0, 0 },
	dev{},
	dxgiFactory{},
	swapchain{},
	cmdAllocator{},
	cmdList{},
	cmdQueue{},
	rtvHeaps{},
	adapters{},
	tmpAdapter{},
	levels{ D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0,
	D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 },
	featureLevel{},
	cmdQueueDesc{},
	swapchainDesc{},
	heapDesc{},
	backBuffer(2),
	fence{},
	fenceVal(0),
	depthResDesc{},
	depthBuffer{},
	dsvHeapDesc{},
	dsvHeap{},
	dsvDesc{},
	bbIndex{}
{
}

void DirectXInit::DebugLayer()
{
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
}

DirectXInit* DirectXInit::GetInstance()
{
	// �C���X�^���X�̐���
	static DirectXInit instance;

	return &instance;
}

HRESULT DirectXInit::Init()
{
	HRESULT hr;

	WindowInit();
	DebugLayer();

	// DXGI�t�@�N�g���[�̐���
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr))
	{
		return hr;
	}

	for (int i = 0; dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter); //���I�z��ɒǉ�����
	}

	for (size_t i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc); //�A�_�v�^�[�̏����擾

		// �\�t�g�E�F�A�f�o�C�X�����
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		std::wstring strDesc = adesc.Description; //�A�_�v�^�[��
		// Intel UHD Graphics (�I���{�[�h�O���t�B�b�N)�����
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i]; //�̗p
			break;
		}
	}

	for (int i = 0; i < _countof(levels); i++)
	{
		// �̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		hr = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&dev));
		if (hr == S_OK)
		{
			// �f�o�C�X�𐶐��o�������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}
	if (FAILED(hr))
	{
		// �f�o�C�X�����������ꍇ���^�[��
		return hr;
	}

	// �R�}���h�A���P�[�^�𐶐�
	hr = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator)
	);
	if (FAILED(hr))
	{
		return hr;
	}

	// �R�}���h���X�g�𐶐�
	hr = dev->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&cmdList)
	);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
	if (FAILED(hr))
	{
		return hr;
	}

	swapchainDesc.Width = windowWidth;
	swapchainDesc.Height = windowHeight;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //�F���̏���
	swapchainDesc.SampleDesc.Count = 1; //�}���`�T���v�����Ȃ�
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; //�o�b�N�o�b�t�@�p
	swapchainDesc.BufferCount = 2; //�o�b�t�@����2�ɐݒ�
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //�t���b�v��͔j��
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGISwapChain1> swapchain1;
	hr = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		Win32API::GetHwnd(),
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapchain1
	);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = swapchain1.As(&swapchain);
	if (FAILED(hr))
	{
		return hr;
	}

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //�����_�[�^�[�Q�b�g�r���[
	heapDesc.NumDescriptors = 2; //���\��2��
	hr = dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
	if (FAILED(hr))
	{
		return hr;
	}

	for (UINT i = 0; i < 2; i++)
	{
		// �X���b�v�`�F�C������o�b�t�@���擾
		hr = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffer[i]));
		if (FAILED(hr))
		{
			return hr;
		}
		// �X�f�X�N���v�^�q�[�v�̃n���h�����擾
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
				i,
				dev->GetDescriptorHandleIncrementSize(heapDesc.Type)
			);
		// �����_�[�^�[�Q�b�g�r���[�̐���
		dev->CreateRenderTargetView(backBuffer[i].Get(), nullptr, handle);
	}

	CreateDepthBuffer();

	hr = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	return hr;
}

void DirectXInit::ClearScreen()
{
#pragma region ChangeResourceBarrier

	// �o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
	bbIndex = swapchain->GetCurrentBackBufferIndex();

	cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer[bbIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		)
	);

#pragma endregion

#pragma region DrawingDestination

	// �`���̎w��
	// �����_�[�^�[�Q�b�g�r���[�p�f�B�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
			(INT)bbIndex,
			dev->GetDescriptorHandleIncrementSize(heapDesc.Type)
		);
	// �[�x�X�e���V���r���[�p�f�B�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

#pragma endregion

#pragma region ClearScreen

	// ��ʃN���A
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

#pragma endregion
}

void DirectXInit::ScreenFlip()
{
#pragma region RetrunResourceBarrier

	// ���\�[�X�o���A��߂�
	cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer[bbIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		)
	);

#pragma endregion

#pragma region FlashCommand

	// ���߂̃N���[�Y
	cmdList->Close();
	// �R�}���h���X�g�̎��s
	ID3D12CommandList* cmdLists[] = { cmdList.Get() }; //�R�}���h���X�g�̔z��
	cmdQueue->ExecuteCommandLists(1, cmdLists);

	// �o�b�t�@���t���b�v(���\�̓���ւ�)
	swapchain->Present(1, 0);

	// �R�}���h�L���[�̎��s������҂�
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

#pragma endregion

#pragma region ScreenFlip

	cmdAllocator->Reset(); //�L���[���N���A
	cmdList->Reset(cmdAllocator.Get(), nullptr); //�ĂуR�}���h���X�g�𒙂߂鏀��

#pragma endregion
}

HRESULT DirectXInit::CreateDepthBuffer()
{
	HRESULT hr;

	// ���\�[�X�ݒ�
	depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		windowWidth,
		windowHeight,
		1,
		0,
		1,
		0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	// ���\�[�X����
	hr = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //�[�x�l�������݂Ɏg�p
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer)
	);
	if (FAILED(hr))
	{
		return hr;
	}

	// �[�x�r���[�p�f�X�N���v�^�q�[�v�쐬
	dsvHeapDesc.NumDescriptors = 1; //�[�x�r���[��1��
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; //�f�v�X�X�e���V���r���[
	hr = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	if (FAILED(hr))
	{
		return hr;
	}

	// �[�x�r���[�쐬
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; //�[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	return hr;
}

int DirectXInit::SetWindowSize(int width, int height)
{
	if (width <= 0 || height <= 0)
	{
		return -1;
	}

	windowWidth = width;
	windowHeight = height;

	return 0;
}
