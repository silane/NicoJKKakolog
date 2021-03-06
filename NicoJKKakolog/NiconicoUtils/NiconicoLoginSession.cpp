#include "../../stdafx.h"
#include "NiconicoLoginSession.h"
#include "cpprest\http_client.h"

namespace NicoJKKakolog {
	NiconicoLoginSession::NiconicoLoginSession()
	{
	}


	NiconicoLoginSession::~NiconicoLoginSession()
	{
	}

#pragma warning(push)
#pragma warning(disable: 4458)
	bool NiconicoLoginSession::Login(const std::string &mail, const std::string &password)
	{
		web::http::client::http_client client(U("https://secure.nicovideo.jp/secure/login?site=niconico"));

		if (IsLoggedIn())
			return false;

		this->mail = mail;
		this->password = password;
		try {
			return client.request(web::http::methods::POST, "", utility::conversions::to_utf8string("mail_tel=" + mail + "&password=" + password + "&next_url="),"application/x-www-form-urlencoded").
				then([this](web::http::http_response &response) {

				const web::http::http_headers &h=response.headers();
				auto cookie = h.find(U("Set-Cookie"));
				if (cookie == std::end(h))
					return false;

				for (utility::string_t::size_type idx = cookie->second.find(U("user_session=")); idx != utility::string_t::npos; idx = cookie->second.find(U("user_session="), idx + 1))
				{
					this->userSession = utility::conversions::to_utf8string(cookie->second.substr(idx, cookie->second.find(U(";"), idx) - idx));
					if (this->userSession != "user_session=deleted")
						return true;
				}
				this->userSession.clear();
				return false;

			}).get();
		}
		catch (...) {
			return false;
		}
	}
#pragma warning(pop)
	void NiconicoLoginSession::Logout() 
	{
		if (!IsLoggedIn())
			return;
		web::http::client::http_client client(U("https://secure.nicovideo.jp/secure/logout"));
		web::http::http_request req(web::http::methods::GET);
		req.headers().add(U("Cookie"), userSession.c_str());
		client.request(req).wait();
		this->userSession.clear();
	}

	bool NiconicoLoginSession::Relogin()
	{
		if (!IsLoggedIn())
			return false;
		this->Logout();
		return this->Login(this->mail, this->password);
	}

	bool NiconicoLoginSession::IsLoggedIn() const
	{
		return userSession.size()>0;
	}

	std::string NiconicoLoginSession::GetUserSessionCookie() const
	{
		return userSession;
	}
}