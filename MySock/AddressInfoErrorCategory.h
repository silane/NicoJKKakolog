#pragma once

#include <system_error>

namespace MySock
{
	class AddressInfoErrorCategory_t :public std::error_category
	{
	public:
		virtual const char *name() const noexcept override;
		virtual std::string message(int ev) const override;
	};

	const std::error_category &AddressInfoErrorCategory() noexcept;
}