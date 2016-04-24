#pragma once
#include <memory>
#include "IChatProvider.h"

namespace NicoJKKakolog
{
	//NicoJKKakolog�ɓo�^��ChatProvider��Ԃ�
	class IChatProviderEntry
	{
	public:
		//���[�U�[�ɕ\������G���g���̖��O
		virtual std::string GetName() const=0;
		//���[�U�[�ɕ\������G���g���̐���
		virtual std::string GetDescription() const = 0;
		//���[�U�[���G���g����L���ɂ����Ƃ��ɌĂ΂��BChatProvider��Ԃ��悤�ɂ���B���[�U�[�ւ̃I�v�V�����񎦂͂����ł���̂��ǂ��B
		virtual IChatProvider *NewProvider() = 0;
		//���݌Ă΂�邱�Ƃ͂Ȃ��ł�
		virtual void DeleteProvider(std::unique_ptr<IChatProvider> provider) = 0;
		virtual ~IChatProviderEntry() = default;
	};

}