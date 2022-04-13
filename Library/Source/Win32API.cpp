#include "./Header/Win32API.h"

HWND Win32API::hWnd{};
WNDCLASSEX Win32API::wndClass{};
#ifdef UNICODE
LPCWSTR Win32API::className = L"DirectX";
LPCWSTR Win32API::title = L"DirectXGame";
#else
LPCSTR Win32API::className = "DirectX";
LPCSTR Win32API::title = "DirectXGame";
#endif // UNICODE

LRESULT Win32API::WindowProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wparam, lparam);
}

Win32API::Win32API() :
	windowWidth(1),
	windowHeight(1),
	windowInit(false),
	MessageCount(0)
{
}

BOOL WINAPI Win32API::WindowInit()
{
	if (windowInit == true)
	{
		return TRUE;
	}
	else
	{
		windowInit = true;
	}

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = (WNDPROC)WindowProc;
	wndClass.lpszClassName = className;
	wndClass.hInstance = GetModuleHandle(nullptr);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	RECT windowSize = { 0, 0, windowWidth, windowHeight };
	// �E�B���h�E�N���X��OS�ɓo�^
	RegisterClassEx(&wndClass);
	// �E�B���h�E�T�C�Y
	if (AdjustWindowRect(&windowSize, WS_OVERLAPPEDWINDOW, false) == false)
	{
		return false;
	}

	hWnd = CreateWindow(
		wndClass.lpszClassName, //�N���X��
		title,                  //�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,    //�W���I�ȃE�B���h�E�X�^�C��
		CW_USEDEFAULT,          //�\��X���W(OS�ɔC����)
		CW_USEDEFAULT,          //�\��Y���W(OS�ɔC����)
		windowWidth,            //�E�B���h�E����
		windowHeight,           //�E�B���h�E�c��
		nullptr,                //�e�E�B���h�E�n���h��
		nullptr,                //���j���[�n���h��
		wndClass.hInstance,     //�Ăяo���A�v���P�[�V�����n���h��
		nullptr                 //�I�v�V����
	);

	// �E�B���h�E�\��
	return ShowWindow(hWnd, SW_SHOW);
}

int Win32API::WindowMessage()
{
	MSG msg{}; //���b�Z�[�W

	MessageCount++;

	// ���b�Z�[�W����?
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg); //�L�[���̓��b�Z�[�W�̏���
		DispatchMessage(&msg); //�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
	}

	// �~�{�^���ŏI�����b�Z�[�W��������Q�[�����[�v�𔲂���
	if (msg.message == WM_QUIT)
	{
		return -1;
	}

	return 0;
}

BOOL WINAPI Win32API::WindowEnd()
{
	//�E�B���h�E�N���X��o�^����
	return UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);
}