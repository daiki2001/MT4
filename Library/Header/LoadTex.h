#pragma once
#include "DirectDrawing.h"
#include <DirectXTex.h>
#include <d3dx12.h>

// �e�N�X�`���S�̂̃f�[�^���܂Ƃ߂��\����
struct TextrueCommon
{
	DirectX::XMFLOAT4 noneTextrue[1]; //�摜�����p
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImg;
	const DirectX::Image* img;

	UINT textrueCount;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap; //srv��p�̃f�B�X�N���v�^�q�[�v

	TextrueCommon();
};

// �e�N�X�`���ꖇ���̃f�[�^���܂Ƃ߂��\����
struct Textrue
{
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff; //�e�N�X�`���o�b�t�@

	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandle; //CPU���̃n���h��
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandle; //GPU���̃n���h��
};

class LoadTex : public DirectDrawing
{
	/*namespace���ȗ�*/
protected:
	using XMFLOAT4 = DirectX::XMFLOAT4;

	/*�����o�ϐ�*/
protected:
	static vector<Textrue> textrueData;
	static TextrueCommon texCommonData;
private:
	UINT spriteCount;

	// �֐��̏������t���O
	bool isLoadTextrueInit = false;
	bool isDrawTextrueInit = false;

	/*�����o�֐�*/
public:
	LoadTex(const DirectXInit* w);
	~LoadTex();

	// �e�N�X�`���o�b�t�@�̐���
	int LoadTextrue(const wchar_t* fileName = nullptr);

	// �X�v���C�g�̕`��
	int DrawTextrue(const float& posX, const float& posY, const float& width, const float& height,
		const float& angle = 0, const int& graphHandle = 0,
		const DirectX::XMFLOAT2& anchorpoint = { 0.5f, 0.5f }, const XMFLOAT4& color = { 1, 1, 1, 1 },
		const int& parent = -1);
	
	// �X�v���C�g�̐؂�o���`��
	int DrawCutTextrue(const float& posX, const float& posY, const float& width, const float& height,
		const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, const float& angle = 0,
		const int& graphHandle = 0, const DirectX::XMFLOAT2& anchorpoint = { 0.5f, 0.5f },
		const XMFLOAT4& color = { 1, 1, 1, 1 }, const int& parent = -1);

	// �����̏������p�֐�
	void LoopEnd();
private:
	// �f�[�^�̏���
	void DataClear();

	int DrawTextureInit();
};
