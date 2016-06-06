#pragma once

namespace NicoJKKakolog
{
	class NiconicoLoginSession
	{
	private:
		std::string mail;
		std::string password;
		std::string userSession;

		void DoLogout();

	public:
		NiconicoLoginSession();
		~NiconicoLoginSession();

		bool Login(const std::string &mail, const std::string &password);
		void Logout();
		bool Relogin();

		bool IsLoggedIn() const;
		std::string GetUserSessionCookie() const;
	};
}