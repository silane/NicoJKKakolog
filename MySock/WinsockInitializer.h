#pragma once
namespace MySock
{
	class WinsockInitializer
	{
	public:
		WinsockInitializer();
		~WinsockInitializer();

		WinsockInitializer(const WinsockInitializer &) = delete;
		WinsockInitializer(WinsockInitializer &&) = delete;
		WinsockInitializer &operator=(const WinsockInitializer &) = delete;
		WinsockInitializer &operator=(WinsockInitializer &&) = delete;
	};
}