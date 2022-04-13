#include "./Header/LoadTex.h"

#define EoF -1 // End of Function

TextrueCommon::TextrueCommon() :
	metadata{},
	scratchImg{},
	img{},
	textrueCount(1),
	descHeap{}
{
	noneTextrue[0] = { 1.0f, 1.0f, 1.0f, 1.0f };
}

/*static�ϐ��̏�����*/
DirectDrawing::vector<Textrue> LoadTex::textrueData = {};
TextrueCommon LoadTex::texCommonData = {};

LoadTex::LoadTex(const DirectXInit* w) :
	DirectDrawing(w),
	spriteCount(0)
{
}

LoadTex::~LoadTex()
{
	DataClear();
}

int LoadTex::LoadTextrue(const wchar_t* fileName)
{
	HRESULT hr = S_FALSE;
	CD3DX12_RESOURCE_DESC texResDesc{};
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; //�ݒ�\����

	if (isLoadTextrueInit == false)
	{
		isLoadTextrueInit = true;

		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{}; //�ݒ�\����
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //�V�F�[�_���猩����
		descHeapDesc.NumDescriptors = (UINT)(50000); //�e�N�X�`���o�b�t�@�̐�
		// �f�X�N���v�^�q�[�v�̐���
		hr = w->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&texCommonData.descHeap));
		if (FAILED(hr))
		{
			return EoF;
		}
	}

	if (textrueData.size() <= 0)
	{
		texResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			1,
			1,
			1,
			1
		);

		textrueData.push_back({});
		hr = w->dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, //�e�N�X�`���p�w��
			nullptr,
			IID_PPV_ARGS(&textrueData[0].texbuff)
		);
		if (FAILED(hr))
		{
			return EoF;
		}

		// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		hr = textrueData[0].texbuff->WriteToSubresource(
			0,
			nullptr,                   //�S�̈�փR�s�[
			texCommonData.noneTextrue, //���f�[�^�A�h���X
			sizeof(XMFLOAT4) * 1,      //1���C���T�C�Y
			sizeof(XMFLOAT4) * 1       //�S�T�C�Y
		);
		if (FAILED(hr))
		{
			return EoF;
		}

		// �f�X�N���v�^�q�[�v�̐擪�n���h��(CPU)���擾
		textrueData[textrueData.size() - 1].cpuDescHandle =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				texCommonData.descHeap->GetCPUDescriptorHandleForHeapStart(),
				(INT)(textrueData.size() - 1),
				w->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			);

		// �f�X�N���v�^�q�[�v�̐擪�n���h��(GPU)���擾
		textrueData[textrueData.size() - 1].gpuDescHandle =
			CD3DX12_GPU_DESCRIPTOR_HANDLE(
				texCommonData.descHeap->GetGPUDescriptorHandleForHeapStart(),
				(INT)(textrueData.size() - 1),
				w->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			);

		// �V�F�[�_���\�[�X�r���[�ݒ�
		srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2D�e�N�X�`��
		srvDesc.Texture2D.MipLevels = 1;

		w->dev->CreateShaderResourceView(
			textrueData[0].texbuff.Get(), //�r���[�Ɗ֘A�t����o�b�t�@
			&srvDesc, //�e�N�X�`���ݒ���
			textrueData[textrueData.size() - 1].cpuDescHandle
		);
	}
	if (fileName != nullptr)
	{
		texCommonData.textrueCount++;

		// WIC�e�N�X�`���̃��[�h
		LoadFromWICFile(
			fileName,
			DirectX::WIC_FLAGS_IGNORE_SRGB,
			&texCommonData.metadata,
			texCommonData.scratchImg
		);

		texCommonData.img = texCommonData.scratchImg.GetImage(0, 0, 0);

		texResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			texCommonData.metadata.format,
			texCommonData.metadata.width,
			(UINT)texCommonData.metadata.height,
			(UINT16)texCommonData.metadata.arraySize,
			(UINT16)texCommonData.metadata.mipLevels
		);

		textrueData.push_back({});
		hr = w->dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, //�e�N�X�`���p�w��
			nullptr,
			IID_PPV_ARGS(&textrueData[textrueData.size() - 1].texbuff)
		);
		if (FAILED(hr))
		{
			return EoF;
		}

		// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		hr = textrueData[textrueData.size() - 1].texbuff->WriteToSubresource(
			0,
			nullptr,                            //�S�̈�փR�s�[
			texCommonData.img->pixels,          //���f�[�^�A�h���X
			(UINT)texCommonData.img->rowPitch,  //1���C���T�C�Y
			(UINT)texCommonData.img->slicePitch //�ꖇ�T�C�Y
		);
		if (FAILED(hr))
		{
			return EoF;
		}

		// �f�X�N���v�^�q�[�v�̐擪�n���h��(CPU)���擾
		textrueData[textrueData.size() - 1].cpuDescHandle =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				texCommonData.descHeap->GetCPUDescriptorHandleForHeapStart(),
				(INT)(textrueData.size() - 1),
				w->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			);

		// �f�X�N���v�^�q�[�v�̐擪�n���h��(GPU)���擾
		textrueData[textrueData.size() - 1].gpuDescHandle =
			CD3DX12_GPU_DESCRIPTOR_HANDLE(
				texCommonData.descHeap->GetGPUDescriptorHandleForHeapStart(),
				(INT)(textrueData.size() - 1),
				w->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			);

		// �V�F�[�_���\�[�X�r���[�ݒ�
		srvDesc.Format = texCommonData.metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2D�e�N�X�`��
		srvDesc.Texture2D.MipLevels = 1;

		w->dev->CreateShaderResourceView(
			textrueData[textrueData.size() - 1].texbuff.Get(), //�r���[�Ɗ֘A�t����o�b�t�@
			&srvDesc, //�e�N�X�`���ݒ���
			textrueData[textrueData.size() - 1].cpuDescHandle
		);
	}

	return texCommonData.textrueCount - 1;
}

void LoadTex::DataClear()
{
	textrueData.clear();
	texCommonData = {};
}

int LoadTex::DrawTextureInit()
{
	int size = 0;

	size = CreateSprite();
	if (size == EoF)
	{
		return EoF;
	}

	if (isDrawTextrueInit == false)
	{
		isDrawTextrueInit = true;

		if (LoadTextrue() == EoF)
		{
			return EoF;
		}
	}

	return size;
}

int LoadTex::DrawTextrue(const float& posX, const float& posY, const float& width, const float& height,
	const float& angle, const int& graphHandle, const DirectX::XMFLOAT2& anchorpoint,
	const XMFLOAT4& color, const int& parent)
{
	using namespace DirectX;

	if ((graphHandle < 0 || (UINT)graphHandle > texCommonData.textrueCount) ||
		(parent < -1 || (parent != -1 && (size_t)parent >= spriteIndex.size())))
	{
		return EoF;
	}

	bool isInit = false;

	if ((size_t)(spriteCount + 1) < spriteIndex.size())
	{
		isInit = true;
	}

	if (isInit == false)
	{
		int size = DrawTextureInit();
		if (size == EoF)
		{
			return EoF;
		}

		spriteIndex.push_back({ size, graphHandle });
	}

	if (spriteIndex.size() == 0)
	{
		return EoF;
	}

	spriteCount++;
	spriteCount %= spriteIndex.size();

#pragma region GraphicsCommand

	BaseDrawSpriteGraphics();

	IndexData& index = spriteIndex[spriteCount];
	index.textrue = graphHandle;

	if (sprite[index.constant].size.x != width ||
		sprite[index.constant].size.y != height ||
		sprite[index.constant].anchorpoint.x != anchorpoint.x ||
		sprite[index.constant].anchorpoint.y != anchorpoint.y)
	{
		sprite[index.constant].size = { width, height };
		sprite[index.constant].anchorpoint = anchorpoint;

		enum Corner { LB, LT, RB, RT };

		SpriteVertex vert[] = {
			{{}, { 0.0f, 1.0f }},
			{{}, { 0.0f, 0.0f }},
			{{}, { 1.0f, 1.0f }},
			{{}, { 1.0f, 0.0f }},
		};

		float left = (0.0f - sprite[index.constant].anchorpoint.x) * sprite[index.constant].size.x;
		float right = (1.0f - sprite[index.constant].anchorpoint.x) * sprite[index.constant].size.x;
		float top = (0.0f - sprite[index.constant].anchorpoint.y) * sprite[index.constant].size.y;
		float bottom = (1.0f - sprite[index.constant].anchorpoint.y) * sprite[index.constant].size.y;

		vert[LB].pos = {  left, bottom, 0.0f };
		vert[LT].pos = {  left,    top, 0.0f };
		vert[RB].pos = { right, bottom, 0.0f };
		vert[RT].pos = { right,    top, 0.0f };

		// ���_�o�b�t�@�ւ̃f�[�^�]��
		SpriteVertex* vertexMap = nullptr;
		sprite[index.constant].vertBuff->Map(0, nullptr, (void**)&vertexMap);
		memcpy(vertexMap, vert, sizeof(vert));
		sprite[index.constant].vertBuff->Unmap(0, nullptr);
	}

	sprite[index.constant].color = color;
	sprite[index.constant].pos = { posX, posY, 0 };
	sprite[index.constant].rotation = angle;

	sprite[index.constant].matWorld = XMMatrixIdentity();
	sprite[index.constant].matWorld *=
		XMMatrixRotationZ(XMConvertToRadians(sprite[index.constant].rotation));
	sprite[index.constant].matWorld *= XMMatrixTranslation(
		sprite[index.constant].pos.x,
		sprite[index.constant].pos.y,
		sprite[index.constant].pos.z);

	if (parent != -1)
	{
		sprite[index.constant].matWorld *= sprite[parent].matWorld;
	}

	SpriteConstBufferData* constMap = nullptr;
	sprite[index.constant].constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = sprite[index.constant].color;
	constMap->mat = sprite[index.constant].matWorld *
		spriteData.matProjection[CommonData::Projection::ORTHOGRAPHIC];
	sprite[index.constant].constBuff->Unmap(0, nullptr);

	// �f�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap* ppHeaps[] = { texCommonData.descHeap.Get() };
	w->cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// �萔�o�b�t�@�r���[���Z�b�g
	w->cmdList->SetGraphicsRootConstantBufferView(0, sprite[index.constant].constBuff->GetGPUVirtualAddress());
	w->cmdList->SetGraphicsRootDescriptorTable(1, textrueData[index.textrue].gpuDescHandle);

	// ���_�o�b�t�@�̐ݒ�
	w->cmdList->IASetVertexBuffers(0, 1, &sprite[index.constant].vbView);
	// �`��R�}���h
	w->cmdList->DrawInstanced(4, 1, 0, 0);

#pragma endregion

	return index.constant;
}

int LoadTex::DrawCutTextrue(const float& posX, const float& posY, const float& width, const float& height,
	const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, const float& angle,
	const int& graphHandle, const DirectX::XMFLOAT2& anchorpoint, const XMFLOAT4& color, const int& parent)
{
	using namespace DirectX;

	if ((graphHandle < 0 || (UINT)graphHandle > texCommonData.textrueCount) ||
		(parent < -1 && (size_t)parent >= spriteIndex.size()))
	{
		return EoF;
	}

	bool isInit = false;

	if ((size_t)(spriteCount + 1) < spriteIndex.size())
	{
		isInit = true;
	}

	if (isInit == false)
	{
		int size = DrawTextureInit();
		if (size == EoF)
		{
			return EoF;
		}

		spriteIndex.push_back({ size, graphHandle });
	}

	if (spriteIndex.size() == 0)
	{
		return EoF;
	}

	spriteCount++;
	spriteCount %= spriteIndex.size();

#pragma region GraphicsCommand

	BaseDrawSpriteGraphics();

	IndexData& index = spriteIndex[spriteCount];
	index.textrue = graphHandle;

	if (sprite[index.constant].size.x != width ||
		sprite[index.constant].size.y != height ||
		sprite[index.constant].texLeftTop.x != texPos.x ||
		sprite[index.constant].texLeftTop.y != texPos.y ||
		sprite[index.constant].texSize.x != texSize.x ||
		sprite[index.constant].texSize.y != texSize.y ||
		sprite[index.constant].anchorpoint.x != anchorpoint.x ||
		sprite[index.constant].anchorpoint.y != anchorpoint.y)
	{
		sprite[index.constant].size = { width, height };
		sprite[index.constant].texLeftTop = texPos;
		sprite[index.constant].texSize = texSize;
		sprite[index.constant].anchorpoint = anchorpoint;

		enum Corner { LB, LT, RB, RT };

		static SpriteVertex vert[] = {
			{{}, { 0.0f, 1.0f }},
			{{}, { 0.0f, 0.0f }},
			{{}, { 1.0f, 1.0f }},
			{{}, { 1.0f, 0.0f }},
		};

		float left = (0.0f - sprite[index.constant].anchorpoint.x) * sprite[index.constant].size.x;
		float right = (1.0f - sprite[index.constant].anchorpoint.x) * sprite[index.constant].size.x;
		float top = (0.0f - sprite[index.constant].anchorpoint.y) * sprite[index.constant].size.y;
		float bottom = (1.0f - sprite[index.constant].anchorpoint.y) * sprite[index.constant].size.y;

		vert[LB].pos = {  left, bottom, 0.0f };
		vert[LT].pos = {  left,    top, 0.0f };
		vert[RB].pos = { right, bottom, 0.0f };
		vert[RT].pos = { right,    top, 0.0f };

		// �e�N�X�`���f�[�^�擾
		D3D12_RESOURCE_DESC resDesc = textrueData[index.textrue].texbuff->GetDesc();

		float texLeft = sprite[index.constant].texLeftTop.x / resDesc.Width;
		float texRight = (sprite[index.constant].texLeftTop.x + sprite[index.constant].texSize.x) / resDesc.Width;
		float texTop = sprite[index.constant].texLeftTop.y / resDesc.Height;
		float texBottom = (sprite[index.constant].texLeftTop.y + sprite[index.constant].texSize.y) / resDesc.Height;

		vert[LB].uv = {  texLeft, texBottom };
		vert[LT].uv = {  texLeft,    texTop };
		vert[RB].uv = { texRight, texBottom };
		vert[RT].uv = { texRight,    texTop };

		// ���_�o�b�t�@�ւ̃f�[�^�]��
		SpriteVertex* vertexMap = nullptr;
		sprite[index.constant].vertBuff->Map(0, nullptr, (void**)&vertexMap);
		memcpy(vertexMap, vert, sizeof(vert));
		sprite[index.constant].vertBuff->Unmap(0, nullptr);
	}

	sprite[index.constant].color = color;
	sprite[index.constant].pos = { posX, posY, 0 };
	sprite[index.constant].rotation = angle;

	sprite[index.constant].matWorld = XMMatrixIdentity();
	sprite[index.constant].matWorld *=
		XMMatrixRotationZ(XMConvertToRadians(sprite[index.constant].rotation));
	sprite[index.constant].matWorld *=
		XMMatrixTranslation(
			sprite[index.constant].pos.x,
			sprite[index.constant].pos.y,
			sprite[index.constant].pos.z);

	if (parent != -1)
	{
		sprite[index.constant].matWorld *= sprite[parent].matWorld;
	}

	SpriteConstBufferData* constMap = nullptr;
	sprite[index.constant].constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = sprite[index.constant].color;
	constMap->mat = sprite[index.constant].matWorld *
		spriteData.matProjection[CommonData::Projection::ORTHOGRAPHIC];
	sprite[index.constant].constBuff->Unmap(0, nullptr);

	// �f�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap* ppHeaps[] = { texCommonData.descHeap.Get() };
	w->cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// �萔�o�b�t�@�r���[���Z�b�g
	w->cmdList->SetGraphicsRootConstantBufferView(0, sprite[index.constant].constBuff->GetGPUVirtualAddress());
	w->cmdList->SetGraphicsRootDescriptorTable(1, textrueData[index.textrue].gpuDescHandle);

	// ���_�o�b�t�@�̐ݒ�
	w->cmdList->IASetVertexBuffers(0, 1, &sprite[index.constant].vbView);
	// �`��R�}���h
	w->cmdList->DrawInstanced(4, 1, 0, 0);

#pragma endregion

	return index.constant;
}

void LoadTex::LoopEnd()
{
	spriteCount = -1;
}
