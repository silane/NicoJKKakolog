#pragma once

namespace NicoJKKakolog
{
	class NiconicoLoginSession
	{
	private:
		std::string userSession;

	public:
		NiconicoLoginSession();
		~NiconicoLoginSession();

		bool Login(const std::string &mail, const std::string &password);
		void Logout();

		bool IsLoggedIn() const;
		std::string GetUserSessionCookie() const;
	};
}