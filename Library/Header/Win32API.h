#pragma once
#include <Windows.h>

class Win32API
{
public:
	// �f�t�H���g�R���X�g���N�^
	Win32API();
	// �f�X�g���N�^
	~Win32API() {}

public: // �ÓI�����o�֐�
	// �E�B���h�E�v���V�[�W��
	static LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// �E�B���h�E�N���X�̎擾
	static WNDCLASSEX GetWindow() { return wndClass; }
	// �E�B���h�E�n���h���̎擾
	static HWND GetHwnd() { return hWnd; }

public: // �ÓI�����o�ϐ�
#ifdef UNICODE
	static LPCWSTR className; //�N���X��
	static LPCWSTR title; //�^�C�g���o�[�̕���
#else
	static LPCSTR className; //�N���X��
	static LPCSTR title; //�^�C�g���o�[�̕���
#endif // UNICODE
private:
	static WNDCLASSEX wndClass;
	static HWND hWnd;

public: // �����o�֐�
	// ������
	BOOL WINAPI WindowInit();
	// �E�B���h�E���b�Z�[�W
	int WindowMessage();
	// �I������
	BOOL WINAPI WindowEnd();

public: // �����o�ϐ�
int windowWidth, windowHeight; //�E�B���h�E�T�C�Y

bool windowInit; //�������t���O
UINT MessageCount; //WindowMessage()��ʂ�����

};
