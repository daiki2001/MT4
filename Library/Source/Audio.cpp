#include "./Header/Audio.h"
#include <fstream>
#include <cassert>

#define EoF -1 // End of Function

Audio::Audio() :
	xAudio2(nullptr),
	masterVoice(nullptr),
	soundDatas{}
{
}

Audio::~Audio()
{
	xAudio2.Reset();

	for (size_t i = 0; i < soundDatas.size(); i++)
	{
		SoundUnload(static_cast<const int>(i));
	}
}

HRESULT Audio::Init()
{
	HRESULT hr = S_FALSE;

	// xAudio�G���W���̃C���X�^���X�𐶐�
	hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr))
	{
		return hr;
	}

	// �}�X�^�[�{�C�X�𐶐�
	hr = xAudio2->CreateMasteringVoice(&masterVoice);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

int Audio::SoundLoadWave(const char* filePath)
{
	using namespace std;

	// �t�@�C�����̓X�g���[���̃C���X�^���X
	ifstream file;
	// .wav�t�@�C�����o�C�i�����[�h�ŊJ��
	file.open(filePath, ios_base::binary);
	// �t�@�C���I�[�v�����s�����o����
	if (file.fail())
	{
		return EoF;
	}

	// RIFF�w�b�_�[�̓ǂݍ���
	Audio::RiffHeader riff = {};
	file.read((char*)&riff, sizeof(riff));
	// �t�@�C����RIFF���`�F�b�N
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}
	// �^�C�v��WAVE���`�F�b�N
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}

	// Format�`�����N�̓ǂݍ���
	Audio::FormatChunk format = {};
	// �`�����N�w�b�_�[�̊m�F
	file.read((char*)&format, sizeof(Audio::Chunk));
	// �`�����N�w�b�_�[�̊m�F
	if (strncmp(format.chunk.id, "fmt ", 4) != 0)
	{
		assert(0);
	}

	// �`�����N�{�̂̓ǂݍ���
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Data�`�����N�̓ǂݍ���
	Audio::Chunk data = {};
	file.read((char*)&data, sizeof(data));
	// JUNK�`�����N��LIST�`�����N�����o�����ꍇ
	if (strncmp(data.id, "JUNK", 4) == 0 || strncmp(data.id, "LIST", 4) == 0)
	{
		// �ǂݎ��ʒu��JUNK�`�����N�̏I���܂Ői�߂�
		file.seekg(data.size, ios_base::cur);
		// �ēǂݍ���
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}

	// Data�`�����N�̃f�[�^���i�g�`�f�[�^�j�̓ǂݍ���
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// wave�t�@�C�������
	file.close();

	// return����ׂ̉����f�[�^
	soundDatas.push_back({});

	soundDatas[soundDatas.size() - 1].wfex = format.fmt;
	soundDatas[soundDatas.size() - 1].pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundDatas[soundDatas.size() - 1].bufferSize = data.size;

	return static_cast<int>(soundDatas.size()) - 1;
}

void Audio::SoundPlayWave(const int& soundData)
{
	HRESULT hr;

	// �g�`�t�H�[�}�b�g������SourceVoice�𐶐�
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hr = xAudio2->CreateSourceVoice(&pSourceVoice, &soundDatas[soundData].wfex);
	assert(SUCCEEDED(hr));

	// �Đ�����g�`�f�[�^�̐ݒ�
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundDatas[soundData].pBuffer;
	buf.AudioBytes = soundDatas[soundData].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// �g�`�f�[�^�̍Đ�
	hr = pSourceVoice->SubmitSourceBuffer(&buf);
	if (FAILED(hr))
	{
		return;
	}
	hr = pSourceVoice->Start();
	if (FAILED(hr))
	{
		return;
	}
}

void Audio::SoundUnload(const int& soundData)
{
	// �o�b�t�@�̃����������
	delete[] soundDatas[soundData].pBuffer;

	soundDatas[soundData].pBuffer = 0;
	soundDatas[soundData].bufferSize = 0;
	soundDatas[soundData].wfex = {};
}
