#pragma once

#include "../Chat.h"

namespace NicoJKKakolog
{
	//�`���b�g�̏C�����[��(NG�����܂�)
	class ChatModRule
	{
	public:
		virtual ~ChatModRule()=default;

		//�߂�l�͕ύX���s�������ǂ���
		virtual bool Modify(Chat &chat)=0;

		//���[�U�[�ɕ\���������
		virtual std::string GetDescription() = 0;
	};
}