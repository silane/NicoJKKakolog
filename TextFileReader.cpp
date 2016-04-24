#include "stdafx.h"
#include "TextFileReader.h"

CTextFileReader::CTextFileReader()
	: hFile_(INVALID_HANDLE_VALUE)
	, bEof_(false)
{
	buf_[0] = '\0';
}

CTextFileReader::~CTextFileReader()
{
	Close();
}

bool CTextFileReader::Open(LPCTSTR path, DWORD shareMode, DWORD flagsAndAttributes)
{
	Close();
	hFile_ = CreateFile(path, GENERIC_READ, shareMode, NULL, OPEN_EXISTING, flagsAndAttributes, NULL);
	return IsOpen();
}

void CTextFileReader::Close()
{
	if (IsOpen()) {
		CloseHandle(hFile_);
		hFile_ = INVALID_HANDLE_VALUE;
	}
	bEof_ = false;
	buf_[0] = '\0';
}

// �t�@�C���|�C���^��擪�ɖ߂�
bool CTextFileReader::ResetPointer()
{
	if (IsOpen()) {
		if (SetFilePointer(hFile_, 0, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER) {
			bEof_ = false;
			buf_[0] = '\0';
			return true;
		}
	}
	return false;
}

// 1�s�܂���NUL���܂ލő�textMax(>0)�o�C�g�ǂݍ���
// ���s�����͎�菜��
// �߂�l��NUL���܂ޓǂݍ��܂ꂽ�o�C�g���A�I�[�ɒB�����0��Ԃ�
int CTextFileReader::ReadLine(char *text, int textMax)
{
	if (!IsOpen()) {
		return 0;
	}
	int textLen = 0;
	for (;;) {
		if (!bEof_) {
			int bufLen = lstrlenA(buf_);
			DWORD read;
			if (!ReadFile(hFile_, buf_ + bufLen, BUF_SIZE - bufLen - 1, &read, NULL)) {
				buf_[bufLen] = '\0';
				bEof_ = true;
			} else {
				buf_[bufLen + read] = '\0';
				if (lstrlenA(buf_) < BUF_SIZE - 1) {
					bEof_ = true;
				}
			}
		}
		if (!textLen && !buf_[0]) {
			return 0;
		}
		int lineLen = StrCSpnA(buf_, "\n");
		int copyNum = min(lineLen + 1, textMax - textLen);
		lstrcpynA(text + textLen, buf_, copyNum);
		textLen += copyNum - 1;
		if (lineLen < BUF_SIZE - 1) {
			if (buf_[lineLen] == '\n') ++lineLen;
			memmove(buf_, buf_ + lineLen, sizeof(buf_) - lineLen);
			if (textLen >= 1 && text[textLen-1] == '\r') {
				text[--textLen] = '\0';
			}
			return textLen + 1;
		}
		buf_[0] = '\0';
	}
}

// �ŏI�s��1�s�܂���NUL���܂ލő�textMax(>0)�o�C�g(���܂�Ȃ��ꍇ�s�������J�b�g)�ǂݍ���
// ���s�����͎�菜��
// �t�@�C���|�C���^�͐擪�ɖ߂�
// �߂�l��NUL���܂ޓǂݍ��܂ꂽ�o�C�g��
int CTextFileReader::ReadLastLine(char *text, int textMax)
{
	if (!IsOpen()) {
		return 0;
	}
	// 2GB�ȏ�ɂ͑Ή����Ȃ�
	DWORD fileSize = GetFileSize(hFile_, NULL);
	if (fileSize > 0x7FFFFFFF ||
	    SetFilePointer(hFile_, -min(textMax - 1, static_cast<int>(fileSize)), NULL, FILE_END) == INVALID_SET_FILE_POINTER) {
		return 0;
	}
	DWORD read;
	if (!ReadFile(hFile_, text, textMax - 1, &read, NULL)) {
		ResetPointer();
		return 0;
	}
	text[read] = '\0';
	int textLen = lstrlenA(text);
	if (textLen >= 1 && text[textLen-1] == '\n') {
		text[--textLen] = '\0';
	}
	if (textLen >= 1 && text[textLen-1] == '\r') {
		text[--textLen] = '\0';
	}
	char *p = StrRChrA(text, text + textLen, '\n');
	if (p) {
		++p;
		memmove(text, p, textLen - static_cast<int>(p - text) + 1);
		textLen -= static_cast<int>(p - text);
	}
	ResetPointer();
	return textLen + 1;
}

// ���݈ʒu����t�@�C���T�C�Y/scale�����V�[�N����
// �߂�l�̓t�@�C���|�C���^�̈ړ��o�C�g��
int CTextFileReader::Seek(int scale)
{
	if (!IsOpen() || scale == 0) {
		return 0;
	}
	DWORD fileSize = GetFileSize(hFile_, NULL);
	DWORD filePos = SetFilePointer(hFile_, 0, NULL, FILE_CURRENT);
	if (fileSize > 0x7FFFFFFF || filePos == INVALID_SET_FILE_POINTER) {
		return 0;
	}
	LONGLONG llNextPos = static_cast<LONGLONG>(fileSize) / (scale < 0 ? -scale : scale) * (scale < 0 ? -1 : 1) + filePos;
	DWORD nextPos = llNextPos < 0 ? 0 : llNextPos >= fileSize ? filePos : static_cast<DWORD>(llNextPos);
	if (nextPos == filePos) {
		return 0;
	}
	nextPos = SetFilePointer(hFile_, nextPos, NULL, FILE_BEGIN);
	if (nextPos == INVALID_SET_FILE_POINTER) {
		return 0;
	}
	bEof_ = false;
	buf_[0] = '\0';
	return static_cast<int>(nextPos) - static_cast<int>(filePos);
}
