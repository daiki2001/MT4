#pragma once
#include "LoadTex.h"

class DebugText : public LoadTex
{
public: // �萔
	static const int fontWidth = 8;      //�����̉���
	static const int fontHeight = 16;    //�����̏c��
	static const int fontLineCount = 16; //��s������̕�����

private: // �ÓI�����o�ϐ�
	static int fontTex;         //�t�H���g�摜
	static UINT64 fontTexWidth; //�t�H���g�摜�̉���
	static UINT fontTexHeight;  //�t�H���g�摜�̏c��

public: // �����o�֐�
	// �R���X�g���N�^
	DebugText(const DirectXInit* w);
	// �f�X�g���N�^
	~DebugText();

	/// <summary>
	/// �f�o�b�O�e�L�X�g
	/// </summary>
	/// <param name="posX, posY"> �������`�悷��̈�̍���̍��W </param>
	/// <param name="fontScale"> �����̑傫���i�{���j </param>
	/// <param name="color"> �F�iRGBA�j </param>
	/// <param name="text"> �`�悵���������t��������̃A�h���X </param>
	/// <param name="..."> �����t��������ɕt��������� </param>
	/// <returns> 0�Ő��� </returns>
	HRESULT DrawString(const float& posX, const float& posY, const float& fontScale,
		const XMFLOAT4& color, const char* text, ...);

	// ���[�v�I���i�f�o�b�O�e�L�X�g�Łj
	void TextLoopEnd();
private:
	// �f�[�^�̏���
	void DataClear();

	// �f�o�b�O�e�L�X�g�̏�����
	HRESULT DrawStringInit();

private: // �����o�ϐ�
	vector<int> fontIndex; //�萔�o�b�t�@
	int charCount;         //������

};
