/*
	TVTest �v���O�C���w�b�_ ver.0.0.13

	���̃t�@�C���͍Ĕz�z�E���ςȂǎ��R�ɍs���č\���܂���B
	�������A���ς����ꍇ�̓I���W�i���ƈႤ�|���L�ڂ��Ē�����ƁA�������Ȃ��Ă�
	���Ǝv���܂��B

	���ɃR���p�C���Ɉˑ�����L�q�͂Ȃ��͂��Ȃ̂ŁABorland �Ȃǂł����v�ł��B
	�܂��A�w�b�_���ڐA����� C++ �ȊO�̌���Ńv���O�C�����쐬���邱�Ƃ��\��
	�͂��ł�(�����ʓ|�Ȃ̂ŒN�����Ȃ��Ǝv���܂���)�B

	���ۂɃv���O�C�����쐬����ꍇ�A���̃w�b�_���������Ă����炭�Ӗ��s�����Ǝv
	���܂��̂ŁA�T���v�����Q�l�ɂ��Ă��������B
*/


/*
	TVTest �v���O�C���̊T�v

	�v���O�C����32/64�r�b�g DLL �̌`���ł��B�g���q�� .tvtp �Ƃ��܂��B
	�v���O�C���ł́A�ȉ��̊֐����G�N�X�|�[�g���܂��B

	DWORD WINAPI TVTGetVersion()
	BOOL WINAPI TVTGetPluginInfo(PluginInfo *pInfo)
	BOOL WINAPI TVTInitialize(PluginParam *pParam)
	BOOL WINAPI TVTFinalize()

	�e�֐��ɂ��ẮA���̃w�b�_�̍Ō�̕��ɂ���v���O�C���N���X�ł̃G�N�X�|�[�g
	�֐��̎���(#ifdef TVTEST_PLUGIN_CLASS_IMPLEMENT �̕���)���Q�Ƃ��Ă��������B

	�v���O�C������́A�R�[���o�b�N�֐���ʂ��ă��b�Z�[�W�𑗐M���邱�Ƃɂ��A
	TVTest �̋@�\�𗘗p���邱�Ƃ��ł��܂��B
	���̂悤�ȕ��@�ɂȂ��Ă���̂́A�����I�Ȋg�����e�Ղł��邽�߂ł��B

	�܂��A�C�x���g�R�[���o�b�N�֐���o�^���邱�Ƃɂ��ATVTest ����C�x���g����
	�m����܂��B

	���b�Z�[�W�̑��M�̓X���b�h�Z�[�t�ł͂���܂���̂ŁA�ʃX���b�h���烁�b�Z�[
	�W�R�[���o�b�N�֐����Ăяo���Ȃ��ł��������B

	TVTEST_PLUGIN_CLASS_IMPLEMENT �V���{���� #define ����Ă���ƁA�G�N�X�|�[�g
	�֐��𒼐ڋL�q���Ȃ��Ă��A�N���X�Ƃ��ăv���O�C�����L�q���邱�Ƃ��ł��܂��B
	���̏ꍇ�ATVTestPlugin �N���X����v���O�C���N���X��h�������܂��B

	�ȉ��͍Œ���̎������s�����T���v���ł��B

	#include <windows.h>
	#define TVTEST_PLUGIN_CLASS_IMPLEMENT	// �v���O�C�����N���X�Ƃ��Ď���
	#include "TVTestPlugin.h"

	// �v���O�C���N���X�BCTVTestPlugin ����h��������
	class CMyPlugin : public TVTest::CTVTestPlugin
	{
	public:
		bool GetPluginInfo(TVTest::PluginInfo *pInfo) {
			// �v���O�C���̏���Ԃ�
			pInfo->Type           = TVTest::PLUGIN_TYPE_NORMAL;
			pInfo->Flags          = 0;
			pInfo->pszPluginName  = L"�T���v��";
			pInfo->pszCopyright   = L"Copyright(c) 2010 Taro Yamada";
			pInfo->pszDescription = L"�������Ȃ��v���O�C��";
			return true;	// false ��Ԃ��ƃv���O�C���̃��[�h�����s�ɂȂ�
		}
		bool Initialize() {
			// �����ŏ��������s��
			// �������Ȃ��̂ł���΃I�[�o�[���C�h���Ȃ��Ă��ǂ�
			return true;	// false ��Ԃ��ƃv���O�C���̃��[�h�����s�ɂȂ�
		}
		bool Finalize() {
			// �����ŃN���[���A�b�v���s��
			// �������Ȃ��̂ł���΃I�[�o�[���C�h���Ȃ��Ă��ǂ�
			return true;
		}
	};

	// CreatePluginClass �֐��ŁA�v���O�C���N���X�̃C���X�^���X�𐶐����ĕԂ�
	TVTest::CTVTestPlugin *CreatePluginClass()
	{
		return new CMyPlugin;
	}
*/


/*
	�X�V����

	ver.0.0.13 (TVTest ver.0.7.16 or later)
	�E�ȉ��̃��b�Z�[�W��ǉ�����
	  �EMESSAGE_ENABLEPROGRAMGUIDEEVENT
	  �EMESSAGE_REGISTERPROGRAMGUIDECOMMAND
	�E�ȉ��̃C�x���g��ǉ�����
	  �EEVENT_STARTUPDONE
	  �EEVENT_PROGRAMGUIDE_INITIALIZE
	  �EEVENT_PROGRAMGUIDE_FINALIZE
	  �EEVENT_PROGRAMGUIDE_COMMAND
	  �EEVENT_PROGRAMGUIDE_INITIALIZEMENU
	  �EEVENT_PROGRAMGUIDE_MENUSELECTED
	  �EEVENT_PROGRAMGUIDE_PROGRAM_DRAWBACKGROUND
	  �EEVENT_PROGRAMGUIDE_PROGRAM_INITIALIZEMENU
	  �EEVENT_PROGRAMGUIDE_PROGRAM_MENUSELECTED

	ver.0.0.12 (TVTest ver.0.7.14 or later)
	�E�ȉ��̃��b�Z�[�W��ǉ�����
	  �EMESSAGE_GETEPGEVENTINFO
	  �EMESSAGE_FREEEPGEVENTINFO
	  �EMESSAGE_GETEPGEVENTLIST
	  �EMESSAGE_FREEEPGEVENTLIST
	  �EMESSAGE_ENUMDRIVER
	  �EMESSAGE_GETDRIVERTUNINGSPACELIST
	  �EMESSAGE_FREEDRIVERTUNINGSPACELIST
	�EChannelInfo �\���̂� Flags �����o��ǉ�����

	ver.0.0.11 (TVTest ver.0.7.6 or later)
	�E�ȉ��̃��b�Z�[�W��ǉ�����
	  �EMESSAGE_SETWINDOWMESSAGECALLBACK
	  �EMESSAGE_REGISTERCONTROLLER
	  �EMESSAGE_ONCOTROLLERBUTTONDOWN
	  �EMESSAGE_GETCONTROLLERSETTINGS
	�EEVENT_CONTROLLERFOCUS ��ǉ�����
	�E�v���O�C���̃t���O�� PLUGIN_FLAG_NOUNLOAD ��ǉ�����

	ver.0.0.10 (TVTest ver.0.7.0 or later)
	�E�ȉ��̃��b�Z�[�W��ǉ�����
	  �EMESSAGE_GETLOGO
	  �EMESSAGE_GETAVAILABLELOGOTYPE
	  �EMESSAGE_RELAYRECORD
	  �EMESSAGE_SILENTMODE
	�E�ȉ��̃C�x���g��ǉ�����
	  �EEVENT_CLOSE
	  �EEVENT_STARTRECORD
	  �EEVENT_RELAYRECORD
	�E�v���O�C���̃t���O�� PLUGIN_FLAG_DISABLEONSTART ��ǉ�����
	�ERecordStatusInfo �\���̂� pszFileName �� MaxFileName �����o��ǉ�����

	ver.0.0.9.1
	�E64�r�b�g�Ōx�����o�Ȃ��悤�ɂ���

	ver.0.0.9 (TVTest ver.0.6.2 or later)
	�EMESSAGE_GETSETTING �� MESSAGE_GETDRIVERFULLPATHNAME ��ǉ�����
	�EEVENT_SETTINGSCHANGE ��ǉ�����
	�EMESSAGE_RESET �Ƀp�����[�^��ǉ�����

	ver.0.0.8 (TVTest ver.0.6.0 or later)
	�E�ȉ��̃��b�Z�[�W��ǉ�����
	  �EMESSAGE_GETBCASINFO
	  �EMESSAGE_SENDBCASCOMMAND
	  �EMESSAGE_GETHOSTINFO
	�EMESSAGE_SETCHANNEL �̃p�����[�^�ɃT�[�r�XID��ǉ�����

	ver.0.0.7 (TVTest ver.0.5.45 or later)
	�EMESSAGE_DOCOMMAND ��ǉ�����

	ver.0.0.6 (TVTest ver.0.5.42 or later)
	�EMESSAGE_SETAUDIOCALLBACK ��ǉ�����
	�EEVENT_DSHOWINITIALIZED ��ǉ�����

	ver.0.0.5 (TVTest ver.0.5.41 or later)
	�E�ȉ��̃C�x���g��ǉ�����
	  �EEVENT_RESET
	  �EEVENT_STATUSRESET
	  �EEVENT_AUDIOSTREAMCHANGE
	�EMESSAGE_RESETSTATUS ��ǉ�����

	ver.0.0.4 (TVTest ver.0.5.33 or later)
	�EEVENT_EXECUTE ��ǉ�����

	ver.0.0.3 (TVTest ver.0.5.27 or later)
	�E�ȉ��̃��b�Z�[�W��ǉ�����
	  �EMESSAGE_ISPLUGINENABLED
	  �EMESSAGE_REGISTERCOMMAND
	  �EMESSAGE_ADDLOG
	�EEVENT_STANDBY �� EVENT_COMMAND ��ǉ�����

	ver.0.0.2
	�EMESSAGE_GETAUDIOSTREAM �� MESSAGE_SETAUDIOSTREAM ��ǉ�����
	�EServiceInfo �\���̂� AudioComponentType �� SubtitlePID �����o��ǉ�����
	�EStatusInfo �\���̂� DropPacketCount �� BcasCardStatus �����o��ǉ�����

	ver.0.0.1
	�E�ȉ��̃��b�Z�[�W��ǉ�����
	  �EMESSAGE_QUERYEVENT
	  �EMESSAGE_GETTUNINGSPACE
	  �EMESSAGE_GETTUNINGSPACEINFO
	  �EMESSAGE_SETNEXTCHANNEL
	�EChannelInfo �\���̂ɂ����������o��ǉ�����

	ver.0.0.0
	�E�ŏ��̃o�[�W����
*/


#ifndef TVTEST_PLUGIN_H
#define TVTEST_PLUGIN_H


#include <pshpack1.h>


namespace TVTest {


// �v���O�C���̃o�[�W����
#define TVTEST_PLUGIN_VERSION_(major,minor,rev) \
	(((major)<<24) | ((minor)<<12) | (rev))
#ifndef TVTEST_PLUGIN_VERSION
#define TVTEST_PLUGIN_VERSION TVTEST_PLUGIN_VERSION_(0,0,13)
#endif

// �G�N�X�|�[�g�֐���`�p
#define TVTEST_EXPORT(type) extern "C" __declspec(dllexport) type WINAPI

// offsetof
#define TVTEST_OFFSETOF(type,member) \
	((size_t)((BYTE*)&((type*)0)->member-(BYTE*)(type*)0))

// �v���O�C���̎��
enum {
	PLUGIN_TYPE_NORMAL	// ����
};

// �v���O�C���̃t���O
enum {
	PLUGIN_FLAG_HASSETTINGS		=0x00000001UL,	// �ݒ�_�C�A���O������
	PLUGIN_FLAG_ENABLEDEFAULT	=0x00000002UL	// �f�t�H���g�ŗL��
												// ���ʂȗ��R����������g��Ȃ�
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,10)
	,PLUGIN_FLAG_DISABLEONSTART	=0x00000004UL	// �N�����͕K������
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,11)
	,PLUGIN_FLAG_NOUNLOAD		=0x00000008UL	// �I�����ȊO�A�����[�h�s��
#endif
};

// �v���O�C���̏��
struct PluginInfo {
	DWORD Type;				// ���(PLUGIN_TYPE_???)
	DWORD Flags;			// �t���O(PLUGIN_FLAG_???)
	LPCWSTR pszPluginName;	// �v���O�C����
	LPCWSTR pszCopyright;	// ���쌠���
	LPCWSTR pszDescription;	// ������
};

// ���b�Z�[�W���M�p�R�[���o�b�N�֐�
typedef LRESULT (CALLBACK *MessageCallbackFunc)(struct PluginParam *pParam,UINT Message,LPARAM lParam1,LPARAM lParam2);

// �v���O�C���p�����[�^
struct PluginParam {
	MessageCallbackFunc Callback;	// �R�[���o�b�N�֐�
	HWND hwndApp;					// ���C���E�B���h�E�̃n���h��
	void *pClientData;				// �v���O�C�����ōD���Ɏg����f�[�^
	void *pInternalData;			// TVTest���Ŏg�p����f�[�^�B�A�N�Z�X�֎~
};

// �G�N�X�|�[�g�֐�
typedef DWORD (WINAPI *GetVersionFunc)();
typedef BOOL (WINAPI *GetPluginInfoFunc)(PluginInfo *pInfo);
typedef BOOL (WINAPI *InitializeFunc)(PluginParam *pParam);
typedef BOOL (WINAPI *FinalizeFunc)();

// ���b�Z�[�W
enum {
	MESSAGE_GETVERSION,					// �v���O�����̃o�[�W�������擾
	MESSAGE_QUERYMESSAGE,				// ���b�Z�[�W�ɑΉ����Ă��邩�₢���킹��
	MESSAGE_MEMORYALLOC,				// �������m��
	MESSAGE_SETEVENTCALLBACK,			// �C�x���g�n���h���p�R�[���o�b�N�̐ݒ�
	MESSAGE_GETCURRENTCHANNELINFO,		// ���݂̃`�����l���̏����擾
	MESSAGE_SETCHANNEL,					// �`�����l����ݒ�
	MESSAGE_GETSERVICE,					// �T�[�r�X���擾
	MESSAGE_SETSERVICE,					// �T�[�r�X��ݒ�
	MESSAGE_GETTUNINGSPACENAME,			// �`���[�j���O��Ԗ����擾
	MESSAGE_GETCHANNELINFO,				// �`�����l���̏����擾
	MESSAGE_GETSERVICEINFO,				// �T�[�r�X�̏����擾
	MESSAGE_GETDRIVERNAME,				// BonDriver�̃t�@�C�������擾
	MESSAGE_SETDRIVERNAME,				// BonDriver��ݒ�
	MESSAGE_STARTRECORD,				// �^��̊J�n
	MESSAGE_STOPRECORD,					// �^��̒�~
	MESSAGE_PAUSERECORD,				// �^��̈ꎞ��~/�ĊJ
	MESSAGE_GETRECORD,					// �^��ݒ�̎擾
	MESSAGE_MODIFYRECORD,				// �^��ݒ�̕ύX
	MESSAGE_GETZOOM,					// �\���{���̎擾
	MESSAGE_SETZOOM,					// �\���{���̐ݒ�
	MESSAGE_GETPANSCAN,					// �p���X�L�����̐ݒ���擾
	MESSAGE_SETPANSCAN,					// �p���X�L������ݒ�
	MESSAGE_GETSTATUS,					// �X�e�[�^�X���擾
	MESSAGE_GETRECORDSTATUS,			// �^��X�e�[�^�X���擾
	MESSAGE_GETVIDEOINFO,				// �f���̏����擾
	MESSAGE_GETVOLUME,					// ���ʂ��擾
	MESSAGE_SETVOLUME,					// ���ʂ�ݒ�
	MESSAGE_GETSTEREOMODE,				// �X�e���I���[�h���擾
	MESSAGE_SETSTEREOMODE,				// �X�e���I���[�h��ݒ�
	MESSAGE_GETFULLSCREEN,				// �S��ʕ\���̏�Ԃ��擾
	MESSAGE_SETFULLSCREEN,				// �S��ʕ\���̏�Ԃ�ݒ�
	MESSAGE_GETPREVIEW,					// �Đ����L�����擾
	MESSAGE_SETPREVIEW,					// �Đ��̗L����Ԃ�ݒ�
	MESSAGE_GETSTANDBY,					// �ҋ@��Ԃł��邩�擾
	MESSAGE_SETSTANDBY,					// �ҋ@��Ԃ�ݒ�
	MESSAGE_GETALWAYSONTOP,				// ��ɍőO�ʕ\���ł��邩�擾
	MESSAGE_SETALWAYSONTOP,				// ��ɍőO�ʕ\����ݒ�
	MESSAGE_CAPTUREIMAGE,				// �摜���L���v�`������
	MESSAGE_SAVEIMAGE,					// �摜��ۑ�����
	MESSAGE_RESET,						// ���Z�b�g���s��
	MESSAGE_CLOSE,						// �E�B���h�E�����
	MESSAGE_SETSTREAMCALLBACK,			// �X�g���[���R�[���o�b�N��ݒ�
	MESSAGE_ENABLEPLUGIN,				// �v���O�C���̗L����Ԃ�ݒ�
	MESSAGE_GETCOLOR,					// �F�̐ݒ���擾
	MESSAGE_DECODEARIBSTRING,			// ARIB������̃f�R�[�h
	MESSAGE_GETCURRENTPROGRAMINFO,		// ���݂̔ԑg�̏����擾
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,1)
	MESSAGE_QUERYEVENT,					// �C�x���g�ɑΉ����Ă��邩�擾
	MESSAGE_GETTUNINGSPACE,				// ���݂̃`���[�j���O��Ԃ��擾
	MESSAGE_GETTUNINGSPACEINFO,			// �`���[�j���O��Ԃ̏����擾
	MESSAGE_SETNEXTCHANNEL,				// �`�����l�������ɐݒ肷��
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,2)
	MESSAGE_GETAUDIOSTREAM,				// �����X�g���[�����擾
	MESSAGE_SETAUDIOSTREAM,				// �����X�g���[����ݒ�
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,3)
	MESSAGE_ISPLUGINENABLED,			// �v���O�C���̗L����Ԃ��擾
	MESSAGE_REGISTERCOMMAND,			// �R�}���h�̓o�^
	MESSAGE_ADDLOG,						// ���O���L�^
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,5)
	MESSAGE_RESETSTATUS,				// �X�e�[�^�X��������
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,6)
	MESSAGE_SETAUDIOCALLBACK,			// �����̃R�[���o�b�N�֐���ݒ�
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,7)
	MESSAGE_DOCOMMAND,					// �R�}���h�̎��s
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,8)
	MESSAGE_GETBCASINFO,				// B-CAS �J�[�h�̏����擾
	MESSAGE_SENDBCASCOMMAND,			// B-CAS �J�[�h�ɃR�}���h�𑗐M
	MESSAGE_GETHOSTINFO,				// �z�X�g�v���O�����̏����擾
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,9)
	MESSAGE_GETSETTING,					// �ݒ�̎擾
	MESSAGE_GETDRIVERFULLPATHNAME,		// BonDriver�̃t���p�X���擾
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,10)
	MESSAGE_GETLOGO,					// ���S�̎擾
	MESSAGE_GETAVAILABLELOGOTYPE,		// ���p�\�ȃ��S�̎擾
	MESSAGE_RELAYRECORD,				// �^��t�@�C���̐؂�ւ�
	MESSAGE_SILENTMODE,					// �T�C�����g���[�h�̎擾/�ݒ�
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,11)
	MESSAGE_SETWINDOWMESSAGECALLBACK,	// �E�B���h�E���b�Z�[�W�R�[���o�b�N�̐ݒ�
	MESSAGE_REGISTERCONTROLLER,			// �R���g���[���̓o�^
	MESSAGE_ONCONTROLLERBUTTONDOWN,		// �R���g���[���̃{�^���������ꂽ�̂�ʒm
	MESSAGE_GETCONTROLLERSETTINGS,		// �R���g���[���̐ݒ���擾
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,12)
	MESSAGE_GETEPGEVENTINFO,			// �ԑg�����擾
	MESSAGE_FREEEPGEVENTINFO,			// �ԑg�������
	MESSAGE_GETEPGEVENTLIST,			// �ԑg�̃��X�g���擾
	MESSAGE_FREEEPGEVENTLIST,			// �ԑg�̃��X�g�����
	MESSAGE_ENUMDRIVER,					// BonDriver�̗�
	MESSAGE_GETDRIVERTUNINGSPACELIST,	// BonDriver�̃`���[�j���O��Ԃ̃��X�g�̎擾
	MESSAGE_FREEDRIVERTUNINGSPACELIST,	// BonDriver�̃`���[�j���O��Ԃ̃��X�g�̉��
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,13)
	MESSAGE_ENABLEPROGRAMGUIDEEVENT,	// �ԑg�\�̃C�x���g�̗L��/������ݒ肷��
	MESSAGE_REGISTERPROGRAMGUIDECOMMAND,	// �ԑg�\�̃R�}���h��o�^
#endif
	MESSAGE_TRAILER
};

// �C�x���g�p�R�[���o�b�N�֐�
typedef LRESULT (CALLBACK *EventCallbackFunc)(UINT Event,LPARAM lParam1,LPARAM lParam2,void *pClientData);

// �C�x���g
// �e�C�x���g�������̃p�����[�^�� CTVTestEventHadler ���Q�Ƃ��Ă��������B
enum {
	EVENT_PLUGINENABLE,			// �L����Ԃ��ω�����
	EVENT_PLUGINSETTINGS,		// �ݒ���s��
	EVENT_CHANNELCHANGE,		// �`�����l�����ύX���ꂽ
	EVENT_SERVICECHANGE,		// �T�[�r�X���ύX���ꂽ
	EVENT_DRIVERCHANGE,			// �h���C�o���ύX���ꂽ
	EVENT_SERVICEUPDATE,		// �T�[�r�X�̍\�����ω�����
	EVENT_RECORDSTATUSCHANGE,	// �^���Ԃ��ω�����
	EVENT_FULLSCREENCHANGE,		// �S��ʕ\����Ԃ��ω�����
	EVENT_PREVIEWCHANGE,		// �v���r���[�\����Ԃ��ω�����
	EVENT_VOLUMECHANGE,			// ���ʂ��ω�����
	EVENT_STEREOMODECHANGE,		// �X�e���I���[�h���ω�����
	EVENT_COLORCHANGE,			// �F�̐ݒ肪�ω�����
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,3)
	EVENT_STANDBY,				// �ҋ@��Ԃ��ω�����
	EVENT_COMMAND,				// �R�}���h���I�����ꂽ
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,4)
	EVENT_EXECUTE,				// �����N���֎~���ɕ����N�����ꂽ
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,5)
	EVENT_RESET,				// ���Z�b�g���ꂽ
	EVENT_STATUSRESET,			// �X�e�[�^�X�����Z�b�g���ꂽ
	EVENT_AUDIOSTREAMCHANGE,	// �����X�g���[�����ύX���ꂽ
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,9)
	EVENT_SETTINGSCHANGE,		// �ݒ肪�ύX���ꂽ
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,10)
	EVENT_CLOSE,				// TVTest�̃E�B���h�E��������
	EVENT_STARTRECORD,			// �^�悪�J�n�����
	EVENT_RELAYRECORD,			// �^��t�@�C�����؂�ւ���ꂽ
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,11)
	EVENT_CONTROLLERFOCUS,		// �R���g���[���̑Ώۂ�ݒ�
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,13)
	EVENT_STARTUPDONE,			// �N�����̏������I�����
	// �ԑg�\�֌W�̃C�x���g�́AMESSAGE_ENABLEPROGRAMGUIDEEVENT ���Ă�ŗL���ɂ��Ȃ��ƒʒm����܂���
	EVENT_PROGRAMGUIDE_INITIALIZE,				// �ԑg�\�̏�����
	EVENT_PROGRAMGUIDE_FINALIZE,				// �ԑg�\�̏I��
	EVENT_PROGRAMGUIDE_COMMAND,					// �ԑg�\�̃R�}���h���s
	EVENT_PROGRAMGUIDE_INITIALIZEMENU,			// �ԑg�\�̃��j���[�̐ݒ�
	EVENT_PROGRAMGUIDE_MENUSELECTED,			// �ԑg�\�̃��j���[���I�����ꂽ
	EVENT_PROGRAMGUIDE_PROGRAM_DRAWBACKGROUND,	// �ԑg�\�̔ԑg�̔w�i��`��
	EVENT_PROGRAMGUIDE_PROGRAM_INITIALIZEMENU,	// �ԑg�\�̔ԑg�̃��j���[�̐ݒ�
	EVENT_PROGRAMGUIDE_PROGRAM_MENUSELECTED,	// �ԑg�\�̔ԑg�̃��j���[���I�����ꂽ
#endif
	EVENT_TRAILER
};

inline DWORD MakeVersion(BYTE Major,WORD Minor,WORD Build) {
	return ((DWORD)Major<<24) | ((DWORD)Minor<<12) | Build;
}
inline DWORD GetMajorVersion(DWORD Version) { return Version>>24; }
inline DWORD GetMinorVersion(DWORD Version) { return (Version&0x00FFF000UL)>>12; }
inline DWORD GetBuildVersion(DWORD Version) { return Version&0x00000FFFUL; }

// �v���O����(TVTest)�̃o�[�W�������擾����
// ���8�r�b�g�����W���[�o�[�W�����A����12�r�b�g���}�C�i�[�o�[�W�����A
// ����12�r�b�g���r���h�i���o�[
// GetMajorVersion/GetMinorVersion/GetBuildVersion���g���Ď擾�ł���
inline DWORD MsgGetVersion(PluginParam *pParam) {
	return (DWORD)(*pParam->Callback)(pParam,MESSAGE_GETVERSION,0,0);
}

// �w�肳�ꂽ���b�Z�[�W�ɑΉ����Ă��邩�₢���킹��
inline bool MsgQueryMessage(PluginParam *pParam,UINT Message) {
	return (*pParam->Callback)(pParam,MESSAGE_QUERYMESSAGE,Message,0)!=0;
}

// �������Ċm��
// �d�l��realloc�Ɠ���
inline void *MsgMemoryReAlloc(PluginParam *pParam,void *pData,DWORD Size) {
	return (void*)(*pParam->Callback)(pParam,MESSAGE_MEMORYALLOC,(LPARAM)pData,Size);
}

// �������m��
// �d�l��realloc(NULL,Size)�Ɠ���
inline void *MsgMemoryAlloc(PluginParam *pParam,DWORD Size) {
	return (void*)(*pParam->Callback)(pParam,MESSAGE_MEMORYALLOC,(LPARAM)(void*)NULL,Size);
}

// �������J��
// �d�l��realloc(pData,0)�Ɠ���
// (���ۂ�realloc�Ń������J�����Ă���R�[�h�͌������Ɩ�������...)
inline void MsgMemoryFree(PluginParam *pParam,void *pData) {
	(*pParam->Callback)(pParam,MESSAGE_MEMORYALLOC,(LPARAM)pData,0);
}

// �C�x���g�n���h���p�R�[���o�b�N�̐ݒ�
// pClientData �̓R�[���o�b�N�̌Ăяo�����ɓn����܂��B
// ��̃v���O�C���Őݒ�ł���R�[���o�b�N�֐��͈�����ł��B
// Callback �� NULL ��n���Ɛݒ肪��������܂��B
inline bool MsgSetEventCallback(PluginParam *pParam,EventCallbackFunc Callback,void *pClientData=NULL) {
	return (*pParam->Callback)(pParam,MESSAGE_SETEVENTCALLBACK,(LPARAM)Callback,(LPARAM)pClientData)!=0;
}

// �`�����l���̏��
struct ChannelInfo {
	DWORD Size;							// �\���̂̃T�C�Y
	int Space;							// �`���[�j���O���(BonDriver�̃C���f�b�N�X)
	int Channel;						// �`�����l��(BonDriver�̃C���f�b�N�X)
	int RemoteControlKeyID;				// �����R��ID
	WORD NetworkID;						// �l�b�g���[�NID
	WORD TransportStreamID;				// �g�����X�|�[�g�X�g���[��ID
	WCHAR szNetworkName[32];			// �l�b�g���[�N��
	WCHAR szTransportStreamName[32];	// �g�����X�|�[�g�X�g���[����
	WCHAR szChannelName[64];			// �`�����l����
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,1)
	int PhysicalChannel;				// �����`�����l���ԍ�(���܂�M�p�ł��Ȃ�)
										// �s���̏ꍇ��0
	WORD ServiceIndex;					// �T�[�r�X�̃C���f�b�N�X
										// (���݂͈Ӗ��𖳂����Ă���̂Ŏg��Ȃ�)
	WORD ServiceID;						// �T�[�r�XID
	// �T�[�r�X�̓`�����l���t�@�C���Őݒ肳��Ă�����̂��擾�����
	// �T�[�r�X�̓��[�U�[���؂�ւ�����̂ŁA���ۂɎ������̃T�[�r�X������ł���Ƃ͌���Ȃ�
	// ���ۂɎ������̃T�[�r�X�� MESSAGE_GETSERVICE �Ŏ擾�ł���
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,12)
	DWORD Flags;						// �e��t���O(CHANNEL_FLAG_*)
#endif
};

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,12)
// �`�����l���̏��̃t���O
enum {
	CHANNEL_FLAG_DISABLED	=0x00000001UL	// �����ɂ���Ă���
};
#endif

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,1)
enum {
	CHANNELINFO_SIZE_V1=TVTEST_OFFSETOF(ChannelInfo,PhysicalChannel)
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,12)
	, CHANNELINFO_SIZE_V2=TVTEST_OFFSETOF(ChannelInfo,Flags)
#endif
};
#endif

// ���݂̃`�����l���̏����擾����
// ���O�� ChannelInfo �� Size �����o��ݒ肵�Ă����܂��B
inline bool MsgGetCurrentChannelInfo(PluginParam *pParam,ChannelInfo *pInfo) {
	return (*pParam->Callback)(pParam,MESSAGE_GETCURRENTCHANNELINFO,(LPARAM)pInfo,0)!=0;
}

// �`�����l����ݒ肷��
#if TVTEST_PLUGIN_VERSION<TVTEST_PLUGIN_VERSION_(0,0,8)
inline bool MsgSetChannel(PluginParam *pParam,int Space,int Channel) {
	return (*pParam->Callback)(pParam,MESSAGE_SETCHANNEL,Space,Channel)!=0;
}
#else
inline bool MsgSetChannel(PluginParam *pParam,int Space,int Channel,WORD ServiceID=0) {
	return (*pParam->Callback)(pParam,MESSAGE_SETCHANNEL,Space,MAKELPARAM((SHORT)Channel,ServiceID))!=0;
}
#endif

// ���݂̃T�[�r�X�y�уT�[�r�X�����擾����
// �T�[�r�X�̃C���f�b�N�X���Ԃ�B�G���[����-1���Ԃ�܂��B
// pNumServices �� NULL �łȂ��ꍇ�́A�T�[�r�X�̐����Ԃ���܂��B
inline int MsgGetService(PluginParam *pParam,int *pNumServices=NULL) {
	return (int)(*pParam->Callback)(pParam,MESSAGE_GETSERVICE,(LPARAM)pNumServices,0);
}

// �T�[�r�X��ݒ肷��
// fByID=false �̏ꍇ�̓C���f�b�N�X�AfByID=true�̏ꍇ�̓T�[�r�XID
inline bool MsgSetService(PluginParam *pParam,int Service,bool fByID=false) {
	return (*pParam->Callback)(pParam,MESSAGE_SETSERVICE,Service,fByID)!=0;
}

// �`���[�j���O��Ԗ����擾����
// �`���[�j���O��Ԗ��̒������Ԃ�܂��BIndex���͈͊O�̏ꍇ��0���Ԃ�܂��B
// pszName �� NULL �ŌĂׂΒ����������擾�ł��܂��B
// MaxLength �ɂ� pszName �̐�Ɋi�[�ł���ő�̗v�f��(�I�[�̋󕶎����܂�)���w�肵�܂��B
inline int MsgGetTuningSpaceName(PluginParam *pParam,int Index,LPWSTR pszName,int MaxLength) {
	return (int)(*pParam->Callback)(pParam,MESSAGE_GETTUNINGSPACENAME,(LPARAM)pszName,MAKELPARAM(Index,min(MaxLength,0xFFFF)));
}

// �`�����l���̏����擾����
// ���O�� ChannelInfo �� Size �����o��ݒ肵�Ă����܂��B
// szNetworkName,szTransportStreamName �� MESSAGE_GETCURRENTCHANNEL �ł����擾�ł��܂���B
// NetworkID,TransportStreamID �̓`�����l���X�L�������Ă��Ȃ��Ǝ擾�ł��܂���B
// �擾�ł��Ȃ������ꍇ��0�ɂȂ�܂��B
inline bool MsgGetChannelInfo(PluginParam *pParam,int Space,int Index,ChannelInfo *pInfo) {
	return (*pParam->Callback)(pParam,MESSAGE_GETCHANNELINFO,(LPARAM)pInfo,MAKELPARAM(Space,Index))!=0;
}

// �T�[�r�X�̏��
struct ServiceInfo {
	DWORD Size;					// �\���̂̃T�C�Y
	WORD ServiceID;				// �T�[�r�XID
	WORD VideoPID;				// �r�f�I�X�g���[����PID
	int NumAudioPIDs;			// ����PID�̐�
	WORD AudioPID[4];			// �����X�g���[����PID
	WCHAR szServiceName[32];	// �T�[�r�X��
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,2)
	BYTE AudioComponentType[4];	// �����R���|�[�l���g�^�C�v
	WORD SubtitlePID;			// �����X�g���[����PID(�����ꍇ��0)
	WORD Reserved;				// �\��
#endif
};

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,2)
enum { SERVICEINFO_SIZE_V1=TVTEST_OFFSETOF(ServiceInfo,AudioComponentType) };
#endif

// �T�[�r�X�̏����擾����
// ���݂̃`�����l���̃T�[�r�X�̏����擾���܂��B
// ���O�� ServiceInfo �� Size �����o��ݒ肵�Ă����܂��B
inline bool MsgGetServiceInfo(PluginParam *pParam,int Index,ServiceInfo *pInfo) {
	return (*pParam->Callback)(pParam,MESSAGE_GETSERVICEINFO,Index,(LPARAM)pInfo)!=0;
}

// BonDriver�̃t�@�C�������擾����
// �߂�l�̓t�@�C�����̒���(�I�[��Null������)���Ԃ�܂��B
// pszName �� NULL �ŌĂׂΒ����������擾�ł��܂��B
// �擾�����̂́A�f�B���N�g�����܂܂Ȃ��t�@�C�����݂̂��A���΃p�X�̏ꍇ������܂��B
// �t���p�X���擾�������ꍇ�� MsgGetDriverFullPathName ���g�p���Ă��������B
inline int MsgGetDriverName(PluginParam *pParam,LPWSTR pszName,int MaxLength) {
	return (int)(*pParam->Callback)(pParam,MESSAGE_GETDRIVERNAME,(LPARAM)pszName,MaxLength);
}

// BonDriver��ݒ肷��
// �t�@�C�����݂̂����΃p�X���w�肷��ƁABonDriver �����t�H���_�̐ݒ肪�g�p����܂��B
inline bool MsgSetDriverName(PluginParam *pParam,LPCWSTR pszName) {
	return (*pParam->Callback)(pParam,MESSAGE_SETDRIVERNAME,(LPARAM)pszName,0)!=0;
}

// �^����̃}�X�N
enum {
	RECORD_MASK_FLAGS		=0x00000001UL,	// Flags ���L��
	RECORD_MASK_FILENAME	=0x00000002UL,	// pszFileName ���L��
	RECORD_MASK_STARTTIME	=0x00000004UL,	// StartTime ���L��
	RECORD_MASK_STOPTIME	=0x00000008UL	// StopTime ���L��
};

// �^��t���O
enum {
	RECORD_FLAG_CANCEL		=0x10000000UL	// �L�����Z��
};

// �^��J�n���Ԃ̎w����@
enum {
	RECORD_START_NOTSPECIFIED,	// ���w��
	RECORD_START_TIME,			// �����w��
	RECORD_START_DELAY			// �����w��
};

// �^���~���Ԃ̎w����@
enum {
	RECORD_STOP_NOTSPECIFIED,	// ���w��
	RECORD_STOP_TIME,			// �����w��
	RECORD_STOP_DURATION		// �����w��
};

// �^����
struct RecordInfo {
	DWORD Size;				// �\���̂̃T�C�Y
	DWORD Mask;				// �}�X�N(RECORD_MASK_???)
	DWORD Flags;			// �t���O(RECORD_FLAG_???)
	LPWSTR pszFileName;		// �t�@�C����(NULL�Ńf�t�H���g)
							// %�`% �ň͂܂ꂽ�u���L�[���[�h���g�p�ł��܂�
	int MaxFileName;		// �t�@�C�����̍ő咷(MESSAGE_GETRECORD�݂̂Ŏg�p)
	FILETIME ReserveTime;	// �^��\�񂳂ꂽ����(MESSAGE_GETRECORD�݂̂Ŏg�p)
	DWORD StartTimeSpec;	// �^��J�n���Ԃ̎w����@(RECORD_START_???)
	union {
		FILETIME Time;		// �^��J�n����(StartTimeSpec==RECORD_START_TIME)
							// ���[�J������
		ULONGLONG Delay;	// �^��J�n����(StartTimeSpec==RECORD_START_DELAY)
							// �^����J�n����܂ł̎���(ms)
	} StartTime;
	DWORD StopTimeSpec;		// �^���~���Ԃ̎w����@(RECORD_STOP_???)
	union {
		FILETIME Time;		// �^���~����(StopTimeSpec==RECORD_STOP_TIME)
							// ���[�J������
		ULONGLONG Duration;	// �^���~����(StopTimeSpec==RECORD_STOP_DURATION)
							// �J�n���Ԃ���̃~���b
	} StopTime;
};

// �^����J�n����
// pInfo �� NULL �ő����^��J�n���܂��B
inline bool MsgStartRecord(PluginParam *pParam,const RecordInfo *pInfo=NULL) {
	return (*pParam->Callback)(pParam,MESSAGE_STARTRECORD,(LPARAM)pInfo,0)!=0;
}

// �^����~����
inline bool MsgStopRecord(PluginParam *pParam) {
	return (*pParam->Callback)(pParam,MESSAGE_STOPRECORD,0,0)!=0;
}

// �^����ꎞ��~/�ĊJ����
inline bool MsgPauseRecord(PluginParam *pParam,bool fPause=true) {
	return (*pParam->Callback)(pParam,MESSAGE_PAUSERECORD,fPause,0)!=0;
}

// �^��ݒ���擾����
// ���O�� RecordInfo �� Size �����o��ݒ肵�Ă����܂��B
inline bool MsgGetRecord(PluginParam *pParam,RecordInfo *pInfo) {
	return (*pParam->Callback)(pParam,MESSAGE_GETRECORD,(LPARAM)pInfo,0)!=0;
}

// �^��ݒ��ύX����
// ���ɘ^�撆�ł���ꍇ�́A�t�@�C�����ƊJ�n���Ԃ̎w��͖�������܂��B
inline bool MsgModifyRecord(PluginParam *pParam,const RecordInfo *pInfo) {
	return (*pParam->Callback)(pParam,MESSAGE_MODIFYRECORD,(LPARAM)pInfo,0)!=0;
}

// �\���{�����擾����(%�P��)
inline int MsgGetZoom(PluginParam *pParam) {
	return (int)(*pParam->Callback)(pParam,MESSAGE_GETZOOM,0,0);
}

// �\���{����ݒ肷��
// %�P�ʂ����ł͂Ȃ��ANum=1/Denom=3�ȂǂƂ��Ċ���؂�Ȃ��{����ݒ肷�邱�Ƃ��ł��܂��B
inline bool MsgSetZoom(PluginParam *pParam,int Num,int Denom=100) {
	return (*pParam->Callback)(pParam,MESSAGE_SETZOOM,Num,Denom)!=0;
}

// �p���X�L�����̎��
enum {
	PANSCAN_NONE,		// �Ȃ�
	PANSCAN_LETTERBOX,	// ���^�[�{�b�N�X
	PANSCAN_SIDECUT,	// �T�C�h�J�b�g
	PANSCAN_SUPERFRAME	// ���z��
};

// �p���X�L�����̏��
struct PanScanInfo {
	DWORD Size;		// �\���̂̃T�C�Y
	int Type;		// ���(PANSCAN_???)
	int XAspect;	// �����A�X�y�N�g��
	int YAspect;	// �����A�X�y�N�g��
};

// �p���X�L�����̐ݒ���擾����
inline bool MsgGetPanScan(PluginParam *pParam,PanScanInfo *pInfo) {
	return (*pParam->Callback)(pParam,MESSAGE_GETPANSCAN,(LPARAM)pInfo,0)!=0;
}

// �p���X�L������ݒ肷��
inline bool MsgSetPanScan(PluginParam *pParam,const PanScanInfo *pInfo) {
	return (*pParam->Callback)(pParam,MESSAGE_SETPANSCAN,(LPARAM)pInfo,0)!=0;
}

// B-CAS �J�[�h�̏��
enum {
	BCAS_STATUS_OK,				// �G���[�Ȃ�
	BCAS_STATUS_NOTOPEN,		// �J����Ă��Ȃ�(�X�N�����u�������Ȃ�)
	BCAS_STATUS_NOCARDREADER,	// �J�[�h���[�_������
	BCAS_STATUS_NOCARD,			// �J�[�h���Ȃ�
	BCAS_STATUS_OPENERROR,		// �I�[�v���G���[
	BCAS_STATUS_TRANSMITERROR,	// �ʐM�G���[
	BCAS_STATUS_ESTABLISHERROR	// �R���e�L�X�g�m�����s
};

// �X�e�[�^�X���
struct StatusInfo {
	DWORD Size;							// �\���̂̃T�C�Y
	float SignalLevel;					// �M�����x��(dB)
	DWORD BitRate;						// �r�b�g���[�g(Bits/Sec)
	DWORD ErrorPacketCount;				// �G���[�p�P�b�g��
										// DropPacketCount ���܂܂��
	DWORD ScramblePacketCount;			// �����R��p�P�b�g��
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,2)
	DWORD DropPacketCount;				// �h���b�v�p�P�b�g��
	DWORD BcasCardStatus;				// B-CAS �J�[�h�̏��(BCAS_STATUS_???)
#endif
};

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,2)
enum { STATUSINFO_SIZE_V1=TVTEST_OFFSETOF(StatusInfo,DropPacketCount) };
#endif

// �X�e�[�^�X���擾����
// ���O��StatusInfo��Size�����o��ݒ肵�Ă����܂��B
inline bool MsgGetStatus(PluginParam *pParam,StatusInfo *pInfo) {
	return (*pParam->Callback)(pParam,MESSAGE_GETSTATUS,(LPARAM)pInfo,0)!=0;
}

// �^��̏��
enum {
	RECORD_STATUS_NOTRECORDING,	// �^�悵�Ă��Ȃ�
	RECORD_STATUS_RECORDING,	// �^�撆
	RECORD_STATUS_PAUSED		// �^��ꎞ��~��
};

// �^��X�e�[�^�X���
struct RecordStatusInfo {
	DWORD Size;				// �\���̂̃T�C�Y
	DWORD Status;			// ���(RECORD_STATUS_???)
	FILETIME StartTime;		// �^��J�n����(���[�J������)
	DWORD RecordTime;		// �^�掞��(ms) �ꎞ��~�����܂܂Ȃ�
	DWORD PauseTime;		// �ꎞ��~����(ms)
	DWORD StopTimeSpec;		// �^���~���Ԃ̎w����@(RECORD_STOP_???)
	union {
		FILETIME Time;		// �^���~�\�莞��(StopTimeSpec==RECORD_STOP_TIME)
							// (���[�J������)
		ULONGLONG Duration;	// �^���~�܂ł̎���(StopTimeSpec==RECORD_STOP_DURATION)
							// �J�n����(StartTime)����~���b�P��
	} StopTime;
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,10)
	LPWSTR pszFileName;		// �t�@�C���p�X
	int MaxFileName;		// �t�@�C���p�X�̍ő咷
	RecordStatusInfo() : pszFileName(NULL), MaxFileName(0) {}
#endif
};

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,10)
enum { RECORDSTATUSINFO_SIZE_V1=TVTEST_OFFSETOF(RecordStatusInfo,pszFileName) };
#endif

// �^��X�e�[�^�X���擾����
// ���O�� RecordStatusInfo �� Size �����o��ݒ肵�Ă����܂��B
// �t�@�C�������擾����ꍇ�� pszFileName �� MaxFileName �����o��ݒ肵�Ă����܂��B
inline bool MsgGetRecordStatus(PluginParam *pParam,RecordStatusInfo *pInfo) {
	return (*pParam->Callback)(pParam,MESSAGE_GETRECORDSTATUS,(LPARAM)pInfo,0)!=0;
}

// �f���̏��
struct VideoInfo {
	DWORD Size;			// �\���̂̃T�C�Y
	int Width;			// ��(�s�N�Z���P��)
	int Height;			// ����(�s�N�Z���P��)
	int XAspect;		// �����A�X�y�N�g��
	int YAspect;		// �����A�X�y�N�g��
	RECT SourceRect;	// �\�[�X�̕\���͈�
};

// �f���̏����擾����
// ���O��VideoInfo��Size�����o��ݒ肵�Ă����܂��B
inline bool MsgGetVideoInfo(PluginParam *pParam,VideoInfo *pInfo) {
	return (*pParam->Callback)(pParam,MESSAGE_GETVIDEOINFO,(LPARAM)pInfo,0)!=0;
}

// ���ʂ��擾����(0-100)
inline int MsgGetVolume(PluginParam *pParam) {
	return LOWORD((*pParam->Callback)(pParam,MESSAGE_GETVOLUME,0,0));
}

// ���ʂ�ݒ肷��(0-100)
inline bool MsgSetVolume(PluginParam *pParam,int Volume) {
	return (*pParam->Callback)(pParam,MESSAGE_SETVOLUME,Volume,0)!=0;
}

// ������Ԃł��邩�擾����
inline bool MsgGetMute(PluginParam *pParam) {
	return HIWORD((*pParam->Callback)(pParam,MESSAGE_GETVOLUME,0,0))!=0;
}

// ������Ԃ�ݒ肷��
inline bool MsgSetMute(PluginParam *pParam,bool fMute) {
	return (*pParam->Callback)(pParam,MESSAGE_SETVOLUME,-1,fMute)!=0;
}

// �X�e���I���[�h
enum {
	STEREOMODE_STEREO,	// �X�e���I
	STEREOMODE_LEFT,	// ��(�剹��)
	STEREOMODE_RIGHT	// �E(������)
};

// �X�e���I���[�h���擾����
inline int MsgGetStereoMode(PluginParam *pParam) {
	return (int)(*pParam->Callback)(pParam,MESSAGE_GETSTEREOMODE,0,0);
}

// �X�e���I���[�h��ݒ肷��
inline bool MsgSetStereoMode(PluginParam *pParam,int StereoMode) {
	return (*pParam->Callback)(pParam,MESSAGE_SETSTEREOMODE,StereoMode,0)!=0;
}

// �S��ʕ\���̏�Ԃ��擾����
inline bool MsgGetFullscreen(PluginParam *pParam) {
	return (*pParam->Callback)(pParam,MESSAGE_GETFULLSCREEN,0,0)!=0;
}

// �S��ʕ\���̏�Ԃ�ݒ肷��
inline bool MsgSetFullscreen(PluginParam *pParam,bool fFullscreen) {
	return (*pParam->Callback)(pParam,MESSAGE_SETFULLSCREEN,fFullscreen,0)!=0;
}

// �Đ����L���ł��邩�擾����
inline bool MsgGetPreview(PluginParam *pParam) {
	return (*pParam->Callback)(pParam,MESSAGE_GETPREVIEW,0,0)!=0;
}

// �Đ��̗L����Ԃ�ݒ肷��
inline bool MsgSetPreview(PluginParam *pParam,bool fPreview) {
	return (*pParam->Callback)(pParam,MESSAGE_SETPREVIEW,fPreview,0)!=0;
}

// �ҋ@��Ԃł��邩�擾����
inline bool MsgGetStandby(PluginParam *pParam) {
	return (*pParam->Callback)(pParam,MESSAGE_GETSTANDBY,0,0)!=0;
}

// �ҋ@��Ԃ�ݒ肷��
inline bool MsgSetStandby(PluginParam *pParam,bool fStandby) {
	return (*pParam->Callback)(pParam,MESSAGE_SETSTANDBY,fStandby,0)!=0;
}

// ��ɍőO�ʕ\���̏�Ԃ��擾����
inline bool MsgGetAlwaysOnTop(PluginParam *pParam) {
	return (*pParam->Callback)(pParam,MESSAGE_GETALWAYSONTOP,0,0)!=0;
}

// ��ɍőO�ʕ\���̏�Ԃ�ݒ肷��
inline bool MsgSetAlwaysOnTop(PluginParam *pParam,bool fAlwaysOnTop) {
	return (*pParam->Callback)(pParam,MESSAGE_SETALWAYSONTOP,fAlwaysOnTop,0)!=0;
}

// �摜���L���v�`������
// �߂�l�̓p�b�NDIB�f�[�^(BITMAPINFOHEADER + �s�N�Z���f�[�^)�ւ̃|�C���^�ł��B
// �s�v�ɂȂ����ꍇ��MsgMemoryFree�ŊJ�����܂��B
// �L���v�`���ł��Ȃ������ꍇ��NULL���Ԃ�܂��B
inline void *MsgCaptureImage(PluginParam *pParam,DWORD Flags=0) {
	return (void*)(*pParam->Callback)(pParam,MESSAGE_CAPTUREIMAGE,Flags,0);
}

// �摜��ۑ�����
inline bool MsgSaveImage(PluginParam *pParam) {
	return (*pParam->Callback)(pParam,MESSAGE_SAVEIMAGE,0,0)!=0;
}

// ���Z�b�g���s��
#if TVTEST_PLUGIN_VERSION<TVTEST_PLUGIN_VERSION_(0,0,9)
inline bool MsgReset(PluginParam *pParam) {
	return (*pParam->Callback)(pParam,MESSAGE_RESET,0,0)!=0;
}
#else
// ���Z�b�g�̃t���O
enum {
	RESET_ALL		= 0x00000000UL,	// �S��
	RESET_VIEWER	= 0x00000001UL	// �r���[�A�̂�
};

inline bool MsgReset(PluginParam *pParam,DWORD Flags=RESET_ALL) {
	return (*pParam->Callback)(pParam,MESSAGE_RESET,Flags,0)!=0;
}
#endif

// �E�B���h�E�N���[�Y�̃t���O
enum {
	CLOSE_EXIT	=0x00000001UL	// �K���I��������
};

// �E�B���h�E�����
inline bool MsgClose(PluginParam *pParam,DWORD Flags=0) {
	return (*pParam->Callback)(pParam,MESSAGE_CLOSE,Flags,0)!=0;
}

// �X�g���[���R�[���o�b�N�֐�
// pData �� 188 �o�C�g�� TS �p�P�b�g���n����܂��B
// FALSE ��Ԃ��ƃp�P�b�g���j������܂��B
typedef BOOL (CALLBACK *StreamCallbackFunc)(BYTE *pData,void *pClientData);

// �X�g���[���R�[���o�b�N�t���O
enum {
	STREAM_CALLBACK_REMOVE	=0x00000001UL	// �R�[���o�b�N�̍폜
};

// �X�g���[���R�[���o�b�N�̏��
struct StreamCallbackInfo {
	DWORD Size;						// �\���̂̃T�C�Y
	DWORD Flags;					// �t���O(STREAM_CALLBACK_???)
	StreamCallbackFunc Callback;	// �R�[���o�b�N�֐�
	void *pClientData;				// �R�[���o�b�N�֐��ɓn�����f�[�^
};

// �X�g���[���R�[���o�b�N��ݒ肷��
// �X�g���[���R�[���o�b�N��o�^����ƁATS �f�[�^���󂯎�邱�Ƃ��ł��܂��B
// �R�[���o�b�N�֐��͈�̃v���O�C���ŕ����ݒ�ł��܂��B
// �X�g���[���R�[���o�b�N�֐��ŏ������x������ƑS�̂��x������̂ŁA
// ���Ԃ��|���鏈���͕ʃX���b�h�ōs���Ȃǂ��Ă��������B
inline bool MsgSetStreamCallback(PluginParam *pParam,DWORD Flags,
					StreamCallbackFunc Callback,void *pClientData=NULL) {
	StreamCallbackInfo Info;
	Info.Size=sizeof(StreamCallbackInfo);
	Info.Flags=Flags;
	Info.Callback=Callback;
	Info.pClientData=pClientData;
	return (*pParam->Callback)(pParam,MESSAGE_SETSTREAMCALLBACK,(LPARAM)&Info,0)!=0;
}

// �v���O�C���̗L����Ԃ�ݒ肷��
inline bool MsgEnablePlugin(PluginParam *pParam,bool fEnable) {
	return (*pParam->Callback)(pParam,MESSAGE_ENABLEPLUGIN,fEnable,0)!=0;
}

// �F�̐ݒ���擾����
// pszColor �Ɏ擾�������F�̖��O���w�肵�܂��B
// ���O�͔z�F�ݒ�t�@�C��(*.httheme)�̍��ږ�("StatusBack" �Ȃ�)�Ɠ����ł��B
inline COLORREF MsgGetColor(PluginParam *pParam,LPCWSTR pszColor) {
	return (COLORREF)(*pParam->Callback)(pParam,MESSAGE_GETCOLOR,(LPARAM)pszColor,0);
}

// ARIB������̃f�R�[�h���
struct ARIBStringDecodeInfo {
	DWORD Size;				// �\���̂̃T�C�Y
	DWORD Flags;			// �t���O(���݂͏��0)
	const void *pSrcData;	// �ϊ����f�[�^
	DWORD SrcLength;		// �ϊ����T�C�Y(�o�C�g�P��)
	LPWSTR pszDest;			// �ϊ���o�b�t�@
	DWORD DestLength;		// �ϊ���o�b�t�@�̃T�C�Y(�����P��)
};

// ARIB��������f�R�[�h����
inline bool MsgDecodeARIBString(PluginParam *pParam,const void *pSrcData,
							DWORD SrcLength,LPWSTR pszDest,DWORD DestLength) {
	ARIBStringDecodeInfo Info;
	Info.Size=sizeof(ARIBStringDecodeInfo);
	Info.Flags=0;
	Info.pSrcData=pSrcData;
	Info.SrcLength=SrcLength;
	Info.pszDest=pszDest;
	Info.DestLength=DestLength;
	return (*pParam->Callback)(pParam,MESSAGE_DECODEARIBSTRING,(LPARAM)&Info,0)!=0;
}

// �ԑg�̏��
struct ProgramInfo {
	DWORD Size;				// �\���̂̃T�C�Y
	WORD ServiceID;			// �T�[�r�XID
	WORD EventID;			// �C�x���gID
	LPWSTR pszEventName;	// �C�x���g��
	int MaxEventName;		// �C�x���g���̍ő咷
	LPWSTR pszEventText;	// �C�x���g�e�L�X�g
	int MaxEventText;		// �C�x���g�e�L�X�g�̍ő咷
	LPWSTR pszEventExtText;	// �ǉ��C�x���g�e�L�X�g
	int MaxEventExtText;	// �ǉ��C�x���g�e�L�X�g�̍ő咷
	SYSTEMTIME StartTime;	// �J�n����(JST)
	DWORD Duration;			// ����(�b�P��)
};

// ���݂̔ԑg�̏����擾����
// ���O�� Size �����o�ɍ\���̂̃T�C�Y��ݒ肵�܂��B
// �܂��ApszEventName / pszEventText / pszEventExtText �����o�Ɏ擾��̃o�b�t�@�ւ̃|�C���^���A
// MaxEventName / MaxEventText / MaxEventExtText �����o�Ƀo�b�t�@�̒���(�v�f��)��ݒ肵�܂��B
// �K�v�̂Ȃ����́A�|�C���^�� NULL �ɂ���Ǝ擾����܂���B
// MsgGetEpgEventInfo �ŁA���ڂ����ԑg�����擾���邱�Ƃ��ł��܂��B
inline bool MsgGetCurrentProgramInfo(PluginParam *pParam,ProgramInfo *pInfo,bool fNext=false) {
	return (*pParam->Callback)(pParam,MESSAGE_GETCURRENTPROGRAMINFO,(LPARAM)pInfo,fNext)!=0;
}

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,1)

// �w�肳�ꂽ�C�x���g�̒ʒm�ɑΉ����Ă��邩�擾����
inline bool MsgQueryEvent(PluginParam *pParam,UINT Event) {
	return (*pParam->Callback)(pParam,MESSAGE_QUERYEVENT,Event,0)!=0;
}

// ���݂̃`���[�j���O��ԋy�у`���[�j���O��Ԑ����擾����
inline int MsgGetTuningSpace(PluginParam *pParam,int *pNumSpaces=NULL) {
	return (int)(*pParam->Callback)(pParam,MESSAGE_GETTUNINGSPACE,(LPARAM)pNumSpaces,0);
}

// �`���[�j���O��Ԃ̎��
enum {
	TUNINGSPACE_UNKNOWN,		// �s��
	TUNINGSPACE_TERRESTRIAL,	// �n��f�W�^��
	TUNINGSPACE_BS,				// BS
	TUNINGSPACE_110CS			// 110�xCS
};

// �`���[�j���O��Ԃ̏��
struct TuningSpaceInfo {
	DWORD Size;			// �\���̂̃T�C�Y
	int Space;			// �`���[�j���O��Ԃ̎��(TUNINGSPACE_???)
						// �ꍇ�ɂ���Ă͐M�p�ł��Ȃ�
	WCHAR szName[64];	// �`���[�j���O��Ԗ�
};

// �`���[�j���O��Ԃ̏����擾����
// ���O�� TuningSpaceInfo �� Size �����o��ݒ肵�Ă����܂��B
inline bool MsgGetTuningSpaceInfo(PluginParam *pParam,int Index,TuningSpaceInfo *pInfo) {
	return (*pParam->Callback)(pParam,MESSAGE_GETTUNINGSPACEINFO,Index,(LPARAM)pInfo)!=0;
}

// �`�����l�������ɐݒ肷��
inline bool MsgSetNextChannel(PluginParam *pParam,bool fNext=true)
{
	return (*pParam->Callback)(pParam,MESSAGE_SETNEXTCHANNEL,fNext,0)!=0;
}

#endif	// TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,1)

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,2)

// ���݂̉����X�g���[�����擾����
// �����X�g���[���̐��� MESSAGE_GETSERVICEINFO �Ŏ擾�ł��܂��B
inline int MsgGetAudioStream(PluginParam *pParam)
{
	return (int)(*pParam->Callback)(pParam,MESSAGE_GETAUDIOSTREAM,0,0);
}

// �����X�g���[����ݒ肷��
inline bool MsgSetAudioStream(PluginParam *pParam,int Index)
{
	return (*pParam->Callback)(pParam,MESSAGE_SETAUDIOSTREAM,Index,0)!=0;
}

#endif	// TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,2)

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,3)

// �v���O�C���̗L����Ԃ��擾����
inline bool MsgIsPluginEnabled(PluginParam *pParam)
{
	return (*pParam->Callback)(pParam,MESSAGE_ISPLUGINENABLED,0,0)!=0;
}

// �R�}���h�̏��
struct CommandInfo {
	int ID;				// ���ʎq
	LPCWSTR pszText;	// �R�}���h�̕�����
	LPCWSTR pszName;	// �R�}���h�̖��O
};

// �R�}���h��o�^����
// TVTInitialize ���ŌĂт܂��B
// �R�}���h��o�^����ƁA�V���[�g�J�b�g�L�[�⃊���R���ɋ@�\�����蓖�Ă���悤�ɂȂ�܂��B
// �ȉ��̂悤�Ɏg�p���܂��B
// MsgRegisterCommand(pParam, ID_MYCOMMAND, L"MyCommand", L"���̃R�}���h");
// �R�}���h�����s������ EVENT_COMMAND �C�x���g�������܂��B
// ���̍ہA�p�����[�^�Ƃ��Ď��ʎq���n����܂��B
inline bool MsgRegisterCommand(PluginParam *pParam,int ID,LPCWSTR pszText,LPCWSTR pszName)
{
	CommandInfo Info;
	Info.ID=ID;
	Info.pszText=pszText;
	Info.pszName=pszName;
	return (*pParam->Callback)(pParam,MESSAGE_REGISTERCOMMAND,(LPARAM)&Info,1)!=0;
}

// �R�}���h��o�^����
// TVTInitialize ���ŌĂт܂��B
inline bool MsgRegisterCommand(PluginParam *pParam,const CommandInfo *pCommandList,int NumCommands)
{
	return (*pParam->Callback)(pParam,MESSAGE_REGISTERCOMMAND,(LPARAM)pCommandList,NumCommands)!=0;
}

// ���O���L�^����
// �ݒ�̃��O�̍��ڂɕ\������܂��B
inline bool MsgAddLog(PluginParam *pParam,LPCWSTR pszText)
{
	return (*pParam->Callback)(pParam,MESSAGE_ADDLOG,(LPARAM)pszText,0)!=0;
}

#endif	// TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,3)

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,5)

// �X�e�[�^�X(MESSAGE_GETSTATUS �Ŏ擾�ł�����e)�����Z�b�g����
// ���Z�b�g���s����� EVENT_STATUSRESET �������܂��B
inline bool MsgResetStatus(PluginParam *pParam)
{
	return (*pParam->Callback)(pParam,MESSAGE_RESETSTATUS,0,0)!=0;
}

#endif	// TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,5)

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,6)

// �����T���v���̃R�[���o�b�N�֐�
// �n�����T���v���� 48kHz / 16�r�b�g�Œ�ł��B
// pData �̐�ɂ� Samples * Channels ���̃f�[�^�������Ă��܂��B
// ���̂Ƃ���A5.1ch���_�E���~�b�N�X����ݒ�ɂȂ��Ă���ꍇ
// �_�E���~�b�N�X���ꂽ�f�[�^���n����܂����A���̂����d�l��ς��邩���m��܂���B
// �߂�l�͍��̂Ƃ�����0��Ԃ��܂��B
typedef LRESULT (CALLBACK *AudioCallbackFunc)(short *pData,DWORD Samples,int Channels,void *pClientData);

// �����̃T���v�����擾����R�[���o�b�N�֐���ݒ肷��
// ��̃v���O�C���Őݒ�ł���R�[���o�b�N�֐��͈�����ł��B
// pClinetData �̓R�[���o�b�N�֐��ɓn����܂��B
// pCallback �� NULL ���w�肷��ƁA�ݒ肪��������܂��B
inline bool MsgSetAudioCallback(PluginParam *pParam,AudioCallbackFunc pCallback,void *pClientData=NULL)
{
	return (*pParam->Callback)(pParam,MESSAGE_SETAUDIOCALLBACK,(LPARAM)pCallback,(LPARAM)pClientData)!=0;
}

#endif	// TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,6)

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,7)

// �R�}���h�����s����
// ��������w�肵�ăR�}���h�����s���܂��B
// �R�}���h�� TVTest.ini �� [Accelerator] �Z�N�V������ Accel*_Command �̕�����Ɠ����ł��B
// �ꗗ�� TVTest �̃\�[�X�� Command.cpp �ɂ���܂��B
inline bool MsgDoCommand(PluginParam *pParam,LPCWSTR pszCommand)
{
	return (*pParam->Callback)(pParam,MESSAGE_DOCOMMAND,(LPARAM)pszCommand,0)!=0;
}

#endif	// TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,7)

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,8)

// B-CAS �̏��
struct BCasInfo {
	DWORD Size;						// �\���̂̃T�C�Y
	WORD CASystemID;				// CA_system_id
	BYTE CardID[6];					// �J�[�hID
	BYTE CardType;					// �J�[�h���
	BYTE MessagePartitionLength;	// ���b�Z�[�W������
	BYTE SystemKey[32];				// �V�X�e����
	BYTE InitialCBC[8];				// CBC�����l
	BYTE CardManufacturerID;		// ���[�J����
	BYTE CardVersion;				// �o�[�W����
	WORD CheckCode;					// �`�F�b�N�R�[�h
	char szFormatCardID[25];		// �ǌ`���̃J�[�hID (4���̐���x5)
};

// B-CAS �J�[�h�̏����擾����
// �J�[�h���J����Ă��Ȃ��ꍇ�� false ���Ԃ�܂��B
inline bool MsgGetBCasInfo(PluginParam *pParam,BCasInfo *pInfo)
{
	return (*pParam->Callback)(pParam,MESSAGE_GETBCASINFO,(LPARAM)pInfo,0)!=0;
}

// B-CAS �R�}���h�̏��
struct BCasCommandInfo {
	const BYTE *pSendData;	// ���M�f�[�^
	DWORD SendSize;			// ���M�T�C�Y (�o�C�g�P��)
	BYTE *pReceiveData;		// ��M�f�[�^
	DWORD ReceiveSize;		// ��M�T�C�Y (�o�C�g�P��)
};

// B-CAS �J�[�h�ɃR�}���h�𑗐M����
// BCasCommandInfo �� pSendData �ɑ��M�f�[�^�ւ̃|�C���^���w�肵�āA
// SendSize �ɑ��M�f�[�^�̃o�C�g����ݒ肵�܂��B
// �܂��ApReceiveData �Ɏ�M�f�[�^���i�[����o�b�t�@�ւ̃|�C���^���w�肵�āA
// ReceiveSize �Ƀo�b�t�@�Ɋi�[�ł���ő�T�C�Y��ݒ肵�܂��B
// ���M����������ƁAReceiveSize �Ɏ�M�����f�[�^�̃T�C�Y���Ԃ���܂��B
inline bool MsgSendBCasCommand(PluginParam *pParam,BCasCommandInfo *pInfo)
{
	return (*pParam->Callback)(pParam,MESSAGE_SENDBCASCOMMAND,(LPARAM)pInfo,0)!=0;
}

// B-CAS �J�[�h�ɃR�}���h�𑗐M����
inline bool MsgSendBCasCommand(PluginParam *pParam,const BYTE *pSendData,DWORD SendSize,BYTE *pReceiveData,DWORD *pReceiveSize)
{
	BCasCommandInfo Info;
	Info.pSendData=pSendData;
	Info.SendSize=SendSize;
	Info.pReceiveData=pReceiveData;
	Info.ReceiveSize=*pReceiveSize;
	if (!MsgSendBCasCommand(pParam,&Info))
		return false;
	*pReceiveSize=Info.ReceiveSize;
	return true;
}

// �z�X�g�v���O�����̏��
struct HostInfo {
	DWORD Size;						// �\���̂̃T�C�Y
	LPCWSTR pszAppName;				// �v���O������ ("TVTest"�A"TVH264" �Ȃ�)
	struct {
		int Major;					// ���W���[�o�[�W����
		int Minor;					// �}�C�i�[�o�[�W����
		int Build;					// �r���h�i���o�[
	} Version;
	LPCWSTR pszVersionText;			// �o�[�W���������� ("1.2.0" �Ȃ�)
	DWORD SupportedPluginVersion;	// �Ή����Ă���v���O�C���̃o�[�W����
									// TVTEST_PLUGIN_VERSION_(?,?,?)
};

// �z�X�g�v���O�����̏����擾����
// ���O�� HostInfo �� Size �����o�ɍ\���̂̃T�C�Y��ݒ肵�ČĂяo���܂��B
inline bool MsgGetHostInfo(PluginParam *pParam,HostInfo *pInfo)
{
	return (*pParam->Callback)(pParam,MESSAGE_GETHOSTINFO,(LPARAM)pInfo,0)!=0;
}

#endif	// TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,8)

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,9)

// �ݒ�̏��
struct SettingInfo {
	LPCWSTR pszName;		// �ݒ薼
	DWORD Type;				// �l�̌^ (SETTING_TYPE_???)
	union {
		int Int;			// int
		unsigned int UInt;	// unsigned int
		LPWSTR pszString;	// ������
		void *pData;		// �f�[�^
	} Value;
	DWORD ValueSize;		// �l�̃T�C�Y (�o�C�g�P��)
};

// �ݒ�̒l�̌^
enum SettingType {
	SETTING_TYPE_UNDEFINED,	// ����`
	SETTING_TYPE_INT,		// int
	SETTING_TYPE_UINT,		// unsigned int
	SETTING_TYPE_STRING,	// ������
	SETTING_TYPE_DATA		// �f�[�^
};

/*
	���̂Ƃ���ȉ��̐ݒ肪�擾�ł��܂��B
	�ݒ薼�̑啶���Ə������͋�ʂ���܂���B

	�ݒ薼                ���e                                �^
	DriverDirectory       BonDriver �̌����f�B���N�g��        ������
	IniFilePath           Ini �t�@�C���̃p�X                  ������
	RecordFolder          �^�掞�̕ۑ���t�H���_              ������
*/

// �ݒ���擾����
// �Ăяo���O�� SettingInfo �̃����o��ݒ肵�Ă����܂��B
// ������ƃf�[�^�̏ꍇ�AValueSize �ɐݒ�̊i�[�ɕK�v�ȃo�C�g�����Ԃ���܂��B
// �ʏ�͉��ɂ���^���ƂɃI�[�o�[���[�h���ꂽ�֐����g�p���������֗��ł��B
inline bool MsgGetSetting(PluginParam *pParam,SettingInfo *pInfo)
{
	return (*pParam->Callback)(pParam,MESSAGE_GETSETTING,(LPARAM)pInfo,0)!=0;
}

// int�̐ݒ���擾����
inline bool MsgGetSetting(PluginParam *pParam,LPCWSTR pszName,int *pValue)
{
	SettingInfo Info;
	Info.pszName=pszName;
	Info.Type=SETTING_TYPE_INT;
	Info.ValueSize=sizeof(int);
	if (!(*pParam->Callback)(pParam,MESSAGE_GETSETTING,(LPARAM)&Info,0))
		return false;
	*pValue=Info.Value.Int;
	return true;
}

// unsigned int�̐ݒ���擾����
inline bool MsgGetSetting(PluginParam *pParam,LPCWSTR pszName,unsigned int *pValue)
{
	SettingInfo Info;
	Info.pszName=pszName;
	Info.Type=SETTING_TYPE_UINT;
	Info.ValueSize=sizeof(unsigned int);
	if (!(*pParam->Callback)(pParam,MESSAGE_GETSETTING,(LPARAM)&Info,0))
		return false;
	*pValue=Info.Value.UInt;
	return true;
}

// ������̐ݒ���擾����
// �߂�l�͎擾����������̒���(�I�[��Null���܂�)�ł��B
// pszString �� NULL �ɂ���ƁA�K�v�ȃo�b�t�@�̒���(�I�[��Null���܂�)���Ԃ�܂��B
// �ݒ肪�擾�ł��Ȃ������ꍇ�� 0 ���Ԃ�܂��B
/*
	// ��
	WCHAR szIniPath[MAX_PATH];
	if (MsgGetSetting(pParam, L"IniFilePath", szIniPath, MAX_PATH) > 0) {
		// �Ăяo�������������ꍇ�́AszIniPath �� Ini �t�@�C���̃p�X���i�[����Ă��܂�
	}
*/
inline DWORD MsgGetSetting(PluginParam *pParam,LPCWSTR pszName,LPWSTR pszString,DWORD MaxLength)
{
	SettingInfo Info;
	Info.pszName=pszName;
	Info.Type=SETTING_TYPE_STRING;
	Info.Value.pszString=pszString;
	Info.ValueSize=MaxLength*sizeof(WCHAR);
	if (!(*pParam->Callback)(pParam,MESSAGE_GETSETTING,(LPARAM)&Info,0))
		return 0;
	return Info.ValueSize/sizeof(WCHAR);
}

// BonDriver�̃t���p�X�����擾����
// �߂�l�̓p�X�̒���(�I�[��Null������)���Ԃ�܂��B
// pszPaht��NULL�ŌĂׂΒ����������擾�ł��܂��B
inline int MsgGetDriverFullPathName(PluginParam *pParam,LPWSTR pszPath,int MaxLength)
{
	return (int)(*pParam->Callback)(pParam,MESSAGE_GETDRIVERFULLPATHNAME,(LPARAM)pszPath,MaxLength);
}

#endif	// TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,9)

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,10)

// ���S�̉摜���擾����
// LogoType��0����5�܂łŎw�肵�܂��B�ȉ��̃T�C�Y�̃��S���擾����܂��B
// 0 = 48x24 / 1 = 36x24 / 2 = 48x27 / 3 = 72x36 / 4 = 54x36 / 5 = 64x36
// ������̃��S��16:9�ŕ\������Ɩ{���̔䗦�ɂȂ�܂��B
// �摜���擾�ł����ꍇ�̓r�b�g�}�b�v(DIB�Z�N�V����)�̃n���h�����Ԃ�܂��B
// �摜���擾�ł��Ȃ��ꍇ��NULL���Ԃ�܂��B
// �r�b�g�}�b�v�͕s�v�ɂȂ�������DeleteObject()�Ŕj�����Ă��������B
inline HBITMAP MsgGetLogo(PluginParam *pParam,WORD NetworkID,WORD ServiceID,BYTE LogoType)
{
	return (HBITMAP)(*pParam->Callback)(pParam,MESSAGE_GETLOGO,MAKELONG(NetworkID,ServiceID),LogoType);
}

// ���p�\�ȃ��S�̎�ނ��擾����
// ���p�\�ȃ��S��\���t���O���Ԃ�܂��B
// ���ʂ���1�r�b�g���Ƃ�LogoType��0����5�܂ł�\���A�r�b�g��1�ł���΂��̎�ނ̃��S�����p�ł��܂��B
/*
	// ��
	if (MsgGetAvailableLogoType(pParam, NetworkID, ServiceID) & 1) {
		// �^�C�v0�̃��S�����p�ł���
	}
*/
inline UINT MsgGetAvailableLogoType(PluginParam *pParam,WORD NetworkID,WORD ServiceID)
{
	return (UINT)(*pParam->Callback)(pParam,MESSAGE_GETAVAILABLELOGOTYPE,MAKELONG(NetworkID,ServiceID),0);
}

// �^��t�@�C����؂�ւ���
// ���ݘ^�撆�̃t�@�C������āA�w�肳�ꂽ�p�X�Ƀt�@�C�����쐬���đ�����^�悷��悤�ɂ��܂��B
// �V�����t�@�C�����J���Ȃ������ꍇ�́A���܂ł̃t�@�C���Ř^�悪�p������܂��B
inline bool MsgRelayRecord(PluginParam *pParam,LPCWSTR pszFileName)
{
	return (*pParam->Callback)(pParam,MESSAGE_RELAYRECORD,(LPARAM)pszFileName,0)!=0;
}

// �T�C�����g���[�h�̃p�����[�^
enum {
	SILENTMODE_GET,	// �擾
	SILENTMODE_SET	// �ݒ�
};

// �T�C�����g���[�h�̎擾
// �T�C�����g���[�h�ł��邩�擾���܂��B
// �T�C�����g���[�h�ł̓G���[���ȂǂɃ_�C�A���O���o�Ȃ��Ȃ�܂��B
// �R�}���h���C���� /silent ���w�肷�邩�AMsgSetSilentMode �Őݒ肷��΃T�C�����g���[�h�ɂȂ�܂��B
inline bool MsgGetSilentMode(PluginParam *pParam)
{
	return (*pParam->Callback)(pParam,MESSAGE_SILENTMODE,SILENTMODE_GET,0)!=0;
}

// �T�C�����g���[�h�̐ݒ�
// �T�C�����g���[�h�̗L��/������ݒ肵�܂��B
inline bool MsgSetSilentMode(PluginParam *pParam,bool fSilent)
{
	return (*pParam->Callback)(pParam,MESSAGE_SILENTMODE,SILENTMODE_SET,(LPARAM)fSilent)!=0;
}

// �^��̃N���C�A���g
enum {
	RECORD_CLIENT_USER,			// ���[�U�[�̑���
	RECORD_CLIENT_COMMANDLINE,	// �R�}���h���C���ł̎w��
	RECORD_CLIENT_PLUGIN		// �v���O�C������̎w��
};

// �^��J�n���ŕύX��������
enum {
	STARTRECORD_MODIFIED_FILENAME	=0x00000001UL	// �t�@�C����
};

// �^��J�n���
struct StartRecordInfo {
	DWORD Size;				// �\���̂̃T�C�Y
	DWORD Flags;			// �t���O(���ݖ��g�p)
	DWORD Modified;			// �ύX��������(STARTRECORD_MODIFIED_???)
	DWORD Client;			// �^��̃N���C�A���g(RECORD_CLIENT_???)
	LPWSTR pszFileName;		// �t�@�C����
	int MaxFileName;		// �t�@�C�����̍ő咷
	DWORD StartTimeSpec;	// �J�n���Ԃ̎w����@(RECORD_START_???)
	FILETIME StartTime;		// �w�肳�ꂽ�J�n����(���[�J������)
							// StartTimeSpec!=RECORD_START_NOTSPECIFIED �̏ꍇ�̂ݗL��
	DWORD StopTimeSpec;		// ��~���Ԃ̎w����@(RECORD_STOP_???)
	union {
		FILETIME Time;		// ��~����(StopTimeSpec==RECORD_STOP_TIME)
							// ���[�J������
		ULONGLONG Duration;	// �^���~�܂ł̎���(StopTimeSpec==RECORD_STOP_DURATION)
							// �J�n��������̃~���b
	} StopTime;
};

#endif	// TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,10)

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,11)

/*
���b�Z�[�W�R�[���o�b�N�֐���o�^����ƁATVTest �̃��C���E�B���h�E�Ƀ��b�Z�[�W��
����ꂽ���ɌĂяo����܂��B
�R�[���o�b�N�֐��ł́ATVTest �Ƀ��b�Z�[�W�̏����������Ȃ����� TRUE ��Ԃ��܂��B
���̍ہApResult �ɏ������񂾒l���Ԃ���܂��B
*/

// �E�B���h�E���b�Z�[�W�R�[���o�b�N�֐�
typedef BOOL (CALLBACK *WindowMessageCallbackFunc)(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT *pResult,void *pUserData);

// �E�B���h�E���b�Z�[�W�R�[���o�b�N�̐ݒ�
// pClientData �̓R�[���o�b�N�̌Ăяo�����ɓn����܂��B
// ��̃v���O�C���Őݒ�ł���R�[���o�b�N�֐��͈�����ł��B
// Callback �� NULL ��n���Ɛݒ肪��������܂��B
inline bool MsgSetWindowMessageCallback(PluginParam *pParam,WindowMessageCallbackFunc Callback,void *pClientData=NULL) {
	return (*pParam->Callback)(pParam,MESSAGE_SETWINDOWMESSAGECALLBACK,(LPARAM)Callback,(LPARAM)pClientData)!=0;
}

/*
�R���g���[���̓o�^���s���ƁA�ݒ�_�C�A���O�̃����R���̃y�[�W�Ŋ��蓖�Ă��ݒ�ł���悤�ɂȂ�܂��B
�R���g���[���̉摜��p�ӂ���ƁA�ݒ�̉E���ɕ\������܂��B
�{�^���������ꂽ���� MsgOnControllerButtonDown ���Ăяo���Ēʒm����ƁA
���蓖�Ă̐ݒ�ɏ]���ċ@�\�����s����܂��B
*/

// �R���g���[���̃{�^���̏��
struct ControllerButtonInfo {
	LPCWSTR pszName;				// �{�^���̖���("�����ؑ�" �Ȃ�)
	LPCWSTR pszDefaultCommand;		// �f�t�H���g�̃R�}���h(MsgDoCommand �Ɠ�������)
									// �w�肵�Ȃ��ꍇ��NULL
	struct {
		WORD Left,Top,Width,Height;	// �摜�̃{�^���̈ʒu(�摜�������ꍇ�͖��������)
	} ButtonRect;
	struct {
		WORD Left,Top;				// �摜�̑I���{�^���̈ʒu(�摜�������ꍇ�͖��������)
	} SelButtonPos;
	DWORD Reserved;					// �\��̈�(0�ɂ��Ă�������)
};

// �R���g���[���̏��
struct ControllerInfo {
	DWORD Size;									// �\���̂̃T�C�Y
	DWORD Flags;								// �e��t���O(CONTROLLER_FLAG_???)
	LPCWSTR pszName;							// �R���g���[�����ʖ�
	LPCWSTR pszText;							// �R���g���[���̖���("HDUS�����R��" �Ȃ�)
	int NumButtons;								// �{�^���̐�
	const ControllerButtonInfo *pButtonList;	// �{�^���̃��X�g
	LPCWSTR pszIniFileName;						// �ݒ�t�@�C����(NULL �ɂ���� TVTest �� Ini �t�@�C��)
	LPCWSTR pszSectionName;						// �ݒ�̃Z�N�V������
	UINT ControllerImageID;						// �R���g���[���̉摜�̎��ʎq(�����ꍇ��0)
	UINT SelButtonsImageID;						// �I���{�^���摜�̎��ʎq(�����ꍇ��0)
	typedef BOOL (CALLBACK *TranslateMessageCallback)(HWND hwnd,MSG *pMessage,void *pClientData);
	TranslateMessageCallback pTranslateMessage;	// ���b�Z�[�W�̕ϊ��R�[���o�b�N(�K�v������� NULL)
	void *pClientData;							// �R�[���o�b�N�ɓn���p�����[�^
};

// �R���g���[���̃t���O
enum {
	CONTROLLER_FLAG_ACTIVEONLY	=0x00000001UL	// �A�N�e�B�u���̂ݎg�p�ł���
};

// �R���g���[����o�^����
inline bool MsgRegisterController(PluginParam *pParam,const ControllerInfo *pInfo)
{
	return (*pParam->Callback)(pParam,MESSAGE_REGISTERCONTROLLER,(LPARAM)pInfo,0)!=0;
}

// �R���g���[���̃{�^���������ꂽ���Ƃ�ʒm����
inline bool MsgOnControllerButtonDown(PluginParam *pParam,LPCWSTR pszName,int Button)
{
	return (*pParam->Callback)(pParam,MESSAGE_ONCONTROLLERBUTTONDOWN,(LPARAM)pszName,Button)!=0;
}

// �R���g���[���̐ݒ�
struct ControllerSettings {
	DWORD Mask;		// �擾���鍀��(CONTROLLER_SETTINGS_MASK_*)
	DWORD Flags;	// �e��t���O(CONTROLLER_SETTINGS_FLAG_*)
};

// �R���g���[���̐ݒ�}�X�N
enum {
	CONTROLLER_SETTINGS_MASK_FLAGS		=0x00000001UL	// Flags ���L��
};

// �R���g���[���̐ݒ�t���O
enum {
	CONTROLLER_SETTINGS_FLAG_ACTIVEONLY	=0x00000001UL	// �A�N�e�B�u���̂�
};

// �R���g���[���̐ݒ���擾����
inline bool MsgGetControllerSettings(PluginParam *pParam,LPCWSTR pszName,ControllerSettings *pSettings)
{
	return (*pParam->Callback)(pParam,MESSAGE_GETCONTROLLERSETTINGS,(LPARAM)pszName,(LPARAM)pSettings)!=0;
}

// �R���g���[�����A�N�e�B�u���݂̂ɐݒ肳��Ă��邩�擾����
inline bool MsgIsControllerActiveOnly(PluginParam *pParam,LPCWSTR pszName)
{
	ControllerSettings Settings;
	Settings.Mask=CONTROLLER_SETTINGS_MASK_FLAGS;
	if (!MsgGetControllerSettings(pParam,pszName,&Settings))
		return false;
	return (Settings.Flags&CONTROLLER_SETTINGS_FLAG_ACTIVEONLY)!=0;
}

#endif	// TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,11)

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,12)

// �C�x���g�̎擾���@
enum {
	EPG_EVENT_QUERY_EVENTID,	// �C�x���gID
	EPG_EVENT_QUERY_TIME		// ����
};

// �C�x���g�̎擾�̂��߂̏��
struct EpgEventQueryInfo {
	WORD NetworkID;			// �l�b�g���[�NID
	WORD TransportStreamID;	// �X�g���[��ID
	WORD ServiceID;			// �T�[�r�XID
	BYTE Type;				// �擾���@(EPG_EVENT_QUERY_*)
	BYTE Flags;				// �t���O(���݂͏��0)
	union {
		WORD EventID;		// �C�x���gID
		FILETIME Time;		// ����(UTC)
	};
};

// �f���̏��
struct EpgEventVideoInfo {
	BYTE StreamContent;	// stream_content
	BYTE ComponentType;	// component_type
						// (0x01 = 480i[4:3] / 0x03 = 480i[16:9] / 0xB1 = 1080i[4:3] / 0xB3 = 1080i[16:9])
	BYTE ComponentTag;	// component_tag
	BYTE Reserved;		// �\��
	DWORD LanguageCode;	// ����R�[�h
	LPCWSTR pszText;	// �e�L�X�g(�����ꍇ��NULL)
};

// �����̏��
struct EpgEventAudioInfo {
	BYTE Flags;				// �t���O(EPG_EVENT_AUDIO_FLAG_*)
	BYTE StreamContent;		// stream_content
	BYTE ComponentType;		// component_type (1 = Mono / 2 = Dual Mono / 3 = Stereo / 9 = 5.1ch)
	BYTE ComponentTag;		// component_tag
	BYTE SimulcastGroupTag;	// simulcast_group_tag
	BYTE QualityIndicator;	// quality_indicator
	BYTE SamplingRate;		// �T���v�����O���g���̎��
	BYTE Reserved;			// �\��
	DWORD LanguageCode;		// ����R�[�h(�剹��)
	DWORD LanguageCode2;	// ����R�[�h(������)
	LPCWSTR pszText;		// �e�L�X�g(�����ꍇ��NULL)
};

// �����̃t���O
enum {
	EPG_EVENT_AUDIO_FLAG_MULTILINGUAL	=0x01,	// �񃖍���
	EPG_EVENT_AUDIO_FLAG_MAINCOMPONENT	=0x02	// �剹��
};

// �W�������̏��
// (�Ӗ��� STD-B10 ��2�� �t�^H ���Q��)
struct EpgEventContentInfo {
	BYTE ContentNibbleLevel1;	// �啪��
	BYTE ContentNibbleLevel2;	// ������
	BYTE UserNibble1;
	BYTE UserNibble2;
};

// �C�x���g�O���[�v�̃C�x���g�̏��
struct EpgGroupEventInfo {
	WORD NetworkID;			// �l�b�g���[�NID
	WORD TransportStreamID;	// �X�g���[��ID
	WORD ServiceID;			// �T�[�r�XID
	WORD EventID;			// �C�x���gID
};

// �C�x���g�O���[�v�̏��
struct EpgEventGroupInfo {
	BYTE GroupType;					// ���
	BYTE EventListLength;			// �C�x���g�̃��X�g�̗v�f��
	BYTE Reserved[6];				// �\��
	EpgGroupEventInfo *EventList;	// �C�x���g�̃��X�g
};

// �C�x���g�̏��
struct EpgEventInfo {
	WORD EventID;						// �C�x���gID
	BYTE RunningStatus;					// running_status
	BYTE FreeCaMode;					// free_CA_mode
	DWORD Reserved;						// �\��
	SYSTEMTIME StartTime;				// �J�n����(���[�J������)
	DWORD Duration;						// ����(�b�P��)
	BYTE VideoListLength;				// �f���̏��̐�
	BYTE AudioListLength;				// �����̏��̐�
	BYTE ContentListLength;				// �W�������̏��̐�
	BYTE EventGroupListLength;			// �C�x���g�O���[�v�̏��̐�
	LPCWSTR pszEventName;				// �C�x���g��(�����ꍇ��NULL)
	LPCWSTR pszEventText;				// �e�L�X�g(�����ꍇ��NULL)
	LPCWSTR pszEventExtendedText;		// �g���e�L�X�g(�����ꍇ��NULL)
	EpgEventVideoInfo **VideoList;		// �f���̏��̃��X�g(�����ꍇ��NULL)
	EpgEventAudioInfo **AudioList;		// �����̏��̃��X�g(�����ꍇ��NULL)
	EpgEventContentInfo *ContentList;	// �W�������̏��(�����ꍇ��NULL)
	EpgEventGroupInfo **EventGroupList;	// �C�x���g�O���[�v���(�����ꍇ��NULL)
};

// �C�x���g�̃��X�g
struct EpgEventList {
	WORD NetworkID;				// �l�b�g���[�NID
	WORD TransportStreamID;		// �X�g���[��ID
	WORD ServiceID;				// �T�[�r�XID
	WORD NumEvents;				// �C�x���g�̐�
	EpgEventInfo **EventList;	// ���X�g
};

// �C�x���g�̏��̎擾
// EpgEventQueryInfo �ŁA�擾�������C�x���g���w�肵�܂��B
// �擾������񂪕s�v�ɂȂ����ꍇ�AMsgFreeEventInfo �ŉ�����܂��B
// ��񂪎擾�ł��Ȃ������ꍇ�� NULL ���Ԃ�܂��B
/*
	// ���݂̃`�����l���̌��݂̔ԑg���擾�����
	ChannelInfo ChInfo;
	ChInfo.Size = sizeof(ChInfo);
	if (MsgGetCurrentChannelInfo(pParam, &ChInfo)) {
		EpgEventQueryInfo QueryInfo;
		QueryInfo.NetworkID         = ChInfo.NetworkID;
		QueryInfo.TransportStreamID = ChInfo.TransportStreamID;
		QueryInfo.ServiceID         = ChInfo.ServiceID;
		QueryInfo.Type              = EPG_EVENT_QUERY_TIME;
		QueryInfo.Flags             = 0;
		GetSystemTimeAsFileTime(&QueryInfo.Time);
		EpgEventInfo *pEvent = MsgGetEpgEventInfo(pParam, &QueryInfo);
		if (pEvent != NULL) {
			...
			MsgFreeEpgEventInfo(pParam, pEvent);
		}
	}
*/
inline EpgEventInfo *MsgGetEpgEventInfo(PluginParam *pParam,const EpgEventQueryInfo *pInfo)
{
	return (EpgEventInfo*)(*pParam->Callback)(pParam,MESSAGE_GETEPGEVENTINFO,(LPARAM)pInfo,0);
}

// �C�x���g�̏��̉��
// MsgGetEpgEventInfo �Ŏ擾�������̃�������������܂��B
inline void MsgFreeEpgEventInfo(PluginParam *pParam,EpgEventInfo *pEventInfo)
{
	(*pParam->Callback)(pParam,MESSAGE_FREEEPGEVENTINFO,(LPARAM)pEventInfo,0);
}

// �C�x���g�̃��X�g�̎擾
// EpgEventList �� NetworkID / TransportStreamID / ServiceID ��ݒ肵�ČĂяo���܂��B
// �擾���ꂽ�ԑg�͊J�n�������Ƀ\�[�g����Ă��܂��B
// ���X�g���s�v�ɂȂ����ꍇ�AMsgFreeEpgEventList �ŉ�����܂��B
/*
	// ���݂̃`�����l���̔ԑg�\���擾�����
	ChannelInfo ChInfo;
	ChInfo.Size = sizeof(ChInfo);
	if (MsgGetCurrentChannelInfo(pParam, &ChInfo)) {
		EpgEventList EventList;
		EventList.NetworkID         = ChInfo.NetworkID;
		EventList.TransportStreamID = ChInfo.TransportStreamID;
		EventList.ServiceID         = ChInfo.ServiceID;
		if (MsgGetEpgEventList(pParam, &EventList)) {
			...
			MsgFreeEpgEventList(pParam, &EventList);
		}
	}
*/
inline bool MsgGetEpgEventList(PluginParam *pParam,EpgEventList *pList)
{
	return (*pParam->Callback)(pParam,MESSAGE_GETEPGEVENTLIST,(LPARAM)pList,0)!=0;
}

// �C�x���g�̃��X�g�̉��
// MsgGetEpgEventList �Ŏ擾�������X�g�̃�������������܂��B
inline void MsgFreeEpgEventList(PluginParam *pParam,EpgEventList *pList)
{
	(*pParam->Callback)(pParam,MESSAGE_FREEEPGEVENTLIST,(LPARAM)pList,0);
}

// BonDriver ��񋓂���
// BonDriver �̃t�H���_�Ƃ��Đݒ肳��Ă���t�H���_���� BonDriver ��񋓂��܂��B
// �߂�l�Ƃ��ăt�@�C�����̒������Ԃ�܂��BIndex ���͈͊O�̏ꍇ��0���Ԃ�܂��B
inline int MsgEnumDriver(PluginParam *pParam,int Index,LPWSTR pszFileName,int MaxLength)
{
	return (int)(*pParam->Callback)(pParam,MESSAGE_ENUMDRIVER,(LPARAM)pszFileName,MAKELPARAM(Index,min(MaxLength,0xFFFF)));
}

// �`���[�j���O��Ԃ̏��
struct DriverTuningSpaceInfo {
	DWORD Flags;				// �t���O(���݂͏��0)
	DWORD NumChannels;			// �`�����l����
	TuningSpaceInfo *pInfo;		// �`���[�j���O��Ԃ̏��
	ChannelInfo **ChannelList;	// �`�����l���̃��X�g
};

// �`���[�j���O��Ԃ̃��X�g
struct DriverTuningSpaceList {
	DWORD Flags;						// �t���O(���݂͏��0)
	DWORD NumSpaces;					// �`���[�j���O��Ԃ̐�
	DriverTuningSpaceInfo **SpaceList;	// �`���[�j���O��Ԃ̃��X�g
};

// BonDriver �̃`�����l���̃��X�g���擾����
// DriverTuningSpaceList �� Flags ��ݒ肵�ČĂяo���܂��B
// ���X�g���s�v�ɂȂ����ꍇ�AMsgFreeDriverTuningSpaceList �ŉ�����܂��B
inline bool MsgGetDriverTuningSpaceList(PluginParam *pParam,LPCWSTR pszDriverName,DriverTuningSpaceList *pList)
{
	return (*pParam->Callback)(pParam,MESSAGE_GETDRIVERTUNINGSPACELIST,(LPARAM)pszDriverName,(LPARAM)pList)!=0;
}

// BonDriver �̃`�����l���̃��X�g���������
inline void MsgFreeDriverTuningSpaceList(PluginParam *pParam,DriverTuningSpaceList *pList)
{
	(*pParam->Callback)(pParam,MESSAGE_FREEDRIVERTUNINGSPACELIST,(LPARAM)pList,0);
}

#endif	// TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,12)

#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,13)

// �ԑg�\�̔ԑg�̏��
struct ProgramGuideProgramInfo {
	WORD NetworkID;			// �l�b�g���[�NID
	WORD TransportStreamID;	// �X�g���[��ID
	WORD ServiceID;			// �T�[�r�XID
	WORD EventID;			// �C�x���gID
	SYSTEMTIME StartTime;	// �J�n����
	DWORD Duration;			// ����(�b�P��)
};

// �ԑg�\�̔ԑg�̔w�i�`��̏��
struct ProgramGuideProgramDrawBackgroundInfo {
	HDC hdc;					// �`���DC�n���h��
	RECT ItemRect;				// ���ڑS�̂̈ʒu
	RECT TitleRect;				// �^�C�g���̈ʒu
	RECT ContentRect;			// �ԑg���e�̈ʒu
	COLORREF BackgroundColor;	// �w�i�F
};

// �ԑg�\�̃��j���[�̏��
struct ProgramGuideInitializeMenuInfo {
	HMENU hmenu;		// ���j���[�̃n���h��
	UINT Command;		// ���ڂ�ID
	UINT Reserved;		// �\��
};

// �ԑg�\�̔ԑg�̃��j���[�̏��
struct ProgramGuideProgramInitializeMenuInfo {
	HMENU hmenu;		// ���j���[�̃n���h��
	UINT Command;		// ���ڂ�ID
	UINT Reserved;		// �\��
	POINT CursorPos;	// �J�[�\���ʒu
	RECT ItemRect;		// �ԑg�̈ʒu
};

// �ԑg�\�̃C�x���g�̃t���O
enum {
	PROGRAMGUIDE_EVENT_GENERAL	=0x0001,	// �S�̂̃C�x���g(EVENT_PROGRAMGUIDE_*)
	PROGRAMGUIDE_EVENT_PROGRAM	=0x0002		// �e�ԑg�̃C�x���g(EVENT_PROGRAMGUIDE_PROGRAM_*)
};

// �ԑg�\�̃C�x���g�̗L��/������ݒ肷��
// �ԑg�\�̃C�x���g(EVENT_PROGRAMGUIDE_*)�̒ʒm���K�v�ȏꍇ�A�ʒm��L���ɐݒ肵�܂��B
// EventFlags �Ŏw�肳�ꂽ�C�x���g(PROGRAMGUIDE_EVENT_*)�̒ʒm���L���ɂȂ�܂��B
inline bool MsgEnableProgramGuideEvent(PluginParam *pParam,UINT EventFlags)
{
	return (*pParam->Callback)(pParam,MESSAGE_ENABLEPROGRAMGUIDEEVENT,EventFlags,0)!=0;
}

// �ԑg�\�̃R�}���h�̏��
struct ProgramGuideCommandInfo {
	WORD Type;			// ���(PROGRAMGUiDE_COMMAND_TYPE_*)
	WORD Flags;			// �e��t���O(���݂͏��0)
	UINT ID;			// ���ʎq
	LPCWSTR pszText;	// �R�}���h�̕�����
	LPCWSTR pszName;	// �R�}���h�̖��O
};

// �ԑg�\�̃R�}���h�̎��
enum {
	PROGRAMGUIDE_COMMAND_TYPE_PROGRAM	=1	// �e�ԑg
};

// �ԑg�\�̃R�}���h���s���̏��
struct ProgramGuideCommandParam {
	UINT ID;							// ���ʎq
	UINT Action;						// ����̎��(PROGRAMGUIDE_COMMAND_ACTION_*)
	ProgramGuideProgramInfo Program;	// �ԑg�̏��
	POINT CursorPos;					// �J�[�\���ʒu
	RECT ItemRect;						// ���ڂ̈ʒu
};

// �ԑg�\�̃R�}���h���s�̑���̎��
enum {
	PROGRAMGUIDE_COMMAND_ACTION_MOUSE,	// �}�E�X�Ȃ�
	PROGRAMGUIDE_COMMAND_ACTION_KEY		// �L�[�{�[�h
};

// �ԑg�\�̃R�}���h��o�^����
// �R�}���h��o�^����ƁA�ԑg�\�̃_�u���N���b�N�Ȃǂɋ@�\�����蓖�Ă邱�Ƃ��ł���悤�ɂȂ�܂��B
// �R�}���h�����s������ EVENT_PROGRAMGUIDE_COMMAND �C�x���g�������܂��B
inline bool MsgRegisterProgramGuideCommand(PluginParam *pParam,
										   const ProgramGuideCommandInfo *pCommandList,int NumCommands=1)
{
	return (*pParam->Callback)(pParam,MESSAGE_REGISTERPROGRAMGUIDECOMMAND,(LPARAM)pCommandList,NumCommands)!=0;
}

#endif	// TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,13)


/*
	TVTest �A�v���P�[�V�����N���X

	TVTest �̊e��@�\���Ăяo�����߂̃N���X�ł��B
	�����̃��b�p�[�Ȃ̂Ŏg��Ȃ��Ă������ł��B
	TVTInitialize �֐����Ă΂ꂽ���ɁA������ PluginParam �\���̂ւ̃|�C���^��
	�R���X�g���N�^�ɓn���ăC���X�^���X�𐶐����܂��B
*/
class CTVTestApp
{
protected:
	PluginParam *m_pParam;
public:
	CTVTestApp(PluginParam *pParam) : m_pParam(pParam) {}
	virtual ~CTVTestApp() {}
	HWND GetAppWindow() {
		return m_pParam->hwndApp;
	}
	DWORD GetVersion() {
		return MsgGetVersion(m_pParam);
	}
	bool QueryMessage(UINT Message) {
		return MsgQueryMessage(m_pParam,Message);
	}
	void *MemoryReAlloc(void *pData,DWORD Size) {
		return MsgMemoryReAlloc(m_pParam,pData,Size);
	}
	void *MemoryAlloc(DWORD Size) {
		return MsgMemoryAlloc(m_pParam,Size);
	}
	void MemoryFree(void *pData) {
		MsgMemoryFree(m_pParam,pData);
	}
	bool SetEventCallback(EventCallbackFunc Callback,void *pClientData=NULL) {
		return MsgSetEventCallback(m_pParam,Callback,pClientData);
	}
	bool GetCurrentChannelInfo(ChannelInfo *pInfo) {
		pInfo->Size=sizeof(ChannelInfo);
		return MsgGetCurrentChannelInfo(m_pParam,pInfo);
	}
#if TVTEST_PLUGIN_VERSION<TVTEST_PLUGIN_VERSION_(0,0,8)
	bool SetChannel(int Space,int Channel) {
		return MsgSetChannel(m_pParam,Space,Channel);
	}
#else
	bool SetChannel(int Space,int Channel,WORD ServiceID=0) {
		return MsgSetChannel(m_pParam,Space,Channel,ServiceID);
	}
#endif
	int GetService(int *pNumServices=NULL) {
		return MsgGetService(m_pParam,pNumServices);
	}
	bool SetService(int Service,bool fByID=false) {
		return MsgSetService(m_pParam,Service,fByID);
	}
	int GetTuningSpaceName(int Index,LPWSTR pszName,int MaxLength) {
		return MsgGetTuningSpaceName(m_pParam,Index,pszName,MaxLength);
	}
	bool GetChannelInfo(int Space,int Index,ChannelInfo *pInfo) {
		pInfo->Size=sizeof(ChannelInfo);
		return MsgGetChannelInfo(m_pParam,Space,Index,pInfo);
	}
	bool GetServiceInfo(int Index,ServiceInfo *pInfo) {
		pInfo->Size=sizeof(ServiceInfo);
		return MsgGetServiceInfo(m_pParam,Index,pInfo);
	}
	int GetDriverName(LPWSTR pszName,int MaxLength) {
		return MsgGetDriverName(m_pParam,pszName,MaxLength);
	}
	bool SetDriverName(LPCWSTR pszName) {
		return MsgSetDriverName(m_pParam,pszName);
	}
	bool StartRecord(RecordInfo *pInfo=NULL) {
		if (pInfo!=NULL)
			pInfo->Size=sizeof(RecordInfo);
		return MsgStartRecord(m_pParam,pInfo);
	}
	bool StopRecord() {
		return MsgStopRecord(m_pParam);
	}
	bool PauseRecord(bool fPause=true) {
		return MsgPauseRecord(m_pParam,fPause);
	}
	bool GetRecord(RecordInfo *pInfo) {
		pInfo->Size=sizeof(RecordInfo);
		return MsgGetRecord(m_pParam,pInfo);
	}
	bool ModifyRecord(RecordInfo *pInfo) {
		pInfo->Size=sizeof(RecordInfo);
		return MsgModifyRecord(m_pParam,pInfo);
	}
	int GetZoom() {
		return MsgGetZoom(m_pParam);
	}
	int SetZoom(int Num,int Denom=100) {
		return MsgSetZoom(m_pParam,Num,Denom);
	}
	bool GetPanScan(PanScanInfo *pInfo) {
		pInfo->Size=sizeof(PanScanInfo);
		return MsgGetPanScan(m_pParam,pInfo);
	}
	bool SetPanScan(PanScanInfo *pInfo) {
		pInfo->Size=sizeof(PanScanInfo);
		return MsgSetPanScan(m_pParam,pInfo);
	}
	bool GetStatus(StatusInfo *pInfo) {
		pInfo->Size=sizeof(StatusInfo);
		return MsgGetStatus(m_pParam,pInfo);
	}
	bool GetRecordStatus(RecordStatusInfo *pInfo) {
#if TVTEST_PLUGIN_VERSION<TVTEST_PLUGIN_VERSION_(0,0,10)
		pInfo->Size=sizeof(RecordStatusInfo);
#else
		if (pInfo->pszFileName!=NULL)
			pInfo->Size=sizeof(RecordStatusInfo);
		else
			pInfo->Size=RECORDSTATUSINFO_SIZE_V1;
#endif
		return MsgGetRecordStatus(m_pParam,pInfo);
	}
	bool GetVideoInfo(VideoInfo *pInfo) {
		pInfo->Size=sizeof(VideoInfo);
		return MsgGetVideoInfo(m_pParam,pInfo);
	}
	int GetVolume() {
		return MsgGetVolume(m_pParam);
	}
	bool SetVolume(int Volume) {
		return MsgSetVolume(m_pParam,Volume);
	}
	bool GetMute() {
		return MsgGetMute(m_pParam);
	}
	bool SetMute(bool fMute) {
		return MsgSetMute(m_pParam,fMute);
	}
	int GetStereoMode() {
		return MsgGetStereoMode(m_pParam);
	}
	bool SetStereoMode(int StereoMode) {
		return MsgSetStereoMode(m_pParam,StereoMode);
	}
	bool GetFullscreen() {
		return MsgGetFullscreen(m_pParam);
	}
	bool SetFullscreen(bool fFullscreen) {
		return MsgSetFullscreen(m_pParam,fFullscreen);
	}
	bool GetPreview() {
		return MsgGetPreview(m_pParam);
	}
	bool SetPreview(bool fPreview) {
		return MsgSetPreview(m_pParam,fPreview);
	}
	bool GetStandby() {
		return MsgGetStandby(m_pParam);
	}
	bool SetStandby(bool fStandby) {
		return MsgSetStandby(m_pParam,fStandby);
	}
	bool GetAlwaysOnTop() {
		return MsgGetAlwaysOnTop(m_pParam);
	}
	bool SetAlwaysOnTop(bool fAlwaysOnTop) {
		return MsgSetAlwaysOnTop(m_pParam,fAlwaysOnTop);
	}
	void *CaptureImage(DWORD Flags=0) {
		return MsgCaptureImage(m_pParam,Flags);
	}
	bool SaveImage() {
		return MsgSaveImage(m_pParam);
	}
#if TVTEST_PLUGIN_VERSION<TVTEST_PLUGIN_VERSION_(0,0,9)
	bool Reset() {
		return MsgReset(m_pParam);
	}
#else
	bool Reset(DWORD Flags=RESET_ALL) {
		return MsgReset(m_pParam,Flags);
	}
#endif
	bool Close(DWORD Flags=0) {
		return MsgClose(m_pParam,Flags);
	}
	bool SetStreamCallback(DWORD Flags,StreamCallbackFunc Callback,void *pClientData=NULL) {
		return MsgSetStreamCallback(m_pParam,Flags,Callback,pClientData);
	}
	bool EnablePlugin(bool fEnable) {
		return MsgEnablePlugin(m_pParam,fEnable);
	}
	COLORREF GetColor(LPCWSTR pszColor) {
		return MsgGetColor(m_pParam,pszColor);
	}
	bool DecodeARIBString(const void *pSrcData,DWORD SrcLength,
						  LPWSTR pszDest,DWORD DestLength) {
		return MsgDecodeARIBString(m_pParam,pSrcData,SrcLength,pszDest,DestLength);
	}
	bool GetCurrentProgramInfo(ProgramInfo *pInfo,bool fNext=false) {
		pInfo->Size=sizeof(ProgramInfo);
		return MsgGetCurrentProgramInfo(m_pParam,pInfo,fNext);
	}
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,1)
	bool QueryEvent(UINT Event) {
		return MsgQueryEvent(m_pParam,Event);
	}
	int GetTuningSpace(int *pNumSpaces=NULL) {
		return MsgGetTuningSpace(m_pParam,pNumSpaces);
	}
	bool GetTuningSpaceInfo(int Index,TuningSpaceInfo *pInfo) {
		pInfo->Size=sizeof(TuningSpaceInfo);
		return MsgGetTuningSpaceInfo(m_pParam,Index,pInfo);
	}
	bool SetNextChannel(bool fNext=true) {
		return MsgSetNextChannel(m_pParam,fNext);
	}
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,2)
	int GetAudioStream() {
		return MsgGetAudioStream(m_pParam);
	}
	bool SetAudioStream(int Index) {
		return MsgSetAudioStream(m_pParam,Index);
	}
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,3)
	bool IsPluginEnabled() {
		return MsgIsPluginEnabled(m_pParam);
	}
	bool RegisterCommand(int ID,LPCWSTR pszText,LPCWSTR pszName) {
		return MsgRegisterCommand(m_pParam,ID,pszText,pszName);
	}
	bool RegisterCommand(const CommandInfo *pCommandList,int NumCommands) {
		return MsgRegisterCommand(m_pParam,pCommandList,NumCommands);
	}
	bool AddLog(LPCWSTR pszText) {
		return MsgAddLog(m_pParam,pszText);
	}
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,5)
	bool ResetStatus() {
		return MsgResetStatus(m_pParam);
	}
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,6)
	bool SetAudioCallback(AudioCallbackFunc pCallback,void *pClientData=NULL) {
		return MsgSetAudioCallback(m_pParam,pCallback,pClientData);
	}
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,7)
	bool DoCommand(LPCWSTR pszCommand) {
		return MsgDoCommand(m_pParam,pszCommand);
	}
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,8)
	bool GetBCasInfo(BCasInfo *pInfo) {
		pInfo->Size=sizeof(BCasInfo);
		return MsgGetBCasInfo(m_pParam,pInfo);
	}
	bool SendBCasCommand(BCasCommandInfo *pInfo) {
		return MsgSendBCasCommand(m_pParam,pInfo);
	}
	bool SendBCasCommand(const BYTE *pSendData,DWORD SendSize,BYTE *pReceiveData,DWORD *pReceiveSize) {
		return MsgSendBCasCommand(m_pParam,pSendData,SendSize,pReceiveData,pReceiveSize);
	}
	bool GetHostInfo(HostInfo *pInfo) {
		pInfo->Size=sizeof(HostInfo);
		return MsgGetHostInfo(m_pParam,pInfo);
	}
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,9)
	bool GetSetting(SettingInfo *pInfo) {
		return MsgGetSetting(m_pParam,pInfo);
	}
	bool GetSetting(LPCWSTR pszName,int *pValue) {
		return MsgGetSetting(m_pParam,pszName,pValue);
	}
	bool GetSetting(LPCWSTR pszName,unsigned int *pValue) {
		return MsgGetSetting(m_pParam,pszName,pValue);
	}
	DWORD GetSetting(LPCWSTR pszName,LPWSTR pszString,DWORD MaxLength) {
		return MsgGetSetting(m_pParam,pszName,pszString,MaxLength);
	}
	int GetDriverFullPathName(LPWSTR pszPath,int MaxLength) {
		return MsgGetDriverFullPathName(m_pParam,pszPath,MaxLength);
	}
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,10)
	HBITMAP GetLogo(WORD NetworkID,WORD ServiceID,BYTE LogoType) {
		return MsgGetLogo(m_pParam,NetworkID,ServiceID,LogoType);
	}
	UINT GetAvailableLogoType(WORD NetworkID,WORD ServiceID) {
		return MsgGetAvailableLogoType(m_pParam,NetworkID,ServiceID);
	}
	bool RelayRecord(LPCWSTR pszFileName) {
		return MsgRelayRecord(m_pParam,pszFileName);
	}
	bool GetSilentMode() {
		return MsgGetSilentMode(m_pParam);
	}
	bool SetSilentMode(bool fSilent) {
		return MsgSetSilentMode(m_pParam,fSilent);
	}
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,11)
	bool SetWindowMessageCallback(WindowMessageCallbackFunc Callback,void *pClientData=NULL) {
		return MsgSetWindowMessageCallback(m_pParam,Callback,pClientData);
	}
	bool RegisterController(ControllerInfo *pInfo) {
		pInfo->Size=sizeof(ControllerInfo);
		return MsgRegisterController(m_pParam,pInfo);
	}
	bool OnControllerButtonDown(LPCWSTR pszName,int Button) {
		return MsgOnControllerButtonDown(m_pParam,pszName,Button);
	}
	bool GetControllerSettings(LPCWSTR pszName,ControllerSettings *pSettings) {
		return MsgGetControllerSettings(m_pParam,pszName,pSettings);
	}
	bool IsControllerActiveOnly(LPCWSTR pszName) {
		return MsgIsControllerActiveOnly(m_pParam,pszName);
	}
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,12)
	EpgEventInfo *GetEpgEventInfo(const EpgEventQueryInfo *pInfo) {
		return MsgGetEpgEventInfo(m_pParam,pInfo);
	}
	void FreeEpgEventInfo(EpgEventInfo *pEventInfo) {
		MsgFreeEpgEventInfo(m_pParam,pEventInfo);
	}
	bool GetEpgEventList(EpgEventList *pList) {
		return MsgGetEpgEventList(m_pParam,pList);
	}
	void FreeEpgEventList(EpgEventList *pList) {
		MsgFreeEpgEventList(m_pParam,pList);
	}
	int EnumDriver(int Index,LPWSTR pszFileName,int MaxLength) {
		return MsgEnumDriver(m_pParam,Index,pszFileName,MaxLength);
	}
	bool GetDriverTuningSpaceList(LPCWSTR pszDriverName,DriverTuningSpaceList *pList) {
		return MsgGetDriverTuningSpaceList(m_pParam,pszDriverName,pList);
	}
	void FreeDriverTuningSpaceList(DriverTuningSpaceList *pList) {
		MsgFreeDriverTuningSpaceList(m_pParam,pList);
	}
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,13)
	bool EnableProgramGuideEvent(UINT EventFlags) {
		return MsgEnableProgramGuideEvent(m_pParam,EventFlags);
	}
	bool RegisterProgramGuideCommand(const ProgramGuideCommandInfo *pCommandList,int NumCommands=1) {
		return MsgRegisterProgramGuideCommand(m_pParam,pCommandList,NumCommands);
	}
#endif
};

/*
	TVTest �v���O�C���N���X

	�v���O�C�����N���X�Ƃ��ċL�q���邽�߂̒��ۃN���X�ł��B
	���̃N���X���e�v���O�C���Ŕh�������āA�v���O�C���̓��e���L�q���܂��B
	���̃N���X���g�킸�ɒ��ڃG�N�X�|�[�g�֐��������Ă������ł��B
*/
class CTVTestPlugin
{
protected:
	PluginParam *m_pPluginParam;
	CTVTestApp *m_pApp;
public:
	CTVTestPlugin() : m_pPluginParam(NULL),m_pApp(NULL) {}
	void SetPluginParam(PluginParam *pParam) {
		m_pPluginParam=pParam;
		m_pApp=new CTVTestApp(pParam);
	}
	virtual ~CTVTestPlugin() { delete m_pApp; }
	virtual DWORD GetVersion() { return TVTEST_PLUGIN_VERSION; }
	virtual bool GetPluginInfo(PluginInfo *pInfo)=0;
	virtual bool Initialize() { return true; }
	virtual bool Finalize() { return true; }
};

/*
	�C�x���g�n���h���N���X

	�C�x���g�����p�N���X�ł��B
	���̃N���X��h�������ăC�x���g�������s�����Ƃ��ł��܂��B
	�C�x���g�R�[���o�b�N�֐��Ƃ��ēo�^�����֐����� HandleEvent ���Ăт܂��B
	�������g��Ȃ��Ă������ł��B

	�ȉ��͎�����ł��B

	class CMyEventHandler : public TVTest::CTVTestEventHandler
	{
	public:
		virtual bool OnPluginEnable(bool fEnable) {
			if (fEnable) {
				if (MessageBox(NULL, TEXT("�L���ɂ����"), TEXT("�C�x���g"),
							   MB_OKCANCEL) != IDOK) {
					return false;
				}
			}
			return true;
		}
	};

	CMyEventHandler Handler;

	// ���̊֐����C�x���g�R�[���o�b�N�֐��Ƃ��ēo�^����Ă�����̂Ƃ��܂�
	LRESULT CALLBACK EventCallback(UINT Event,LPARAM lParam1,LPARAM lParam2,void *pClientData)
	{
		return Handler.HandleEvent(Event,lParam1,lParam2,pClientData);
	}
*/
class CTVTestEventHandler
{
protected:
	void *m_pClientData;

	// �C�x���g�n���h���́A���ɋL�q�̖������͉̂������������� true ��Ԃ��܂�

	// �L����Ԃ��ω�����
	// �ω������ۂ���ꍇ false ��Ԃ��܂�
	virtual bool OnPluginEnable(bool fEnable) { return false; }
	// �ݒ���s��
	// �v���O�C���̃t���O�� PLUGIN_FLAG_HASSETTINGS ���ݒ肳��Ă���ꍇ�ɌĂ΂�܂�
	// �ݒ肪 OK ���ꂽ�� true ��Ԃ��܂�
	virtual bool OnPluginSettings(HWND hwndOwner) { return false; }
	// �`�����l�����ύX���ꂽ
	virtual bool OnChannelChange() { return false; }
	// �T�[�r�X���ύX���ꂽ
	virtual bool OnServiceChange() { return false; }
	// �h���C�o���ύX���ꂽ
	virtual bool OnDriverChange() { return false; }
	// �T�[�r�X�̍\�����ω�����
	virtual bool OnServiceUpdate() { return false; }
	// �^���Ԃ��ω�����
	virtual bool OnRecordStatusChange(int Status) { return false; }
	// �S��ʕ\����Ԃ��ω�����
	virtual bool OnFullscreenChange(bool fFullscreen) { return false; }
	// �v���r���[�\����Ԃ��ω�����
	virtual bool OnPreviewChange(bool fPreview) { return false; }
	// ���ʂ��ω�����
	virtual bool OnVolumeChange(int Volume,bool fMute) { return false; }
	// �X�e���I���[�h���ω�����
	virtual bool OnStereoModeChange(int StereoMode) { return false; }
	// �F�̐ݒ肪�ω�����
	virtual bool OnColorChange() { return false; }
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,3)
	// �ҋ@��Ԃ��ω�����
	virtual bool OnStandby(bool fStandby) { return false; }
	// �R�}���h���I�����ꂽ
	virtual bool OnCommand(int ID) { return false; }
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,4)
	// �����N���֎~���ɕ����N�����ꂽ
	virtual bool OnExecute(LPCWSTR pszCommandLine) { return false; }
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,5)
	// ���Z�b�g���ꂽ
	virtual bool OnReset() { return false; }
	// �X�e�[�^�X(MESSAGE_GETSTUATUS�Ŏ擾�ł�����e)�����Z�b�g���ꂽ
	virtual bool OnStatusReset() { return false; }
	// �����X�g���[�����ύX���ꂽ
	virtual bool OnAudioStreamChange(int Stream) { return false; }
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,9)
	// �ݒ肪�ύX���ꂽ
	virtual bool OnSettingsChange() { return false; }
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,10)
	// TVTest�̃E�B���h�E��������
	virtual bool OnClose() { return false; }
	// �^�悪�J�n�����
	virtual bool OnStartRecord(StartRecordInfo *pInfo) { return false; }
	// �^��t�@�C���̐؂�ւ����s��ꂽ
	virtual bool OnRelayRecord(LPCWSTR pszFileName) { return false; }
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,11)
	// �R���g���[���̑Ώۂ̐ݒ�
	virtual bool OnControllerFocus(HWND hwnd) { return false; }
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,13)
	// �N���������I������
	virtual void OnStartupDone() {}
	// �ԑg�\�̏�����
	virtual bool OnProgramGuideInitialize(HWND hwnd) { return true; }
	// �ԑg�\�̏I��
	virtual bool OnProgramGuideFinalize(HWND hwnd) { return true; }
	// �ԑg�\�̃R�}���h�̎��s
	virtual bool OnProgramGuideCommand(
		UINT Command,const ProgramGuideCommandParam *pParam) { return false; }
	// �ԑg�\�̃��j���[�̏�����
	virtual int OnProgramGuideInitializeMenu(
		const ProgramGuideInitializeMenuInfo *pInfo) { return 0; }
	// �ԑg�\�̃��j���[���I�����ꂽ
	virtual bool OnProgramGuideMenuSelected(UINT Command) { return false; }
	// �ԑg�\�̔ԑg�̔w�i�`��
	virtual bool OnProgramGuideProgramDrawBackground(
		const ProgramGuideProgramInfo *pProgramInfo,
		const ProgramGuideProgramDrawBackgroundInfo *pInfo) { return false; }
	// �ԑg�\�̔ԑg�̃��j���[������
	virtual int OnProgramGuideProgramInitializeMenu(
		const ProgramGuideProgramInfo *pProgramInfo,
		const ProgramGuideProgramInitializeMenuInfo *pInfo) { return 0; }
	// �ԑg�\�̔ԑg�̃��j���[���I�����ꂽ
	virtual bool OnProgramGuideProgramMenuSelected(
		const ProgramGuideProgramInfo *pProgramInfo,UINT Command) { return false; }
#endif

public:
	virtual ~CTVTestEventHandler() {}
	LRESULT HandleEvent(UINT Event,LPARAM lParam1,LPARAM lParam2,void *pClientData)
	{
		m_pClientData=pClientData;
		switch (Event) {
		case EVENT_PLUGINENABLE:		return OnPluginEnable(lParam1!=0);
		case EVENT_PLUGINSETTINGS:		return OnPluginSettings((HWND)lParam1);
		case EVENT_CHANNELCHANGE:		return OnChannelChange();
		case EVENT_SERVICECHANGE:		return OnServiceChange();
		case EVENT_DRIVERCHANGE:		return OnDriverChange();
		case EVENT_SERVICEUPDATE:		return OnServiceUpdate();
		case EVENT_RECORDSTATUSCHANGE:	return OnRecordStatusChange((int)lParam1);
		case EVENT_FULLSCREENCHANGE:	return OnFullscreenChange(lParam1!=0);
		case EVENT_PREVIEWCHANGE:		return OnPreviewChange(lParam1!=0);
		case EVENT_VOLUMECHANGE:		return OnVolumeChange((int)lParam1,lParam2!=0);
		case EVENT_STEREOMODECHANGE:	return OnStereoModeChange((int)lParam1);
		case EVENT_COLORCHANGE:			return OnColorChange();
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,3)
		case EVENT_STANDBY:				return OnStandby(lParam1!=0);
		case EVENT_COMMAND:				return OnCommand((int)lParam1);
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,4)
		case EVENT_EXECUTE:				return OnExecute((LPCWSTR)lParam1);
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,5)
		case EVENT_RESET:				return OnReset();
		case EVENT_STATUSRESET:			return OnStatusReset();
		case EVENT_AUDIOSTREAMCHANGE:	return OnAudioStreamChange((int)lParam1);
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,9)
		case EVENT_SETTINGSCHANGE:		return OnSettingsChange();
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,10)
		case EVENT_CLOSE:				return OnClose();
		case EVENT_STARTRECORD:			return OnStartRecord((StartRecordInfo*)lParam1);
		case EVENT_RELAYRECORD:			return OnRelayRecord((LPCWSTR)lParam1);
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,11)
		case EVENT_CONTROLLERFOCUS:		return OnControllerFocus((HWND)lParam1);
#endif
#if TVTEST_PLUGIN_VERSION>=TVTEST_PLUGIN_VERSION_(0,0,13)
		case EVENT_STARTUPDONE:			OnStartupDone(); return 0;
		case EVENT_PROGRAMGUIDE_INITIALIZE:	return OnProgramGuideInitialize((HWND)lParam1);
		case EVENT_PROGRAMGUIDE_FINALIZE:	return OnProgramGuideFinalize((HWND)lParam1);
		case EVENT_PROGRAMGUIDE_COMMAND:
			return OnProgramGuideCommand(
				(UINT)lParam1,
				(const ProgramGuideCommandParam*)lParam2);
		case EVENT_PROGRAMGUIDE_INITIALIZEMENU:
			return OnProgramGuideInitializeMenu(
				(const ProgramGuideInitializeMenuInfo*)lParam1);
		case EVENT_PROGRAMGUIDE_MENUSELECTED:
			return OnProgramGuideMenuSelected((UINT)lParam1);
		case EVENT_PROGRAMGUIDE_PROGRAM_DRAWBACKGROUND:
			return OnProgramGuideProgramDrawBackground(
				(const ProgramGuideProgramInfo*)lParam1,
				(const ProgramGuideProgramDrawBackgroundInfo*)lParam2);
		case EVENT_PROGRAMGUIDE_PROGRAM_INITIALIZEMENU:
			return OnProgramGuideProgramInitializeMenu(
				(const ProgramGuideProgramInfo*)lParam1,
				(const ProgramGuideProgramInitializeMenuInfo*)lParam2);
		case EVENT_PROGRAMGUIDE_PROGRAM_MENUSELECTED:
			return OnProgramGuideProgramMenuSelected(
				(const ProgramGuideProgramInfo*)lParam1,(UINT)lParam2);
#endif
		}
		return 0;
	}
};


}	// namespace TVTest


#include <poppack.h>


#ifdef TVTEST_PLUGIN_CLASS_IMPLEMENT
/*
	�v���O�C�����N���X�Ƃ��ċL�q�ł���悤�ɂ��邽�߂́A�G�N�X�|�[�g�֐��̎���
	�ł��B
	������g���΁A�G�N�X�|�[�g�֐��������Ŏ�������K�v���Ȃ��Ȃ�܂��B

	�v���O�C�����ł� CreatePluginClass �֐����������āACTVTestPlugin �N���X����
	�h���������v���O�C���N���X�̃C���X�^���X�� new �Ő������ĕԂ��܂��B�Ⴆ�΁A
	�ȉ��̂悤�ɏ����܂��B

	#include <windows.h>
	#define TVTEST_PLUGIN_CLASS_IMPLEMENT
	#include "TVTestPlugin.h"

	// �v���O�C���N���X�̐錾
	class CMyPluginClass : public TVTest::CTVTestPlugin {
		...
	};

	// �N���X�̃C���X�^���X�𐶐�����
	TVTest::CTVTestPlugin *CreatePluginClass()
	{
		return new CMyPluginClass;
	}
*/


TVTest::CTVTestPlugin *CreatePluginClass();

HINSTANCE g_hinstDLL;				// DLL �̃C���X�^���X�n���h��
TVTest::CTVTestPlugin *g_pPlugin;	// �v���O�C���N���X�ւ̃|�C���^


// �G���g���|�C���g
// �v���O�C���N���X�̃C���X�^���X�̐����Ɣj�����s���Ă��܂�
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		g_hinstDLL=hinstDLL;
		g_pPlugin=CreatePluginClass();
		if (g_pPlugin==NULL)
			return FALSE;
		break;
	case DLL_PROCESS_DETACH:
		if (g_pPlugin) {
			delete g_pPlugin;
			g_pPlugin=NULL;
		}
		break;
	}
	return TRUE;
}

// �v���O�C���̏�������v���O�C���d�l�̃o�[�W������Ԃ�
// �v���O�C�������[�h�����ƍŏ��ɂ��̊֐����Ă΂�A
// �Ή����Ă��Ȃ��o�[�W�������Ԃ��ꂽ�ꍇ�͂����ɃA�����[�h����܂��B
TVTEST_EXPORT(DWORD) TVTGetVersion()
{
	return g_pPlugin->GetVersion();
}

// �v���O�C���̏����擾����
// TVTGetVersion �̎��ɌĂ΂��̂ŁA�v���O�C���̏��� PluginInfo �\���̂ɐݒ肵�܂��B
// FALSE ���Ԃ��ꂽ�ꍇ�A�����ɃA�����[�h����܂��B
TVTEST_EXPORT(BOOL) TVTGetPluginInfo(TVTest::PluginInfo *pInfo)
{
	return g_pPlugin->GetPluginInfo(pInfo);
}

// ���������s��
// TVTGetPluginInfo �̎��ɌĂ΂��̂ŁA�������������s���܂��B
// FALSE ���Ԃ��ꂽ�ꍇ�A�����ɃA�����[�h����܂��B
TVTEST_EXPORT(BOOL) TVTInitialize(TVTest::PluginParam *pParam)
{
	g_pPlugin->SetPluginParam(pParam);
	return g_pPlugin->Initialize();
}

// �I���������s��
// �v���O�C�����A�����[�h�����O�ɌĂ΂��̂ŁA�I���������s���܂��B
// ���̊֐����Ă΂��̂� TVTInitialize �֐��� TRUE ��Ԃ����ꍇ�����ł��B
TVTEST_EXPORT(BOOL) TVTFinalize()
{
	bool fOK=g_pPlugin->Finalize();
	delete g_pPlugin;
	g_pPlugin=NULL;
	return fOK;
}


#endif	// TVTEST_PLUGIN_CLASS_IMPLEMENT


#endif	// TVTEST_PLUGIN_H
