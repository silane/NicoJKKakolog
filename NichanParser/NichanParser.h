#pragma once
#include "Nichan.h"
#include "../MyXml/MyXml.h"
#include <memory>


namespace Nichan
{
	//�X���b�h��html�̉�͊�
	class ThreadParser
	{
	public:
		virtual Thread Parse(const MyXml::Doc &doc)=0;
		virtual ~ThreadParser()=default;
	};

	//html5�ŗp�X���b�h�p�[�T�[
	class Type1ThreadParser:public ThreadParser
	{
	private:
		static std::chrono::system_clock::time_point GetDate(const std::string &val);
	public:
		virtual Thread Parse(const MyXml::Doc &doc) override;
	};

	//og���O��Ԃ��g���Ă�ŗp�X���b�h�p�[�T�[
	class Type2ThreadParser:public ThreadParser
	{
	private:
		static std::chrono::system_clock::time_point GetDate(const std::string &val);
	public:
		virtual Thread Parse(const MyXml::Doc &doc) override;
	};

	//��html��͊�
	class BoardParser
	{
	public:
		virtual Board Parse(const MyXml::Doc &doc) = 0;
		virtual ~BoardParser() = default;
	};

	//�B��̔p�[�T�[
	class Type1BoardParser:public BoardParser
	{
	public:
		virtual Board Parse(const MyXml::Doc &doc) override;
	};

	//�X���̉�͂łǂ̃p�[�T�[���g���ׂ������肷��
	std::unique_ptr<ThreadParser> DetectThreadParser(const MyXml::Doc &doc);

	//�̉�͂łǂ̃p�[�T�[���g���ׂ������肷��
	std::unique_ptr<BoardParser> DetectBoardParser(const MyXml::Doc &doc);

	//�X����URL����_�E�����[�h���ăp�[�X����
	//���[�J���ȃt�@�C�����ł�����
	Thread ParseThreadFromUrl(const std::string &url);

	//�X����HTML�����񂩂�p�[�X����
	Thread ParseThreadFromText(const std::string &text);

	//��URL����_�E�����[�h���ăp�[�X����
	//���[�J���ȃt�@�C�����ł�����
	Board ParseBoardFromUrl(std::string url);

	//��HTML�����񂩂�p�[�X����
	Board ParseBoardFromText(std::string text);
}
