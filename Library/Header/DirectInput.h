#pragma once
#define DIRECTINPUT_VERSION 0x0800 //DirectInput�̃o�[�W�����w��
#include <dinput.h>
#include <vector>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

// �Q�[���p�b�h�̏\���L�[����
#define PAD_POVLEFT   (28)
#define PAD_POVRIGHT  (29)
#define PAD_POVUP     (30)
#define PAD_POVDOWN   (31)

// CheckHitKeyAll �Œ��ׂ���̓^�C�v
#define CHECKINPUT_KEY   (0x01) //�L�[���͂𒲂ׂ�
#define CHECKINPUT_PAD   (0x02) //�p�b�h���͂𒲂ׂ�
#define CHECKINPUT_MOUSE (0x04) //�}�E�X�{�^�����͂𒲂ׂ�
#define CHECKINPUT_ALL   (CHECKINPUT_KEY | CHECKINPUT_PAD | CHECKINPUT_MOUSE) //���ׂĂ̓��͂𒲂ׂ�

class DirectInput final
{
private:
	// �R���X�g���N�^��private�ɂ���
	DirectInput() {};
	// �f�X�g���N�^��private�ɂ���
	~DirectInput() {}
public:
	// �R�s�[�R���X�g���N�^�𖳌��ɂ���
	DirectInput(const DirectInput& obj) = delete;
	// ������Z�q�𖳌��ɂ���
	const DirectInput& operator=(const DirectInput& obj) = delete;
	// �C���X�^���X�̎擾
	static DirectInput* GetInstance();

public: // �萔
	static const LONG zoneMax; //�X�e�B�b�N���͂̏���l
	static const LONG zoneMin; //�X�e�B�b�N���͂̉����l

public: // �ÓI�����o�֐�
	// ������
	static int Init();
	// �X�V
	static int Update();
	// �f�o�C�X�̉��
	static void Release();

	// ����̃L�[�{�[�h�̓��͏�Ԃ̎擾
	static bool CheckHitKey(int keyCode);
	// �S�Ă̓��̓f�o�C�X�̏�Ԃ̎擾
	static int CheckHitKeyAll(int CheckType = CHECKINPUT_ALL);
	// �L�[�{�[�h�f�o�C�X�̓��͏�Ԃ̎擾
	static int GetHitKeyStateAll(char* KeyStateArray);
	// ����̃Q�[���p�b�h�f�o�C�X�̐��̓��͏�Ԃ̎擾
	static int GetGamepadStateAll(DIJOYSTATE* gamepadState, const size_t& gamepadIndex = 0);
private:
	// �f�o�C�X��񋓂��鎞�̃R�[���o�b�N�֐�
	static BOOL CALLBACK EnumGamepadCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	// �L�[�{�[�h�f�o�C�X�̍X�V
	static int KeyInputUpdate();
	// �Q�[���p�b�h�f�o�C�X�̍X�V
	static int GamePadInputUpdate(const size_t& gamepadNo = 0);
	// �Q�[���p�b�h�f�o�C�X�̐���
	static HRESULT CreateGamePadDevice();
	// �v���p�e�B�̐ݒ�
	static HRESULT SetUpGamePadProperty(const size_t& gamepadNo = 0);

private: // �ÓI�����o�ϐ�
	// DirectInput�C���^�[�t�F�[�X
	static IDirectInput8* dinput;
	// �L�[�{�[�h�f�o�C�X
	static IDirectInputDevice8* devkeyborad;
	// �Q�[���p�b�h�f�o�C�X
	static std::vector<IDirectInputDevice8*> devgamepad;

	// �L�[�{�[�h�̓��͏��
	static BYTE key[256];
	// �Q�[���p�b�h�̓��͏��
	static std::vector<DIJOYSTATE> gamepad;

	static std::vector<DIDEVICEINSTANCE> parameter;

};
