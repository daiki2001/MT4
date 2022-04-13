#pragma once
#include "DirectInput.h"
#include "Win32API.h"

class Input final
{
private:
	// �R���X�g���N�^��private�ɂ���
	Input() {}
	// �f�X�g���N�^��private�ɂ���
	~Input() {}
public:
	// �R�s�[�R���X�g���N�^�̍폜
	Input(const Input& obj) = delete;
	// ������Z�q�̍폜
	const Input& operator=(const Input& obj) = delete;

public: // �ÓI�����o�֐�
	// ������
	static void Init();
	// �X�V
	static void Update();
	// �L�[��������Ă��邩�ǂ���
	static bool IsKey(int key);
	// �L�[�������ꂽ���ǂ���
	static bool IsKeyTrigger(int key);
	// �L�[���痣�ꂽ���ǂ���
	static bool IsKeyReturn(int key);

private: // �ÓI�����o�ϐ�
	// ���̓f�o�C�X
	static DirectInput* input;
	// �ŐV�̃L�[�{�[�h���p
	static char keys[256];
	// 1���[�v(�t���[��)�O�̃L�[�{�[�h���
	static char oldkeys[256];

};
