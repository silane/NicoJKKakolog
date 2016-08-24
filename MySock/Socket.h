#pragma once
#include "CommonIncludes.h"
#include "EndPoint/EndPoint.h"

#include <system_error>
#include <memory>
#include <vector>
#include "MemoryBlock.h"

namespace MySock
{
	class TimeoutError:public std::system_error
	{
	public:
		using system_error::system_error;
	};

	class Socket
	{
	public:
#ifdef _WIN32
		typedef SOCKET socket_t;

		enum class ShutdownMode :int
		{
			SHUT_RD = SD_RECEIVE,
			SHUT_WR = SD_SEND,
			SHUT_RDWR = SD_BOTH,
		};
		
#else
		typedef int socket_t;

		enum class ShutdownMode :int
		{
			SHUT_RD = SHUT_RD,
			SHUT_WR = SHUT_WR,
			SHUT_RDWR = SHUT_RDWR,
		};

	private:
		static constexpr int INVALID_SOCKET = -1;
		static constexpr int SOCKET_ERROR = -1;
#endif
	protected:
		socket_t sock;
		Socket() noexcept;
		explicit Socket(socket_t sock) noexcept;
		void initialize(int family, int type, int proto);//‚¢‚Á‚½‚ñclose‚µ‚½‚ç‚Ü‚½ŒÄ‚ñ‚Å‚¢‚¢
		int GetErrorNum() const;
		void ThrowSystemError() const;
		
		static bool IsTimeoutError(int err);

	public:
		Socket(const Socket &obj) = delete;
		Socket &operator=(const Socket &obj) = delete;
		Socket(Socket &&obj);
		Socket &operator=(Socket &&obj);
		Socket(int family, int type, int proto);
		~Socket() noexcept;

		Socket Accept();
		void Bind(const EndPoint &locaplEP);
		void Close();
		void Connect(const EndPoint &remoteEP);
		void GetSocketOption(int level, int optname, void *optval, int *optlen) const;
		void SetSocketOption(int level, int optname, void *optval, int optlen);
		void Listen(int backlog);
		int Receive(void *buf, std::size_t len, int flags = 0);
		MemoryBlock Receive(std::size_t len, int flags = 0);
		int ReceiveFrom(void *buf, std::size_t len, int flags, std::unique_ptr<EndPoint> &remoteEP);
		MemoryBlock ReceiveFrom(std::size_t len, int flags, std::unique_ptr<EndPoint> &remoteEP);
		short Poll(int milliSeconds, short events);
		int Send(const void *buf, std::size_t len, int flags = 0);
		void Send(const MemoryBlock &data, int flags = 0);
		int SendTo(const void *buf, std::size_t len, int flags, const EndPoint &remoteEP);
		void SendTo(const MemoryBlock &data, int flags, const EndPoint &remoteEP);
		void Shutdown(ShutdownMode how);
		

		socket_t GetFileHandle() const;

		std::size_t GetAvailableBytesCount() const;

		void SetNonblocking(bool val);

		bool GetBroadcast() const;
		void SetBroadcast(bool val);

		bool GetReuseAddress() const;
		void SetReuseAddress(bool val);

		linger GetLingerState() const;
		void SetLingerState(const linger &val);

		int GetReceiveBufferSize() const;
		void SetReceiveBufferSize(int val);

		int GetSendBufferSize() const;
		void SetSendBufferSize(int val);

		//millisecond
		int GetReceiveTimeout() const;
		void SetReceiveTimeout(int milliSeconds);

		//millisecond
		int GetSendTimeout() const;
		void SetSendTimeout(int milliSeconds);

		std::unique_ptr<EndPoint> GetRemoteEndPoint() const;
		std::unique_ptr<EndPoint> GetLocalEndPoint() const;	
	};
}