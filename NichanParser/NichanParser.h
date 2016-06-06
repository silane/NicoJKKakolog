#pragma once
#include "Nichan.h"
#include "../MyXml/MyXml.h"
#include <memory>

namespace Nichan
{
	//�X���b�h��html�̉�͊�
	class Parser
	{
	public:
		virtual Thread Parse(const MyXml::Doc &doc)=0;
	};

	//html5�ŗp�p�[�T�[
	class Type1Parser:public Parser
	{
	private:
		static std::chrono::system_clock::time_point GetDate(const std::string &val);
	public:
		virtual Thread Parse(const MyXml::Doc &doc) override;
	};

	//og���O��Ԃ��g���Ă�ŗp�p�[�T�[
	class Type2Parser:public Parser
	{
	private:
		static std::chrono::system_clock::time_point GetDate(const std::string &val);
	public:
		virtual Thread Parse(const MyXml::Doc &doc) override;
	};

	//�ǂ̃p�[�T�[���g���ׂ������肷��
	std::unique_ptr<Parser> DetectParser(const MyXml::Doc &doc);

	//���[�J���ȃt�@�C�����ł�����
	Thread ParseFromUrl(const std::string &url);

	Thread PraseFromText(const std::string &text);
}
