#include "./Header/DirectInput.h"
#include "./Header/Win32API.h"
#include <cassert>

#define EoF (-1) // End of Function

const LONG DirectInput::zoneMax = +1000;
const LONG DirectInput::zoneMin = -1000;

IDirectInput8* DirectInput::dinput = nullptr;
IDirectInputDevice8* DirectInput::devkeyborad = nullptr;
std::vector<IDirectInputDevice8*> DirectInput::devgamepad = {};
BYTE DirectInput::key[256] = {};
std::vector<DIJOYSTATE> DirectInput::gamepad = {};
std::vector<DIDEVICEINSTANCE> DirectInput::parameter = {};

DirectInput* DirectInput::GetInstance()
{
	// �C���X�^���X�̐���
	static DirectInput instance;

	return &instance;
}

BOOL CALLBACK DirectInput::EnumGamepadCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	std::vector<DIDEVICEINSTANCE>* ptr = (std::vector<DIDEVICEINSTANCE>*)pvRef;
	DIDEVICEINSTANCE tmp = *lpddi;

	// �z��Ɋi�[
	ptr->push_back(tmp);

	return DIENUM_CONTINUE;
}

int DirectInput::Init()
{
	HRESULT hr;

	// DirectInput�C���^�[�t�F�[�X�̐���
	hr = DirectInput8Create(Win32API::GetWindow().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		OutputDebugStringA("DirectInput�C���^�[�t�F�[�X�̍쐬�Ɏ��s���܂���\n");
#endif // _DEBUG

		return EoF;
	}

	// �L�[�{�[�h�f�o�C�X�̐���
	hr = dinput->CreateDevice(GUID_SysKeyboard, &devkeyborad, NULL);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		OutputDebugStringA("�L�[�{�[�h�f�o�C�X�̍쐬�Ɏ��s���܂���\n");
#endif // _DEBUG

		return EoF;
	}

	// ���̓f�[�^�`���̃Z�b�g
	hr = devkeyborad->SetDataFormat(&c_dfDIKeyboard); //�W���`��
	if (FAILED(hr))
	{
#ifdef _DEBUG
		OutputDebugStringA("�L�[�{�[�h�f�o�C�X�̃f�[�^�`���̐ݒ�Ɏ��s���܂���\n");
#endif // _DEBUG

		return EoF;
	}

	// �������x���̃Z�b�g
	hr = devkeyborad->SetCooperativeLevel(Win32API::GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		OutputDebugStringA("�L�[�{�[�h�f�o�C�X�̋������x���̐ݒ�Ɏ��s���܂���\n");
#endif // _DEBUG

		return EoF;
	}

	// �Q�[���p�b�h�f�o�C�X�̐���
	hr = CreateGamePadDevice();
	if (FAILED(hr))
	{
		return EoF;
	}

	return 0;
}

int DirectInput::Update()
{
	if (KeyInputUpdate() == EoF)
	{
		return EoF;
	}
	if (GamePadInputUpdate() == EoF)
	{
		return EoF;
	}

	return 0;
}

void DirectInput::Release()
{
	// �L�[�{�[�h�f�o�C�X�̐�����~������
	devkeyborad->Unacquire();
	// �L�[�{�[�h�f�o�C�X���������
	devkeyborad->Release();

	for (size_t i = 0; i < devgamepad.size(); i++)
	{
		// �e�Q�[���p�b�h�f�o�C�X�̐�����~������
		devgamepad[i]->Unacquire();
		// �e�Q�[���p�b�h�f�o�C�X���������
		devgamepad[i]->Release();
	}

	// DirectInput���������
	dinput->Release();
}

bool DirectInput::CheckHitKey(int keyCode)
{
	if (keyCode < 0 || keyCode >= sizeof(key))
	{
		return false;
	}

	if (KeyInputUpdate() != 0)
	{
		return false;
	}

	if (key[keyCode])
	{
		return true;
	}

	return false;
}

int DirectInput::CheckHitKeyAll(int CheckType)
{
	if ((CheckType & CHECKINPUT_KEY) != 0)
	{
		for (int i = 0; i < sizeof(key) / sizeof(key[0]); i++)
		{
			if (CheckHitKey(i) == true)
			{
				return 0;
			}
		}
	}
	if ((CheckType & CHECKINPUT_PAD) != 0)
	{

	}
	if ((CheckType & CHECKINPUT_MOUSE) != 0)
	{

	}

	return -1;
}

int DirectInput::GetHitKeyStateAll(char* KeyStateArray)
{
	if (KeyStateArray == nullptr)
	{
		return -1;
	}

	for (int i = 0; i < sizeof(key) / sizeof(key[0]); i++)
	{
		if (CheckHitKey(i) == true)
		{
			KeyStateArray[i] = (char)true;
		}
		else
		{
			KeyStateArray[i] = (char)false;
		}
	}

	return 0;
}

int DirectInput::GetGamepadStateAll(DIJOYSTATE* gamepadState, const size_t& gamepadIndex)
{
	if (gamepadIndex >= gamepad.size())
	{
		return EoF;
	}

	*gamepadState = gamepad[gamepadIndex];

	return 0;
}

int DirectInput::KeyInputUpdate()
{
	HRESULT hr;

	ZeroMemory(key, sizeof(key));
	hr = devkeyborad->GetDeviceState(sizeof(key), key);
	if (FAILED(hr)) {
		// ���s�Ȃ�ĊJ�����Ă�����x�擾
		devkeyborad->Acquire();
		devkeyborad->GetDeviceState(sizeof(key), key);
	}

	return 0;
}

int DirectInput::GamePadInputUpdate(const size_t& gamepadNo)
{
	HRESULT hr;

	if (gamepadNo > devgamepad.size())
	{
#ifdef _DEBUG
		OutputDebugStringA("�w�肳�ꂽ�Q�[���p�b�h�͂���܂���\n");
#endif // _DEBUG

		assert(0);
		return EoF;
	}

	if (gamepadNo == devgamepad.size())
	{
		hr = CreateGamePadDevice();
		if (FAILED(hr))
		{
			return EoF;
		}
	}

	hr = devgamepad[gamepadNo]->Acquire();
	if (hr == DIERR_INPUTLOST)
	{
		// �f�o�C�X�����X�g���Ă�����֐����I������
		return EoF;
	}

	devgamepad[gamepadNo]->Poll();

	for (size_t i = gamepad.size(); i < gamepadNo; i++)
	{
		gamepad.push_back({});
	}
	ZeroMemory(&gamepad[gamepadNo], sizeof(DIJOYSTATE));
	devgamepad[gamepadNo]->GetDeviceState(sizeof(DIJOYSTATE), &gamepad[gamepadNo]);

	return 0;
}

HRESULT DirectInput::CreateGamePadDevice()
{
	HRESULT hr = S_OK;

	// �Q�[���p�b�h�f�o�C�X�̗�
	hr = dinput->EnumDevices(DI8DEVTYPE_GAMEPAD, &EnumGamepadCallback, &parameter, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		OutputDebugStringA("�Q�[���p�b�h�f�o�C�X�̗񋓂Ɏ��s���܂���\n");
#endif // _DEBUG

		return hr;
	}

	for (size_t i = 0; i < parameter.size(); i++)
	{
		devgamepad.push_back({});

		// �Q�[���p�b�h�f�o�C�X�̐���
		hr = dinput->CreateDevice(parameter[i].guidInstance, &devgamepad[i], NULL);
		if (FAILED(hr))
		{
#ifdef _DEBUG
			OutputDebugStringA("�Q�[���p�b�h�f�o�C�X�̐����Ɏ��s���܂���\n");
#endif // _DEBUG

			return hr;
		}

		// ���̓f�[�^�`���̃Z�b�g
		hr = devgamepad[i]->SetDataFormat(&c_dfDIJoystick);
		if (FAILED(hr))
		{
#ifdef _DEBUG
			OutputDebugStringA("�Q�[���p�b�h�f�o�C�X�̃f�[�^�`���̐ݒ�Ɏ��s���܂���\n");
#endif // _DEBUG

			return hr;
		}

		// �v���p�e�B�̐ݒ�
		hr = SetUpGamePadProperty(i);
		if (FAILED(hr))
		{
#ifdef _DEBUG
			OutputDebugStringA("�Q�[���p�b�h�f�o�C�X�̃v���p�e�B�̐ݒ�Ɏ��s���܂���\n");
#endif // _DEBUG

			return hr;
		}

		// �������x���̐ݒ�
		hr = devgamepad[i]->SetCooperativeLevel(Win32API::GetHwnd(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);
		if (FAILED(hr))
		{
#ifdef _DEBUG
			OutputDebugStringA("�Q�[���p�b�h�f�o�C�X�̋������x���̐ݒ�Ɏ��s���܂���\n");
#endif // _DEBUG

			return hr;
		}
	}

	return hr;
}

HRESULT DirectInput::SetUpGamePadProperty(const size_t& gamepadNo)
{
	if (gamepadNo >= devgamepad.size())
	{
#ifdef _DEBUG
		OutputDebugStringA("�w�肳�ꂽ�Q�[���p�b�h�͂���܂���\n");
#endif // _DEBUG

		return S_FALSE;
	}

	HRESULT hr;

	// �����[�h���Βl���[�h�Ƃ��Đݒ�
	DIPROPDWORD diprop;
	ZeroMemory(&diprop, sizeof(diprop));
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.diph.dwObj = 0;
	diprop.dwData = DIPROPAXISMODE_ABS;
	hr = devgamepad[gamepadNo]->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	if (FAILED(hr))
	{
		return hr;
	}

	// X���̒l�͈̔͐ݒ�
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.diph.dwObj = DIJOFS_X;
	diprg.lMin = zoneMin;
	diprg.lMax = zoneMax;
	hr = devgamepad[gamepadNo]->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		OutputDebugStringA("X���̐ݒ�Ɏ��s���܂���\n");
#endif // _DEBUG
	}

	// Y���̒l�͈̔͐ݒ�
	diprg.diph.dwObj = DIJOFS_Y;
	hr = devgamepad[gamepadNo]->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		OutputDebugStringA("Y���̐ݒ�Ɏ��s���܂���\n");
#endif // _DEBUG
	}

	// Z���̒l�͈̔͐ݒ�
	diprg.diph.dwObj = DIJOFS_Z;
	hr = devgamepad[gamepadNo]->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		OutputDebugStringA("Z���̐ݒ�Ɏ��s���܂���\n");
#endif // _DEBUG
	}

	// RX���̒l�͈̔͐ݒ�
	diprg.diph.dwObj = DIJOFS_RX;
	hr = devgamepad[gamepadNo]->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		OutputDebugStringA("RX���̐ݒ�Ɏ��s���܂���\n");
#endif // _DEBUG
	}

	// RY���̒l�͈̔͐ݒ�
	diprg.diph.dwObj = DIJOFS_RY;
	hr = devgamepad[gamepadNo]->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		OutputDebugStringA("RY���̐ݒ�Ɏ��s���܂���\n");
#endif // _DEBUG
	}

	// RZ���̒l�͈̔͐ݒ�
	diprg.diph.dwObj = DIJOFS_RZ;
	hr = devgamepad[gamepadNo]->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		OutputDebugStringA("RZ���̐ݒ�Ɏ��s���܂���\n");
#endif // _DEBUG
	}

	return S_OK;
}