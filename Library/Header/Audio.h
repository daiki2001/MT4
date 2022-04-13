#pragma once
#include <xaudio2.h>
#include <wrl.h>
#include <vector>

#pragma comment(lib, "xaudio2.lib")

class Audio
{
public: // �T�u�N���X

	// �`�����N�w�b�_
	struct Chunk
	{
		char id[4]; //�`�����N����ID
		INT32 size; //�`�����N�T�C�Y
	};
	// RIFF�w�b�_�`�����N
	struct RiffHeader
	{
		Chunk chunk;  //"RIFF"
		char type[4]; //"WAVE"
	};
	// FMT�w�b�_�`�����N
	struct FormatChunk
	{
		Chunk chunk;      //"fmt"
		WAVEFORMATEX fmt; //�g�`�t�H�[�}�b�g
	};
	// �����f�[�^
	struct SoundData
	{
		WAVEFORMATEX wfex;       //�g�`�t�H�[�}�b�g
		BYTE* pBuffer;           //�o�b�t�@�̐擪�A�h���X
		unsigned int bufferSize; //�o�b�t�@�̃T�C�Y
	};

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;

	std::vector<SoundData> soundDatas;

public:
	Audio();
	~Audio();

	// ������
	HRESULT Init();

	// �����f�[�^�̓ǂݍ���
	int SoundLoadWave(const char* filePath);
	// �����f�[�^�̍Đ�
	void SoundPlayWave(const int& soundData);
private:
	// �����f�[�^�̊J��
	void SoundUnload(const int& soundData);
};
