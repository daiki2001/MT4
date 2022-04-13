#pragma once
#include "Win32API.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <wrl.h>
#include <d3dx12.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class DirectXInit final : public Win32API
{
protected: // �G�C���A�X
	// std::���ȗ�
	template<class T> using vector = std::vector<T>;
	// Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	// �R���X�g���N�^��private�ɂ���
	DirectXInit();
	// �f�X�g���N�^��private�ɂ���
	~DirectXInit() {}
public: // �����o�֐�
	// �R�s�[�R���X�g���N�^�𖳌��ɂ���
	DirectXInit(const DirectXInit& obj) = delete;
	// ������Z�q�𖳌��ɂ���
	const DirectXInit& operator=(const DirectXInit& obj) = delete;
	// �C���X�^���X�̎擾
	static DirectXInit* GetInstance();

	// ������
	HRESULT Init();

	// �`��O����
	void ClearScreen();
	// �`��㏈��
	void ScreenFlip();

	// �E�B���h�E�T�C�Y�̐ݒ�
	int SetWindowSize(int width, int height);
private:
	// �f�o�b�O���C���[
	void DebugLayer();

	// �[�x�o�b�t�@�̐���
	HRESULT CreateDepthBuffer();

public: // �����o�ϐ�
	float clearColor[4]; //�w�i�F

	ComPtr<ID3D12Device> dev;
	ComPtr<ID3D12GraphicsCommandList> cmdList;
protected:
	ComPtr<IDXGIFactory6> dxgiFactory;
	ComPtr<IDXGISwapChain4> swapchain;
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;

	vector<ComPtr<IDXGIAdapter1>> adapters; //�A�_�v�^�[�̗񋓗p
	ComPtr<IDXGIAdapter1> tmpAdapter;       //�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������

	D3D_FEATURE_LEVEL levels[4]; //�Ή����x���̔z��
	D3D_FEATURE_LEVEL featureLevel;

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc;     //�W���ݒ�ŃR�}���h�L���[�𐶐�
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc;       //�e��ݒ�����ăX���b�v�`�F�C���𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;       //�e��ݒ�����ăf�X�N���v�^�q�[�v�𐶐�
	vector<ComPtr<ID3D12Resource>> backBuffer; //���\��2�ɂ���

	// �t�F���X�̐���
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal;
private:
	CD3DX12_RESOURCE_DESC depthResDesc;     //���\�[�X�ݒ�
	ComPtr<ID3D12Resource> depthBuffer;     //���\�[�X����
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc; //�[�x�r���[�p�ݒ�\����
	ComPtr<ID3D12DescriptorHeap> dsvHeap;   //�[�x�r���[�p�f�X�N���v�^�q�[�v�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;  //�[�x�r���[�쐬

	SIZE_T bbIndex; //�o�b�N�o�b�t�@�̃C���f�b�N�X

};
