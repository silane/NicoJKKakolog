#pragma once

#include "../Chat.h"

namespace NicoJKKakolog
{
	//�`���b�g�̏C�����[��(NG�����܂�)
	class IChatModRule
	{
	public:
		virtual ~IChatModRule()=default;

		//�߂�l�͕ύX���s�������ǂ���
		virtual bool Modify(Chat &chat)=0;

		//���[�U�[�ɕ\���������
		virtual std::string GetDescription() const = 0;
	};
}