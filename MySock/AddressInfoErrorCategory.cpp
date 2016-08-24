#include "AddressInfoErrorCategory.h"
#include "CommonIncludes.h"

namespace MySock
{
	const char *AddressInfoErrorCategory_t::name() const noexcept
	{
		return "get_address_info";
	}

	std::string AddressInfoErrorCategory_t::message(int ev) const
	{
#ifdef _WIN32
		return gai_strerrorA(ev);
#else
		return gai_strerror(ev);
#endif
	}


	const std::error_category &AddressInfoErrorCategory() noexcept
	{
		static const AddressInfoErrorCategory_t category;
		return category;
	}
}