// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once

#ifndef WINVER
#define WINVER 0x0600		// Windows Vista
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600	// Windows Vista
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600	// Internet Explorer 6.0
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WindowsX.h>
#include <WinSock2.h>
#include <MMSystem.h>
#include <Shlwapi.h>
#include <GdiPlus.h>
#include <vector>
#include <list>
#include <regex>
#include <tchar.h>
#include <dwmapi.h>
#include <CommDlg.h>
#include <ShellAPI.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "imm32.lib")
