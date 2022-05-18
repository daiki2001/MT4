#include "./Header/DirectDrawing.h"
#include <string>
#include "./Math/Collision/BaseCollider.h"
#include "./Math/Collision/CollisionManager.h"
#include "./Header/SafeDelete.h"

#define EoF -1 // End of Function

CommonData::CommonData() :
	pipelinestate{},
	rootsignature(nullptr)
{
	matProjection[CommonData::Projection::ORTHOGRAPHIC] = DirectX::XMMatrixIdentity();
	matProjection[CommonData::Projection::PERSPECTIVE] = DirectX::XMMatrixIdentity();
}

Object::Object() :
	polygonData(-1),
	constBuff{},
	position(0.0f, 0.0f, 0.0f),
	rotation(DirectX::XMMatrixIdentity()),
	scale(1.0f, 1.0f, 1.0f),
	color(1.0f, 1.0f, 1.0f, 1.0f),
	matWorld(DirectX::XMMatrixIdentity()),
	parent(nullptr),
	name(nullptr),
	collider(nullptr)
{
}

Object::~Object()
{
	if (collider != nullptr)
	{
		// �R���C�_�[�̓o�^����������
		CollisionManager::Get()->RemoveCollider(collider);
	}

	safe_delete(parent);
	safe_delete(collider);
}

void Object::Init()
{
	name = typeid(*this).name();
}

void Object::Update()
{
	// �����蔻��X�V
	if (collider != nullptr)
	{
		collider->Update();
	}
}

void Object::SetCollider(BaseCollider* collider)
{
	collider->SetObject(this);
	this->collider = collider;
	// �R���C�_�[��o�^����
	CollisionManager::Get()->AddCollider(collider);
	// �R���C�_�[���X�V���Ă���
	collider->Update();
}

/*static�ϐ��̏�����*/
CommonData DirectDrawing::spriteData = {};
DirectDrawing::vector<Sprite> DirectDrawing::sprite = {};
DirectDrawing::vector<DirectDrawing::IndexData> DirectDrawing::spriteIndex = {};
size_t DirectDrawing::blendMode = BLENDMODE_NOBLEND;
bool DirectDrawing::isDepthWriteBan = false;

void DirectDrawing::DataClear()
{
	sprite.clear();
	spriteIndex.clear();
	vertices.clear();
	obj.clear();
	objIndex.clear();
	objSubset.clear();
}

DirectDrawing::DirectDrawing(const DirectXInit* w) :
	w(w),
	vertices{},
	vertMap(nullptr),
	obj{},
	objIndex{},
	basicDescHeap{},
	srvHandle{},
	nullBufferCount(0),
	vsBlob{},
	psBlob{},
	errorBlob{},
	rootSigBlob{},
	nearClip(0.1f),
	farClip(1000.0f)
{
	using namespace DirectX;

	float winW = (float)w->windowWidth;
	float winH = (float)w->windowHeight;

	for (size_t i = 0; i < sizeof(objectData) / sizeof(objectData[0]); i++)
	{
		if (i >= sizeof(materialData) / sizeof(materialData[0]))
		{
			break;
		}

		objectData[i] = {};
		objectData[i].matProjection[CommonData::Projection::ORTHOGRAPHIC] =
			XMMatrixOrthographicOffCenterLH(0.0f, winW, winH, 0.0f, 0.0f, 1.0f);
		objectData[i].matProjection[CommonData::Projection::PERSPECTIVE] =
			XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), winW / winH, 0.1f, 1000.0f);

		materialData[i] = objectData[i];
	}

	spriteData = {};
	spriteData.matProjection[CommonData::Projection::ORTHOGRAPHIC] =
		XMMatrixOrthographicOffCenterLH(0.0f, winW, winH, 0.0f, 0.0f, 1.0f);

	Init();
}

DirectDrawing::~DirectDrawing()
{
	DataClear();
}

HRESULT DirectDrawing::Init()
{
	HRESULT hr = S_FALSE;

	hr = DrawingInit();
	if (FAILED(hr))
	{
		return hr;
	}

	hr = MaterialInit();
	if (FAILED(hr))
	{
		return hr;
	}

	hr = SpriteDrawingInit();
	if (FAILED(hr))
	{
		return hr;
	}

	CreateNullConstant(XMFLOAT3(), XMMATRIX(), XMFLOAT3(1, 1, 1));

	return hr;
}

HRESULT DirectDrawing::DrawingInit()
{
	HRESULT hr;

	if (isDrawingInit == true)
	{
		return S_OK;
	}

	isDrawingInit = true;

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	hr = D3DCompileFromFile(
		L"./lib/Shaders/ObjectVS.hlsl", //�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,               //�C���N���[�h�\�ɂ���
		"main",                                          //�G���g���[�|�C���g��
		"vs_5_0",                                        //�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //�f�o�b�O�p�ݒ�
		0,
		&vsBlob,
		&errorBlob);

	if (FAILED(hr))
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
					errorBlob->GetBufferSize(),
					errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	hr = D3DCompileFromFile(
		L"./lib/Shaders/ObjectPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob,
		&errorBlob);

	if (FAILED(hr))
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
					errorBlob->GetBufferSize(),
					errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		// xyz���W
		{
			"POSITION",                                 //�Z�}���e�B�b�N��
			0,                                          //�����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X
			DXGI_FORMAT_R32G32B32_FLOAT,                //�v�f���ƃr�b�g����\��
			0,                                          //���̓X���b�g�C���f�b�N�X
			D3D12_APPEND_ALIGNED_ELEMENT,               //�f�[�^�̃I�t�Z�b�g�l
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, //���̓f�[�^���
			0                                           //��x�ɕ`�悷��C���X�^���X��
		},
		// �@���x�N�g��
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		// uv���W
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	// �f�X�N���v�^�e�[�u���̐ݒ�
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV = {}; //�f�X�N���v�^�e�[�u���̐ݒ�(�V�F�[�_���\�[�X)
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	// ���[�g�p�����[�^�̐ݒ�
	CD3DX12_ROOT_PARAMETER rootparams[2]{}; //���[�g�p�����[�^�̐ݒ�
	rootparams[0].InitAsConstantBufferView(0);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc  {};
	rootSignatureDesc.Init_1_0(
		_countof(rootparams),
		rootparams,
		1,
		&samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	hr = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob
	);
	if (FAILED(hr))
	{
		return hr;
	}

	for (size_t i = 0; i < sizeof(objectData) / sizeof(objectData[0]); i++)
	{
		hr = w->dev->CreateRootSignature(
			0,
			rootSigBlob->GetBufferPointer(),
			rootSigBlob->GetBufferSize(),
			IID_PPV_ARGS(&objectData[i].rootsignature)
		);
		if (FAILED(hr))
		{
			return hr;
		}
	}

	for (size_t i = 0; i < sizeof(objectData) / sizeof(objectData[0]); i++)
	{
		for (size_t j = 0; j < sizeof(CommonData::pipelinestate) / sizeof(CommonData::pipelinestate[0]); j++)
		{
			// �O���t�B�b�N�X�p�C�v���C���ݒ�
			D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

			// ���_�V�F�[�_�A�s�N�Z���V�F�[�_���p�C�v���C���ɐݒ�
			gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
			gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
			// �T���v���}�X�N�ƃ��X�^���C�U�X�e�[�g�̐ݒ�
			gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; //�W���ݒ�
			gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; //�w�ʃJ�����O���Ȃ�
			// �u�����h�f�X�N�̐ݒ�
			D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = gpipeline.BlendState.RenderTarget[0];
			blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //�W���ݒ�

			switch (j)
			{
			case BLENDMODE_NOBLEND:
				/*�m�[�u�����h�p�̐ݒ�*/
				blendDesc.BlendEnable = false; //�u�����h�𖳌��ɂ���
				break;
			case BLENDMODE_ALPHA:
				/*���u�����h�p�̐ݒ�*/
				blendDesc.BlendOp = D3D12_BLEND_OP_ADD;          //���Z
				blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;      //�\�[�X�̃A���t�@�l
				blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; //1.0f - �\�[�X�̃A���t�@�l
				goto BaseBlendState;
			case BLENDMODE_ADD:
				/*���Z�����p�̐ݒ�*/
				blendDesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
				blendDesc.SrcBlend = D3D12_BLEND_ONE;   //�\�[�X�̒l�� 100% �g��
				blendDesc.DestBlend = D3D12_BLEND_ONE;  //�f�X�g�̒l�� 100% �g��
				goto BaseBlendState;
			case BLENDMODE_SUB:
				/*���Z�����p�̐ݒ�*/
				blendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT; //�f�X�g����\�[�X�����Z
				blendDesc.SrcBlend = D3D12_BLEND_ONE;            //�\�[�X�̒l�� 100% �g��
				blendDesc.DestBlend = D3D12_BLEND_ONE;           //�f�X�g�̒l�� 100% �g��
				goto BaseBlendState;
			case BLENDMODE_INV:
				/*�F���]�p�̐ݒ�*/
				blendDesc.BlendOp = D3D12_BLEND_OP_ADD;          //���Z
				blendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR; //1.0f - �f�X�g�J���[�̒l
				blendDesc.DestBlend = D3D12_BLEND_ZERO;          //�g��Ȃ�
				goto BaseBlendState;
			default:
BaseBlendState:
				/*���ʐݒ�*/
				blendDesc.BlendEnable = true;                //�u�����h��L���ɂ���
				blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; //���Z
				blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;   //�\�[�X�̒l�� 100% �g��
				blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO; //�f�X�g�̒l��   0% �g��
				break;
			}
			// �f�v�X�X�e���V���X�e�[�g�̐ݒ�
			gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			if (i == 1)
			{
				gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; //�f�v�X�̏㏑���֎~
			}
			gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT; //�[�x�l�t�H�[�}�b�g
			// ���_���C�A�E�g�̐ݒ�
			gpipeline.InputLayout.pInputElementDescs = inputLayout;
			gpipeline.InputLayout.NumElements = _countof(inputLayout);
			// �}�`�̌`����O�p�`�ɐݒ�
			gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			// ���̑��̐ݒ�
			gpipeline.NumRenderTargets = 1; //�`��Ώۂ�1��
			gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; //0~255�w���RGBA
			gpipeline.SampleDesc.Count = 1; //1�s�N�Z���ɂ�1��T���v�����O

			// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
			gpipeline.pRootSignature = objectData[i].rootsignature.Get();

			hr = w->dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&objectData[i].pipelinestate[j]));
		}
	}

	return hr;
}

HRESULT DirectDrawing::MaterialInit()
{
	HRESULT hr;

	if (isMaterialInit == true)
	{
		return S_OK;
	}

	isMaterialInit = true;

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	hr = D3DCompileFromFile(
		L"./lib/Shaders/MaterialVS.hlsl", //�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,               //�C���N���[�h�\�ɂ���
		"main",                                          //�G���g���[�|�C���g��
		"vs_5_0",                                        //�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //�f�o�b�O�p�ݒ�
		0,
		&vsBlob,
		&errorBlob);

	if (FAILED(hr))
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
					errorBlob->GetBufferSize(),
					errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	hr = D3DCompileFromFile(
		L"./lib/Shaders/MaterialPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob,
		&errorBlob);

	if (FAILED(hr))
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
					errorBlob->GetBufferSize(),
					errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		// xyz���W
		{
			"POSITION",                                 //�Z�}���e�B�b�N��
			0,                                          //�����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X
			DXGI_FORMAT_R32G32B32_FLOAT,                //�v�f���ƃr�b�g����\��
			0,                                          //���̓X���b�g�C���f�b�N�X
			D3D12_APPEND_ALIGNED_ELEMENT,               //�f�[�^�̃I�t�Z�b�g�l
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, //���̓f�[�^���
			0                                           //��x�ɕ`�悷��C���X�^���X��
		},
		// �@���x�N�g��
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		// uv���W
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	// �f�X�N���v�^�e�[�u���̐ݒ�
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV = {}; //�f�X�N���v�^�e�[�u���̐ݒ�(�V�F�[�_���\�[�X)
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	// ���[�g�p�����[�^�̐ݒ�
	CD3DX12_ROOT_PARAMETER rootparams[3]{}; //���[�g�p�����[�^�̐ݒ�
	rootparams[0].InitAsConstantBufferView(0);
	rootparams[1].InitAsConstantBufferView(1);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init_1_0(
		_countof(rootparams),
		rootparams,
		1,
		&samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	// �o�[�W������������̃V���A���C�Y
	hr = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob
	);
	if (FAILED(hr))
	{
		return hr;
	}

	for (size_t i = 0; i < sizeof(materialData) / sizeof(materialData[0]); i++)
	{
		hr = w->dev->CreateRootSignature(
			0,
			rootSigBlob->GetBufferPointer(),
			rootSigBlob->GetBufferSize(),
			IID_PPV_ARGS(&materialData[i].rootsignature)
		);
		if (FAILED(hr))
		{
			return hr;
		}
	}

	for (size_t i = 0; i < sizeof(materialData) / sizeof(materialData[0]); i++)
	{
		for (size_t j = 0; j < sizeof(CommonData::pipelinestate) / sizeof(CommonData::pipelinestate[0]); j++)
		{
			// �O���t�B�b�N�X�p�C�v���C���ݒ�
			D3D12_GRAPHICS_PIPELINE_STATE_DESC materialPipeline = {};

			// ���_�V�F�[�_�A�s�N�Z���V�F�[�_���p�C�v���C���ɐݒ�
			materialPipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
			materialPipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
			// �T���v���}�X�N�ƃ��X�^���C�U�X�e�[�g�̐ݒ�
			materialPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; //�W���ݒ�
			materialPipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			//materialPipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; //�w�ʃJ�����O���Ȃ�
#pragma region SetBlendState
			D3D12_RENDER_TARGET_BLEND_DESC& materialBlendDesc(materialPipeline.BlendState.RenderTarget[0]);
			materialBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //�W���ݒ�

			switch (j)
			{
			case BLENDMODE_NOBLEND:
				/*�m�[�u�����h�p�̐ݒ�*/
				materialBlendDesc.BlendEnable = false; //�u�����h�𖳌��ɂ���
				break;
			case BLENDMODE_ALPHA:
				/*���u�����h�p�̐ݒ�*/
				materialBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;          //���Z
				materialBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;      //�\�[�X�̃A���t�@�l
				materialBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; //1.0f - �\�[�X�̃A���t�@�l
				goto MaterialBaseBlend;
			case BLENDMODE_ADD:
				/*���Z�����p�̐ݒ�*/
				materialBlendDesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
				materialBlendDesc.SrcBlend = D3D12_BLEND_ONE;   //�\�[�X�̒l�� 100% �g��
				materialBlendDesc.DestBlend = D3D12_BLEND_ONE;  //�f�X�g�̒l�� 100% �g��
				goto MaterialBaseBlend;
			case BLENDMODE_SUB:
				/*���Z�����p�̐ݒ�*/
				materialBlendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT; //�f�X�g����\�[�X�����Z
				materialBlendDesc.SrcBlend = D3D12_BLEND_ONE;            //�\�[�X�̒l�� 100% �g��
				materialBlendDesc.DestBlend = D3D12_BLEND_ONE;           //�f�X�g�̒l�� 100% �g��
				goto MaterialBaseBlend;
			case BLENDMODE_INV:
				/*�F���]�p�̐ݒ�*/
				materialBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;          //���Z
				materialBlendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR; //1.0f - �f�X�g�J���[�̒l
				materialBlendDesc.DestBlend = D3D12_BLEND_ZERO;          //�g��Ȃ�
				goto MaterialBaseBlend;
			default:
MaterialBaseBlend:
				/*���ʐݒ�*/
				materialBlendDesc.BlendEnable = true;                //�u�����h��L���ɂ���
				materialBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; //���Z
				materialBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;   //�\�[�X�̒l�� 100% �g��
				materialBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO; //�f�X�g�̒l��   0% �g��
				break;
			}
#pragma endregion
			// �f�v�X�X�e���V���X�e�[�g�̐ݒ�
			materialPipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			if (i == 1)
			{
				materialPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; //�f�v�X�̏㏑���֎~
			}
			materialPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT; //�[�x�l�t�H�[�}�b�g
			// ���_���C�A�E�g�̐ݒ�
			materialPipeline.InputLayout.pInputElementDescs = inputLayout;
			materialPipeline.InputLayout.NumElements = _countof(inputLayout);
			// �}�`�̌`����O�p�`�ɐݒ�
			materialPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			// ���̑��̐ݒ�
			materialPipeline.NumRenderTargets = 1; //�`��Ώۂ�1��
			materialPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; //0~255�w���RGBA
			materialPipeline.SampleDesc.Count = 1; //1�s�N�Z���ɂ�1��T���v�����O

			// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
			materialPipeline.pRootSignature = materialData[i].rootsignature.Get();

			hr = w->dev->CreateGraphicsPipelineState(&materialPipeline, IID_PPV_ARGS(&materialData[i].pipelinestate[j]));
		}
	}

	return hr;
}

HRESULT DirectDrawing::SpriteDrawingInit()
{
	HRESULT hr;

	if (isSpriteDrawingInit == true)
	{
		return S_OK;
	}

	isSpriteDrawingInit = true;

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	hr = D3DCompileFromFile(
		L"./lib/Shaders/SpriteVS.hlsl",       //�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,               //�C���N���[�h�\�ɂ���
		"main",                                          //�G���g���[�|�C���g��
		"vs_5_0",                                        //�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //�f�o�b�O�p�ݒ�
		0,
		&vsBlob,
		&errorBlob);

	if (FAILED(hr))
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
					errorBlob->GetBufferSize(),
					errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	hr = D3DCompileFromFile(
		L"./lib/Shaders/SpritePS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob,
		&errorBlob);

	if (FAILED(hr))
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
					errorBlob->GetBufferSize(),
					errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		// xyz���W
		{
			"POSITION",                                 //�Z�}���e�B�b�N��
			0,                                          //�����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X
			DXGI_FORMAT_R32G32B32_FLOAT,                //�v�f���ƃr�b�g����\��
			0,                                          //���̓X���b�g�C���f�b�N�X
			D3D12_APPEND_ALIGNED_ELEMENT,               //�f�[�^�̃I�t�Z�b�g�l
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, //���̓f�[�^���
			0                                           //��x�ɕ`�悷��C���X�^���X��
		},
		// uv���W
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	// �f�X�N���v�^�e�[�u���̐ݒ�
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV = {}; //�f�X�N���v�^�e�[�u���̐ݒ�(�V�F�[�_���\�[�X)
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	// ���[�g�p�����[�^�̐ݒ�
	CD3DX12_ROOT_PARAMETER rootparams[2]{}; //���[�g�p�����[�^�̐ݒ�
	rootparams[0].InitAsConstantBufferView(0);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init_1_0(
		_countof(rootparams),
		rootparams,
		1,
		&samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	hr = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob
	);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = w->dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&spriteData.rootsignature)
	);
	if (FAILED(hr))
	{
		return hr;
	}

	for (size_t i = 0; i < sizeof(CommonData::pipelinestate) / sizeof(CommonData::pipelinestate[0]); i++)
	{
		// �O���t�B�b�N�X�p�C�v���C���ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC spritePipeline = {};

		// ���_�V�F�[�_�A�s�N�Z���V�F�[�_���p�C�v���C���ɐݒ�
		spritePipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
		spritePipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
		// �T���v���}�X�N�ƃ��X�^���C�U�X�e�[�g�̐ݒ�
		spritePipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; //�W���ݒ�
		spritePipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		spritePipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; //�w�ʃJ�����O���Ȃ�
#pragma region SetBlendState
		D3D12_RENDER_TARGET_BLEND_DESC& spriteBlendDesc = spritePipeline.BlendState.RenderTarget[0];
		spriteBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //�W���ݒ�

		switch (i)
		{
		case BLENDMODE_NOBLEND:
			/*�m�[�u�����h�p�̐ݒ�*/
			spriteBlendDesc.BlendEnable = false; //�u�����h�𖳌��ɂ���
			break;
		case BLENDMODE_ALPHA:
			/*���u�����h�p�̐ݒ�*/
			spriteBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;          //���Z
			spriteBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;      //�\�[�X�̃A���t�@�l
			spriteBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; //1.0f - �\�[�X�̃A���t�@�l
			goto SpriteBaseBlend;
		case BLENDMODE_ADD:
			/*���Z�����p�̐ݒ�*/
			spriteBlendDesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
			spriteBlendDesc.SrcBlend = D3D12_BLEND_ONE;   //�\�[�X�̒l�� 100% �g��
			spriteBlendDesc.DestBlend = D3D12_BLEND_ONE;  //�f�X�g�̒l�� 100% �g��
			goto SpriteBaseBlend;
		case BLENDMODE_SUB:
			/*���Z�����p�̐ݒ�*/
			spriteBlendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT; //�f�X�g����\�[�X�����Z
			spriteBlendDesc.SrcBlend = D3D12_BLEND_ONE;            //�\�[�X�̒l�� 100% �g��
			spriteBlendDesc.DestBlend = D3D12_BLEND_ONE;           //�f�X�g�̒l�� 100% �g��
			goto SpriteBaseBlend;
		case BLENDMODE_INV:
			/*�F���]�p�̐ݒ�*/
			spriteBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;          //���Z
			spriteBlendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR; //1.0f - �f�X�g�J���[�̒l
			spriteBlendDesc.DestBlend = D3D12_BLEND_ZERO;          //�g��Ȃ�
			goto SpriteBaseBlend;
		default:
SpriteBaseBlend:
			/*���ʐݒ�*/
			spriteBlendDesc.BlendEnable = true;                //�u�����h��L���ɂ���
			spriteBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; //���Z
			spriteBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;   //�\�[�X�̒l�� 100% �g��
			spriteBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO; //�f�X�g�̒l��   0% �g��
			break;
		}
#pragma endregion
		// �f�v�X�X�e���V���X�e�[�g�̐ݒ�
		spritePipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		spritePipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; //��ɏ㏑��
		spritePipeline.DepthStencilState.DepthEnable = false;                      //�[�x�e�X�g�����Ȃ�
		spritePipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT; //�[�x�l�t�H�[�}�b�g
		// ���_���C�A�E�g�̐ݒ�
		spritePipeline.InputLayout.pInputElementDescs = inputLayout;
		spritePipeline.InputLayout.NumElements = _countof(inputLayout);
		// �}�`�̌`����O�p�`�ɐݒ�
		spritePipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// ���̑��̐ݒ�
		spritePipeline.NumRenderTargets = 1; //�`��Ώۂ�1��
		spritePipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; //0~255�w���RGBA
		spritePipeline.SampleDesc.Count = 1; //1�s�N�Z���ɂ�1��T���v�����O

		// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
		spritePipeline.pRootSignature = spriteData.rootsignature.Get();

		hr = w->dev->CreateGraphicsPipelineState(&spritePipeline, IID_PPV_ARGS(&spriteData.pipelinestate[i]));
	}

	return hr;
}

int DirectDrawing::CreateVertexAndIndexBuffer()
{
	HRESULT hr = S_FALSE;

	// 'vertexNum'��0�ȉ��Ȃ琶�������Ƀ��^�[������
	if (vertices.size() <= 0 || vertices[vertices.size() - 1].vertices.size() <= 0)
	{
		return EoF;
	}

#pragma region VertexBuffer

	// ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB;
	sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices[vertices.size() - 1].vertices.size());

	hr = w->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertices[vertices.size() - 1].vertBuff));
	if (FAILED(hr))
	{
		return EoF;
	}

	vertices[vertices.size() - 1].vbView.BufferLocation = vertices[vertices.size() - 1].vertBuff->GetGPUVirtualAddress();
	vertices[vertices.size() - 1].vbView.SizeInBytes = sizeVB;
	vertices[vertices.size() - 1].vbView.StrideInBytes = sizeof(Vertex);

#pragma endregion //VertexBuffer

	if (vertices[vertices.size() - 1].indices.size() <= 0)
	{
		for (unsigned short i = 0; i < vertices[vertices.size() - 1].vertices.size(); i++)
		{
			vertices[vertices.size() - 1].indices.push_back(i);
		}
	}

#pragma region IndexBuffer

	//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB;
	sizeIB = static_cast<UINT>(sizeof(unsigned short) * vertices[vertices.size() - 1].indices.size());

	hr = w->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertices[vertices.size() - 1].indexBuff));
	if (FAILED(hr))
	{
		return EoF;
	}

	// GPU��̃o�b�t�@�ɑΉ��������z���������擾
	static unsigned short* indexMap = nullptr;
	hr = vertices[vertices.size() - 1].indexBuff->Map(0, nullptr, (void**)&indexMap);

	// �S�C���f�b�N�X�ɑ΂���
	for (size_t i = 0; i < vertices[vertices.size() - 1].indices.size(); i++)
	{
		indexMap[i] = vertices[vertices.size() - 1].indices[i]; //�C���f�b�N�X���R�s�[
	}

	// �q���������
	vertices[vertices.size() - 1].indexBuff->Unmap(0, nullptr);

	vertices[vertices.size() - 1].ibView.BufferLocation = vertices[vertices.size() - 1].indexBuff->GetGPUVirtualAddress();
	vertices[vertices.size() - 1].ibView.Format = DXGI_FORMAT_R16_UINT;
	vertices[vertices.size() - 1].ibView.SizeInBytes = sizeIB;

#pragma endregion //IndexBuffer

#pragma region NormalVector

	for (size_t i = 0; i < vertices[vertices.size() - 1].indices.size() / 3; i++)
	{
		using namespace DirectX;

		// �O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
		unsigned short temp0 = vertices[vertices.size() - 1].indices[i * 3 + 0];
		unsigned short temp1 = vertices[vertices.size() - 1].indices[i * 3 + 1];
		unsigned short temp2 = vertices[vertices.size() - 1].indices[i * 3 + 2];
		// �O�p�`���\�����钸�_���W���x�N�g���ɑ��
		XMVECTOR p0 = XMLoadFloat3(&vertices[vertices.size() - 1].vertices[temp0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[vertices.size() - 1].vertices[temp1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[vertices.size() - 1].vertices[temp2].pos);
		// p0��p1�x�N�g���Ap0��p2�x�N�g�����v�Z(�x�N�g���̌��Z)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		// �O�ς͗������琂���ȃx�N�g��
		XMVECTOR normal = XMVector3Cross(v1, v2);
		// ���K��
		normal = XMVector3Normalize(normal);
		// ���߂��@���𒸓_�f�[�^�ɑ��
		XMStoreFloat3(&vertices[vertices.size() - 1].vertices[temp0].normal, normal);
		XMStoreFloat3(&vertices[vertices.size() - 1].vertices[temp1].normal, normal);
		XMStoreFloat3(&vertices[vertices.size() - 1].vertices[temp2].normal, normal);
	}

	hr = vertices[vertices.size() - 1].vertBuff->Map(0, nullptr, (void**)&vertMap);

	// �S���_�ɑ΂���
	for (UINT i = 0; i < vertices[vertices.size() - 1].vertices.size(); i++)
	{
		vertMap[i] = vertices[vertices.size() - 1].vertices[i]; //���W���R�s�[
	}

	// �}�b�v������
	vertices[vertices.size() - 1].vertBuff->Unmap(0, nullptr);

#pragma endregion //NormalVector

	return (int)(vertices.size() - 1);
}

HRESULT DirectDrawing::CreateConstBuffer(int* objIndex)
{
	HRESULT hr = S_FALSE;

	obj.push_back({});

	hr = w->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xFF) & ~0xFF), //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&obj[obj.size() - 1].constBuff));
	if (FAILED(hr))
	{
		return hr;
	}
	hr = w->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(MaterialConstBufferData) + 0xFF) & ~0xFF), //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&obj[obj.size() - 1].materialConstBuff));
	if (FAILED(hr))
	{
		return hr;
	}

	hr = BasicDescHeapInit();
	if (FAILED(hr))
	{
		return hr;
	}

	// �萔�o�b�t�@�r���[����
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = obj[obj.size() - 1].constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)obj[obj.size() - 1].constBuff->GetDesc().Width;
	w->dev->CreateConstantBufferView(
		&cbvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			basicDescHeap->GetCPUDescriptorHandleForHeapStart(),
			*objIndex,
			w->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	*objIndex = (int)(obj.size() - 1);
	return hr;
}

int DirectDrawing::CreateNullConstant(const XMFLOAT3& pos, const XMMATRIX& rota, const XMFLOAT3& scale)
{
	obj.push_back({});

	using namespace DirectX;

	XMMATRIX rot = XMMatrixIdentity();

	obj[obj.size() - 1].position = pos;
	obj[obj.size() - 1].rotation = rota;
	obj[obj.size() - 1].scale = scale;
	obj[obj.size() - 1].matWorld = XMMatrixIdentity();

	obj[obj.size() - 1].matWorld *= XMMatrixScaling(
		obj[obj.size() - 1].scale.x,
		obj[obj.size() - 1].scale.y,
		obj[obj.size() - 1].scale.z);
	obj[obj.size() - 1].matWorld *= obj[obj.size() - 1].rotation;
	obj[obj.size() - 1].matWorld *= XMMatrixTranslation(
		obj[obj.size() - 1].position.x,
		obj[obj.size() - 1].position.y,
		obj[obj.size() - 1].position.z);

	nullBufferCount++;

	return (int)obj.size() - 1;
}

void DirectDrawing::UpdataConstant(
	const XMFLOAT3& pos, const DirectX::XMMATRIX& rota, const XMFLOAT3& scale,
	const DirectX::XMFLOAT4& color, const int& objectIndex, const int& polygonData,
	Object* parent)
{
	if ((objectIndex < 0 || (size_t)objectIndex >= obj.size()) ||
		(polygonData < -1 || (polygonData != -1 && (size_t)polygonData >= vertices.size()))/* ||
		(parent < -1 || (parent != -1 && (size_t)parent >= obj.size()))*/)
	{
		return;
	}

	using namespace DirectX;
	using namespace Engine::Math;

	bool dirtyFlag = false; //�_�[�e�B�t���O

	// ���_��񂪈������X�V
	if (obj[objectIndex].polygonData != polygonData)
	{
		obj[objectIndex].polygonData = polygonData;
	}

	// ���W���������X�V���A�_�[�e�B�t���O��true�ɂ���
	if (obj[objectIndex].position.x != pos.x ||
		obj[objectIndex].position.y != pos.y ||
		obj[objectIndex].position.z != pos.z)
	{
		obj[objectIndex].position = pos;
		dirtyFlag = true;
	}

	// ��]�s�񂪈������X�V���A�_�[�e�B�t���O��true�ɂ���
	if ((obj[objectIndex].rotation.r[0].m128_f32[0] != rota.r[0].m128_f32[0]) ||
		(obj[objectIndex].rotation.r[0].m128_f32[1] != rota.r[0].m128_f32[1]) ||
		(obj[objectIndex].rotation.r[0].m128_f32[2] != rota.r[0].m128_f32[2]) ||
		(obj[objectIndex].rotation.r[0].m128_f32[3] != rota.r[0].m128_f32[3]) ||
		(obj[objectIndex].rotation.r[1].m128_f32[0] != rota.r[0].m128_f32[0]) ||
		(obj[objectIndex].rotation.r[1].m128_f32[1] != rota.r[0].m128_f32[1]) ||
		(obj[objectIndex].rotation.r[1].m128_f32[2] != rota.r[0].m128_f32[2]) ||
		(obj[objectIndex].rotation.r[1].m128_f32[3] != rota.r[0].m128_f32[3]) ||
		(obj[objectIndex].rotation.r[2].m128_f32[0] != rota.r[0].m128_f32[0]) ||
		(obj[objectIndex].rotation.r[2].m128_f32[1] != rota.r[0].m128_f32[1]) ||
		(obj[objectIndex].rotation.r[2].m128_f32[2] != rota.r[0].m128_f32[2]) ||
		(obj[objectIndex].rotation.r[2].m128_f32[3] != rota.r[0].m128_f32[3]) ||
		(obj[objectIndex].rotation.r[3].m128_f32[0] != rota.r[0].m128_f32[0]) ||
		(obj[objectIndex].rotation.r[3].m128_f32[1] != rota.r[0].m128_f32[1]) ||
		(obj[objectIndex].rotation.r[3].m128_f32[2] != rota.r[0].m128_f32[2]) ||
		(obj[objectIndex].rotation.r[3].m128_f32[3] != rota.r[0].m128_f32[3]))
	{
		obj[objectIndex].rotation = rota;
		dirtyFlag = true;
	}

	// �X�P�[�����������X�V���A�_�[�e�B�t���O��true�ɂ���
	if (obj[objectIndex].scale.x != scale.x ||
		obj[objectIndex].scale.y != scale.y ||
		obj[objectIndex].scale.z != scale.z)
	{
		obj[objectIndex].scale = scale;
		dirtyFlag = true;
	}

	// �e�̗v�f�ԍ����������X�V���A�_�[�e�B�t���O��true�ɂ���
	if (obj[objectIndex].parent != nullptr)
	{
		obj[objectIndex].parent = parent;
		dirtyFlag = true;
	}

	// �_�[�e�B�t���O��true��������A���[���h�s����X�V
	if (dirtyFlag == true)
	{
		obj[objectIndex].matWorld = XMMatrixIdentity();
		obj[objectIndex].matWorld *= XMMatrixScaling(
			obj[objectIndex].scale.x,
			obj[objectIndex].scale.y,
			obj[objectIndex].scale.z);
		obj[objectIndex].matWorld *= obj[objectIndex].rotation;
		obj[objectIndex].matWorld *= XMMatrixTranslation(
			obj[objectIndex].position.x,
			obj[objectIndex].position.y,
			obj[objectIndex].position.z);

		if (obj[objectIndex].parent != nullptr)
		{
			obj[objectIndex].matWorld *= parent->matWorld;
		}
	}

	// �F���������X�V
	if (obj[objectIndex].color != color)
	{
		obj[objectIndex].color = color;
	}
}

int DirectDrawing::CreateSprite()
{
	using namespace DirectX;

	HRESULT hr = S_FALSE;
	sprite.push_back({});

	static SpriteVertex vert[] = {
		{{}, { 0.0f, 1.0f }},
		{{}, { 0.0f, 0.0f }},
		{{}, { 1.0f, 1.0f }},
		{{}, { 1.0f, 0.0f }},
	};

	// ���_�o�b�t�@�̐���
	hr = w->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vert)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite[sprite.size() - 1].vertBuff));
	if (FAILED(hr))
	{
		return EoF;
	}

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	SpriteVertex* vertexMap = nullptr;
	hr = sprite[sprite.size() - 1].vertBuff->Map(0, nullptr, (void**)&vertexMap);
	memcpy(vertexMap, vert, sizeof(vert));
	sprite[sprite.size() - 1].vertBuff->Unmap(0, nullptr);

	// ���_�o�b�t�@�r���[�̐���
	sprite[sprite.size() - 1].vbView.BufferLocation = sprite[sprite.size() - 1].vertBuff->GetGPUVirtualAddress();
	sprite[sprite.size() - 1].vbView.SizeInBytes = sizeof(vert);
	sprite[sprite.size() - 1].vbView.StrideInBytes = sizeof(SpriteVertex);

	// �萔�o�b�t�@�̐���
	hr = w->dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xFF) & ~0xFF), //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite[sprite.size() - 1].constBuff));
	if (FAILED(hr))
	{
		return EoF;
	}

	// �萔�o�b�t�@�Ƀf�[�^�]��
	SpriteConstBufferData* constMap = nullptr;
	hr = sprite[sprite.size() - 1].constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = XMFLOAT4(1, 1, 1, 1); //�F�w��(RGBA)
	constMap->mat = spriteData.matProjection[CommonData::Projection::ORTHOGRAPHIC]; //���s���e�s��
	sprite[sprite.size() - 1].constBuff->Unmap(0, nullptr);

	hr = BasicDescHeapInit();
	if (FAILED(hr))
	{
		return EoF;
	}

	return (int)(sprite.size() - 1);
}

HRESULT DirectDrawing::BasicDescHeapInit()
{
	HRESULT hr;

	if (isBasicDescHeapInit == true)
	{
		return S_OK;
	}
	else
	{
		isBasicDescHeapInit = true;

		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{}; //�ݒ�\����

		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //�V�F�[�_���猩����
		descHeapDesc.NumDescriptors = (UINT)(50000); //�萔�o�b�t�@�̐�
		// �f�X�N���v�^�q�[�v�̐���
		hr = w->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

		return hr;
	}
}

void DirectDrawing::BaseDrawGraphics()
{
	// �r���[�|�[�g�̈�̐ݒ�
	w->cmdList->RSSetViewports(
		1,
		&CD3DX12_VIEWPORT(
			0.0f,
			0.0f,
			(float)w->windowWidth,
			(float)w->windowHeight
		)
	);

	// �V�U�[��`�̐ݒ�
	w->cmdList->RSSetScissorRects(
		1,
		&CD3DX12_RECT(
			0,
			0,
			w->windowWidth,
			w->windowHeight
		)
	);
}

void DirectDrawing::BaseDrawSpriteGraphics()
{
	// �r���[�|�[�g�̈�̐ݒ�
	w->cmdList->RSSetViewports(
		1,
		&CD3DX12_VIEWPORT(
			0.0f,
			0.0f,
			(float)w->windowWidth,
			(float)w->windowHeight
		)
	);

	// �V�U�[��`�̐ݒ�
	w->cmdList->RSSetScissorRects(
		1,
		&CD3DX12_RECT(
			0,
			0,
			w->windowWidth,
			w->windowHeight
		)
	);

	w->cmdList->SetPipelineState(spriteData.pipelinestate[blendMode].Get());
	w->cmdList->SetGraphicsRootSignature(spriteData.rootsignature.Get());

	// �v���~�e�B�u�`��̐ݒ�
	w->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

int DirectDrawing::SetDrawBlendMode(int blendMode)
{
	if (blendMode < BLENDMODE_NOBLEND || blendMode > BLENDMODE_INV)
	{
		return EoF;
	}

	this->blendMode = blendMode;

	return (int)(this->blendMode);
}

HRESULT DirectDrawing::SetNearFar(float nearClip, float farClip)
{
	using namespace DirectX;

	if (nearClip <= 0.0f || farClip <= 0.0f || nearClip == farClip)
	{
		return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
	}

	this->nearClip = nearClip;
	this->farClip = farClip;

	for (size_t i = 0; i < sizeof(objectData) / sizeof(objectData[0]); i++)
	{
		objectData[i].matProjection[CommonData::Projection::ORTHOGRAPHIC] = XMMatrixOrthographicOffCenterLH(
			0.0f,
			(float)w->windowWidth,
			(float)w->windowHeight,
			0.0f,
			this->nearClip, //�O�[
			this->farClip   //���[
		);

		objectData[i].matProjection[CommonData::Projection::PERSPECTIVE] = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(60.0f),                      //�㉺��p60�x
			(float)w->windowWidth / (float)w->windowHeight, //�A�X�y�N�g��
			this->nearClip, //�O�[
			this->farClip   //���[
		);
	}

	for (size_t i = 0; i < sizeof(materialData) / sizeof(materialData[0]); i++)
	{
		materialData[i].matProjection[CommonData::Projection::ORTHOGRAPHIC] = XMMatrixOrthographicOffCenterLH(
			0.0f,
			(float)w->windowWidth,
			(float)w->windowHeight,
			0.0f,
			this->nearClip, //�O�[
			this->farClip   //���[
		);

		materialData[i].matProjection[CommonData::Projection::PERSPECTIVE] = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(60.0f),                    //�㉺��p60�x
			(float)w->windowWidth / (float)w->windowHeight, //�A�X�y�N�g��
			this->nearClip, //�O�[
			this->farClip   //���[
		);
	}

	return S_OK;
}
