#pragma once
#include "DirectXInit.h"
#include "./Math/EngineMath.h"
#include "./Math/Collision/CollisionInfo.h"
#include <vector>
#include <wrl.h>
#include <d3dx12.h>

/*�V�F�[�_�p*/
#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

/*DirectDrawing�̃o�[�W�����w��*/
#define DIRECTDRAWING_VERSION 0x0001

/*�u�����h���[�h*/
#define BLENDMODE_NOBLEND (0) //�m�[�u�����h
#define BLENDMODE_ALPHA   (1) //���u�����h
#define BLENDMODE_ADD     (2) //���Z����
#define BLENDMODE_SUB     (3) //���Z����
#define BLENDMODE_INV     (4) //�F���]����

class BaseCollider;

// ���_�f�[�^�\����
struct Vertex
{
	DirectX::XMFLOAT3 pos;    //xyz���W
	DirectX::XMFLOAT3 normal; //�@���x�N�g��
	DirectX::XMFLOAT2 uv;     //uv���W
};

// 1�I�u�W�F�N�g�̒��_�����܂Ƃ߂��\����
struct VertexData
{
	/*�z��ɂ��Ďg�����Ƃ��O��*/

private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	std::vector<Vertex> vertices;    //���_�f�[�^
	ComPtr<ID3D12Resource> vertBuff; //���_�o�b�t�@�̐���
	D3D12_VERTEX_BUFFER_VIEW vbView = {}; //���_�o�b�t�@�r���[�̍쐬

	std::vector<unsigned short> indices; //�C���f�b�N�X�f�[�^
	ComPtr<ID3D12Resource> indexBuff;    //�C���f�b�N�X�o�b�t�@�̐ݒ�
	D3D12_INDEX_BUFFER_VIEW ibView = {};      //�C���f�b�N�X�o�b�t�@�r���[�̍쐬
};

// ���_�f�[�^�\����(�X�v���C�g�p)
struct SpriteVertex
{
	DirectX::XMFLOAT3 pos; //xyz���W
	DirectX::XMFLOAT2 uv;  //uv���W
};

// �萔�o�b�t�@�p�f�[�^�\����
struct ConstBufferData
{
	DirectX::XMFLOAT4 color;    //�F (RGBA)
	DirectX::XMMATRIX mat;      //3D�ϊ��s��
	DirectX::XMFLOAT3 lightVec; //�����ւ̃x�N�g��
};
// �萔�o�b�t�@�p�f�[�^�\����(�}�e���A��)
struct MaterialConstBufferData
{
	DirectX::XMFLOAT3 ambient;  //�A���r�G���g�W��
	float pad1;                 //�p�f�B���O
	DirectX::XMFLOAT3 diffuse;  //�f�B�t���[�Y�W��
	float pad2;                 //�p�f�B���O
	DirectX::XMFLOAT3 specular; //�X�y�L�����[�W��
	float alpha;                //�A���t�@
};
// �萔�o�b�t�@�p�f�[�^�\����(�X�v���C�g�p)
struct SpriteConstBufferData
{
	DirectX::XMFLOAT4 color; //�F (RGBA)
	DirectX::XMMATRIX mat;   //3D�ϊ��s��
};

// �f�B�X�N���v�^�n���h��
struct DescHandle
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandle; //CPU���̃n���h��
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandle; //GPU���̃n���h��
};

// �e�I�u�W�F�N�g(�X�v���C�g)�ŋ��ʂ̃f�[�^���܂Ƃ߂��\����
struct CommonData
{
	/*'matProjection'�̈���*/
	enum Projection
	{
		ORTHOGRAPHIC, //���s���e
		PERSPECTIVE   //�������e
	};

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelinestate[5]; //�p�C�v���C���̏��
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature;    //���[�g�V�O�l�`��

	DirectX::XMMATRIX matProjection[2]; //�ˉe�s��

	// �R���X�g���N�^
	CommonData();
};

// �I�u�W�F�N�g�̃��[���h�s����܂Ƃ߂��\����
class Object
{
	/*�z��ɂ��Ďg�����Ƃ��O��*/
public: // �����o�֐�
	Object();
	virtual ~Object();

	// ������
	virtual void Init();
	// �X�V
	virtual void Update();

	// �R���C�_�[�̐ݒ�
	void SetCollider(BaseCollider* collider);
	// �Փˎ��R�[���o�b�N�֐�
	virtual void OnCollision(const CollisionInfo& info) {}

public: // �����o�ϐ�
	int polygonData; //���_���̗v�f�ԍ�
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff; //�萔�o�b�t�@

	DirectX::XMFLOAT3 position; //���W
	DirectX::XMMATRIX rotation; //��]�s��
	DirectX::XMFLOAT3 scale;    //�X�P�[��
	DirectX::XMFLOAT4 color;    //�F

	DirectX::XMMATRIX matWorld; //���[���h�s��

	Object* parent; //�e�̗v�f�ԍ�

	const char* name = nullptr; //�N���X��(�f�o�b�O�p)
	BaseCollider* collider = nullptr; //�R���C�_�[
};

// �}�e���A��
struct Material
{
	std::string name;            //�}�e���A����
	DirectX::XMFLOAT3 ambient;   //�A���r�G���g�e���x
	DirectX::XMFLOAT3 diffuse;   //�f�B�t���[�Y�e���x
	DirectX::XMFLOAT3 specular;  //�X�y�L�����[�e���x
	float alpha;                 //�A���t�@
	std::string textureFilename; //�e�N�X�`���t�@�C����

	size_t textrueIndex; //�e�N�X�`���̃C���f�b�N�X

	// �R���X�g���N�^
	Material()
	{
		ambient = { 0.3f,0.3f,0.3f };
		diffuse = { 0.0f,0.0f,0.0f };
		specular = { 0.0f,0.0f,0.0f };
		alpha = 1.0f;

		textrueIndex = 0;
	}
};

// obj�t�@�C���̃f�[�^
class OBJData : public Object
{
	/*�z��ɂ��Ďg�����Ƃ��O��*/
	/*��Ƀ��f���s�v�̃I�u�W�F�N�g�𐶐�����*/
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> materialConstBuff; //�萔�o�b�t�@
	Material material;

	// �R���X�g���N�^
	OBJData() : Object()
	{
		material = {};
	}
	// �f�X�g���N�^
	virtual ~OBJData() override
	{
		Object::~Object();
	}
};

struct OBJSubset
{
	size_t objStart;
	size_t verticesCount;
};

// �X�v���C�g�ꖇ���̃f�[�^���܂Ƃ߂��\����
struct Sprite
{
	/*�z��ɂ��Ďg�����Ƃ��O��*/

	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;  //���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView;                  //���_�o�b�t�@�r���[
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff; //�萔�o�b�t�@

	DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity(); //���[���h�s��
	DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }; //�F (RGBA)

	DirectX::XMFLOAT3 pos = { 0, 0, 0 }; //���W
	float rotation = 0.0f; //��]�p
	DirectX::XMFLOAT2 size = { 0.000000001f, 0.000000001f }; //�傫��

	DirectX::XMFLOAT2 anchorpoint = { 0.5f, 0.5f }; //�A���J�[�|�C���g

	// �`��͈�
	DirectX::XMFLOAT2 texLeftTop = { 0, 0 };
	DirectX::XMFLOAT2 texSize = { 100, 100 };
};

class DirectDrawing
{
public: // �T�u�N���X
	// �I�u�W�F�N�g�f�[�^�i�X�v���C�g�̃f�[�^�j�̃C���f�b�N�X
	struct IndexData
	{
		int constant; //�萔�o�b�t�@
		int textrue;  //�e�N�X�`��
	};

protected: // �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMMATRIX = DirectX::XMMATRIX;
	// std::���ȗ�
	template<class T> using vector = std::vector<T>;
	// Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // �ÓI�����o�ϐ�
	static bool isDepthWriteBan; //true��������A�f�v�X�ɑ΂��Ă̏㏑���֎~
protected:
	static vector<Sprite> sprite; //�X�v���C�g�̃f�[�^
	static vector<IndexData> spriteIndex; //�X�v���C�g�̃f�[�^�̃C���f�b�N�X

	static CommonData spriteData; //�X�v���C�g�̃f�[�^�ŋ��ʂ̃f�[�^

	static size_t blendMode; //�u�����h���[�h

public: // �����o�֐�
	// �R���X�g���N�^
	DirectDrawing(const DirectXInit* w);
	// �f�X�g���N�^
	~DirectDrawing();

	/// <summary>
	/// ��̒萔�o�b�t�@�̐���
	/// </summary>
	/// <param name="pos"> ���W </param>
	/// <param name="rota"> ��]�s�� </param>
	/// <param name="scale"> �X�P�[�� </param>
	/// <returns> �I�u�W�F�N�g�f�[�^�̗v�f�ԍ� </returns>
	int CreateNullConstant(const XMFLOAT3& pos, const XMMATRIX& rota, const XMFLOAT3& scale);
	/// <summary>
	/// �萔�o�b�t�@�̍X�V
	/// </summary>
	/// <param name="pos"> ���W </param>
	/// <param name="rota"> ��]�s�� </param>
	/// <param name="scale"> �X�P�[�� </param>
	/// <param name="objectIndex"> �I�u�W�F�N�g�f�[�^�̗v�f�ԍ� </param>
	/// <param name="polygonData"> ���_���̗v�f�ԍ� </param>
	/// <param name="parent"> �e�̗v�f�ԍ� </param>
	void UpdataConstant(const XMFLOAT3& pos, const XMMATRIX& rota, const XMFLOAT3& scale,
		const DirectX::XMFLOAT4& color, const int& objectIndex, const int& polygonData = -1,
		Object* parent = nullptr);
	// �`��̍ۂ̃u�����h���[�h���Z�b�g����
	int SetDrawBlendMode(int blendMode);
	// �J�����̃j�A�N���b�v�����ƃt�@�[�N���b�v������ݒ肷��
	HRESULT SetNearFar(float nearClip, float farClip);

	inline OBJData* GetObjData(int object)
	{
		if (object < 0 || object >= (int)obj.size()) { return nullptr; }
		return &obj[object];
	}
protected:
	// ������
	HRESULT Init();
	// �I�u�W�F�N�g�̕`��֌W�̏�����
	HRESULT DrawingInit();
	// �I�u�W�F�N�g�̕`��֌W�̏�����
	HRESULT MaterialInit();
	// �X�v���C�g�̕`��֌W�̏�����
	HRESULT SpriteDrawingInit();
	// ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�̐���
	int CreateVertexAndIndexBuffer();
	// �萔�o�b�t�@�̐���
	HRESULT CreateConstBuffer(int* objIndex);
	// �X�v���C�g�̐���
	int CreateSprite();
	// �f�B�X�N���v�^�q�[�v�̏�����
	HRESULT BasicDescHeapInit();

	// �`�揈���̋��ʕ���
	void BaseDrawGraphics();
	// �`�揈���̋��ʕ���(�X�v���C�g�p)
	void BaseDrawSpriteGraphics();
private:
	// �f�[�^�̏���
	void DataClear();
protected: // �����o�ϐ�
	const DirectXInit* w;

	vector<VertexData> vertices; //���_�f�[�^
	Vertex* vertMap;

	vector<OBJData> obj; //�I�u�W�F�N�g�f�[�^
	vector<IndexData> objIndex; //�I�u�W�F�N�g�f�[�^�̃C���f�b�N�X
	ComPtr<ID3D12DescriptorHeap> basicDescHeap; //�萔�o�b�t�@�p�̃f�X�N���v�^�q�[�v
	vector<DescHandle> srvHandle;
	size_t nullBufferCount; //���[���h�s�񂾂��̃I�u�W�F�N�g�f�[�^�̐�

	ComPtr<ID3DBlob> vsBlob;    //���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;    //�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; //�G���[�I�u�W�F�N�g
	ComPtr<ID3DBlob> rootSigBlob;

	CommonData objectData[2];   //�I�u�W�F�N�g�f�[�^�ŋ��ʂ̃f�[�^
	CommonData materialData[2]; //�}�e���A���f�[�^�ŋ��ʂ̃f�[�^

	vector<OBJSubset> objSubset;
private:
	float nearClip; //�j�A�N���b�v����
	float farClip;  //�t�@�[�N���b�v����

	// �֐��̏������t���O
	bool isDrawingInit = false;
	bool isMaterialInit = false;
	bool isSpriteDrawingInit = false;
	bool isBasicDescHeapInit = false;

};
