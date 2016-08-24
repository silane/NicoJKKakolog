#include "Socket.h"
#include "EndPointUtil.h"

namespace MySock
{
	Socket::Socket() noexcept :sock(INVALID_SOCKET)
	{
	}

	Socket::Socket(socket_t sock) noexcept : sock(sock)
	{
	}

	void Socket::initialize(int family, int type, int proto)
	{
		if (sock != INVALID_SOCKET)
			throw;

		this->sock = socket(family, type, proto);
		if (this->sock == INVALID_SOCKET)
			ThrowSystemError();
	}

	int Socket::GetErrorNum() const
	{
		/*int val;
		int len = sizeof(val);
		if (getsockopt(this->sock, SOL_SOCKET, SO_ERROR, (raw_type *)&val, &len) == -1)
			throw;
		return val;*/

#ifdef _WIN32
		return WSAGetLastError();
#elif
		return std::errno;
#endif
	}

	void Socket::ThrowSystemError() const
	{
		auto code = GetErrorNum();
		if (IsTimeoutError(code))
			throw TimeoutError(code, std::system_category());
		else
			throw std::system_error(code, std::system_category());
	}

	bool Socket::IsTimeoutError(int err)
	{
#ifdef _WIN32
		return err == WSAEWOULDBLOCK || err==WSAETIMEDOUT;
#elif
		return err == EAGAIN || err == EWOULDBLOCK || err == EINPROGRESS;
#endif
	}

	Socket::Socket(Socket &&obj) :sock(obj.sock)
	{
		obj.sock = INVALID_SOCKET;
	}

	Socket &Socket::operator=(Socket &&obj)
	{
		this->sock = obj.sock;
		obj.sock = INVALID_SOCKET;

		return *this;
	}

	Socket::Socket(int family, int type, int proto) :sock(INVALID_SOCKET)
	{
		initialize(family, type, proto);
	}

	Socket::~Socket() noexcept
	{
		try {
			this->Close();
		}
		catch (...) {}
	}

	Socket Socket::Accept()
	{
		//SocketAddress addr(sizeof(sockaddr_storage));
		//int size = (int)addr.GetSize();
		socket_t remote_sock = accept(this->sock, NULL, NULL);
		if (remote_sock == INVALID_SOCKET)
			ThrowSystemError();

		return Socket(remote_sock);
	}

	void Socket::Bind(const EndPoint &localEP)
	{
		auto addr = localEP.Serialize();
		if (bind(this->sock, (sockaddr *)addr.GetData(), (int)addr.GetSize()) == SOCKET_ERROR)
			ThrowSystemError();
	}

	void Socket::Close()
	{
#ifdef _WIN32
		int ret = closesocket(this->sock);
#elif
		int ret = close(this->sock);
#endif
		if (ret == SOCKET_ERROR)
			ThrowSystemError();
		this->sock = INVALID_SOCKET;
	}

	void Socket::Connect(const EndPoint &remoteEP)
	{
		auto addr = remoteEP.Serialize();
		if (connect(this->sock, (sockaddr *)addr.GetData(), (int)addr.GetSize()) != 0)
			ThrowSystemError();
	}

	void Socket::GetSocketOption(int level, int optname, void *optval, int *optlen) const
	{
		if (getsockopt(this->sock, level, optname, (raw_type *)optval, optlen) == SOCKET_ERROR)
			ThrowSystemError();
	}

	void Socket::SetSocketOption(int level, int optname, void *optval, int optlen)
	{
		if (setsockopt(this->sock, level, optname, (raw_type *)optval, optlen) == SOCKET_ERROR)
			ThrowSystemError();
	}

	void Socket::Listen(int backlog)
	{
		if (listen(this->sock, backlog) == SOCKET_ERROR)
			ThrowSystemError();
	}

	int Socket::Receive(void *buf, std::size_t len, int flags)
	{
		int ret = recv(this->sock, (raw_type *)buf, (buf_size_type)len, flags);
		if (ret == SOCKET_ERROR)
			ThrowSystemError();
		return ret;
	}

	MemoryBlock Socket::Receive(std::size_t len, int flags)
	{
		MemoryBlock ret(len);
		ret.Resize(this->Receive(ret.GetBuf(), len, flags));
		return ret;
	}

	int Socket::ReceiveFrom(void *buf, std::size_t len, int flags, std::unique_ptr<EndPoint> &remoteEP)
	{
		SocketAddress addr(sizeof(sockaddr_storage));
		int size = (int)sizeof(sockaddr_storage);
		int ret = recvfrom(this->sock, (raw_type *)buf, (buf_size_type)len, flags, (sockaddr *)addr.GetData(), &size);
		if (ret == SOCKET_ERROR)
			ThrowSystemError();
		addr.Shrink((std::size_t)size);
		remoteEP = EndPointUtil::FromSocketAddress(addr);
		return ret;
	}

	MemoryBlock Socket::ReceiveFrom(std::size_t len, int flags, std::unique_ptr<EndPoint> &remoteEP)
	{
		MemoryBlock ret(len);
		ret.Resize(this->ReceiveFrom(ret.GetBuf(), len, flags, remoteEP));
		return ret;
	}

	short Socket::Poll(int microSeconds, short events)
	{
		pollfd fd;
		fd.fd = this->sock;
		fd.events = events;
		fd.revents = 0;
#ifdef _WIN32
		if (WSAPoll(&fd, 1, microSeconds) < 0)
#else
		if(poll(&fd,1,microSeconds)<0)
#endif
			ThrowSystemError();

		return fd.revents;
	}

	int Socket::Send(const void *buf, std::size_t len, int flags)
	{
		int ret = send(this->sock, (const raw_type *)buf, (buf_size_type)len, flags);
		if (ret == SOCKET_ERROR)
			ThrowSystemError();
		return ret;
	}

	void Socket::Send(const MemoryBlock &data, int flags)
	{
		std::size_t sentBytes = 0;
		while (sentBytes < data.GetSize())
			sentBytes += this->Send(data.GetBuf(), data.GetSize(), flags);
	}

	int Socket::SendTo(const void *buf, std::size_t len, int flags, const EndPoint &remoteEP)
	{
		SocketAddress addr = remoteEP.Serialize();
		int ret = sendto(this->sock, (const raw_type *)buf, (buf_size_type)len, flags, (sockaddr *)addr.GetData(), (int)addr.GetSize());
		if (ret == SOCKET_ERROR)
			ThrowSystemError();
		return ret;
	}

	void Socket::SendTo(const MemoryBlock &data, int flags, const EndPoint &remoteEP)
	{
		std::size_t sentBytes = 0;
		while (sentBytes < data.GetSize())
			sentBytes += this->SendTo(data.GetBuf(), data.GetSize(), flags, remoteEP);
	}

	void Socket::Shutdown(Socket::ShutdownMode how)
	{
		if (shutdown(this->sock, static_cast<int>(how)) == SOCKET_ERROR)
			ThrowSystemError();
	}



	Socket::socket_t Socket::GetFileHandle() const
	{
		return this->sock;
	}

	std::size_t Socket::GetAvailableBytesCount() const
	{
#ifdef _WIN32
		unsigned long ret;
		ioctlsocket(this->sock, FIONREAD, &ret);
		return (std::size_t)ret;
#elif
		int ret;
		ioctl(this->sock, FIONREAD, &ret);
		return (std::size_t)ret;
#endif
	}

	void Socket::SetNonblocking(bool val)
	{
#ifdef _WIN32
		u_long v = (u_long)val;
		ioctlsocket(this->sock, FIONBIO, &v);
#elif
		int v = (int)val;
		ioctl(this->sock, FIONBIO, &v);
#endif
	}

	bool Socket::GetBroadcast() const
	{
		int ret;
		int size = sizeof(ret);
		this->GetSocketOption(SOL_SOCKET, SO_BROADCAST, &ret, &size);
		return ret!=0;
	}
	void Socket::SetBroadcast(bool val)
	{
		int v = (int)val;
		this->SetSocketOption(SOL_SOCKET, SO_BROADCAST, &v, (int)sizeof(v));
	}

	bool Socket::GetReuseAddress() const
	{
		int ret;
		int size = sizeof(ret);
		this->GetSocketOption(SOL_SOCKET, SO_REUSEADDR, &ret, &size);
		return ret!=0;
	}
	void Socket::SetReuseAddress(bool val)
	{
		int v = (int)val;
		this->SetSocketOption(SOL_SOCKET, SO_REUSEADDR, &v, (int)sizeof(v));
	}
	linger Socket::GetLingerState() const
	{
		linger ret;
		int size = sizeof(ret);
		this->GetSocketOption(SOL_SOCKET, SO_LINGER, &ret, &size);
		return ret;
	}

	void Socket::SetLingerState(const linger &val)
	{
		this->SetSocketOption(SOL_SOCKET, SO_LINGER, (void *)&val, (int)sizeof(val));
	}

	int Socket::GetReceiveBufferSize() const
	{
		int ret;
		int size = sizeof(ret);
		this->GetSocketOption(SOL_SOCKET, SO_RCVBUF, &ret, &size);
		return ret;
	}
	void Socket::SetReceiveBufferSize(int val)
	{
		this->SetSocketOption(SOL_SOCKET, SO_RCVBUF, &val, (int)sizeof(val));
	}

	int Socket::GetSendBufferSize() const
	{
		int ret;
		int size = sizeof(ret);
		this->GetSocketOption(SOL_SOCKET, SO_SNDBUF, &ret, &size);
		return ret;
	}
	void Socket::SetSendBufferSize(int val)
	{
		this->SetSocketOption(SOL_SOCKET, SO_SNDBUF, &val, (int)sizeof(val));
	}

	int Socket::GetReceiveTimeout() const
	{
#ifdef _WIN32
		int ret;
		int size = sizeof(ret);
		this->GetSocketOption(SOL_SOCKET, SO_RCVTIMEO, &ret, &size);
		return ret;
#elif
		timeval ret;
		int size = sizeof(ret);
		this->GetSocketOption(SOL_SOCKET, SO_RCVTIMEO, &ret, &size);
		return (int)(ret.tv_sec * 1000 + ret.tv_usec / 1000);
#endif
	}
	void Socket::SetReceiveTimeout(int milliSeconds)
	{
#ifdef _WIN32
		DWORD v = (int)milliSeconds;
#elif
		timeval v;
		v.tv_sec = 0;
		v.tv_usec = timeout * milliSeconds;
#endif
		this->SetSocketOption(SOL_SOCKET, SO_RCVTIMEO, &v, (int)sizeof(v));
	}

	int Socket::GetSendTimeout() const
	{
#ifdef _WIN32
		int ret;
		int size = sizeof(ret);
		this->GetSocketOption(SOL_SOCKET, SO_SNDTIMEO, &ret, &size);
		return ret;
#elif
		timeval ret;
		int size = sizeof(ret);
		this->GetSocketOption(SOL_SOCKET, SO_SNDTIMEO, &ret, &size);
		return (int)(ret.tv_sec * 1000 + ret.tv_usec / 1000);
#endif
	}
	void Socket::SetSendTimeout(int milliSeconds)
	{
#ifdef _WIN32
		DWORD v = (int)milliSeconds;
#elif
		timeval v;
		v.tv_sec = 0;
		v.tv_usec = timeout * milliSeconds;
#endif
		this->SetSocketOption(SOL_SOCKET, SO_SNDTIMEO, &v, (int)sizeof(v));
	}
	
	std::unique_ptr<EndPoint> Socket::GetRemoteEndPoint() const
	{
		SocketAddress addr(sizeof(sockaddr_storage));
		int size = (int)sizeof(sockaddr_storage);
		if (getpeername(this->sock, (sockaddr *)addr.GetData(), &size) == SOCKET_ERROR)
			ThrowSystemError();
		addr.Shrink((std::size_t)size);
		return EndPointUtil::FromSocketAddress(addr);
	}

	std::unique_ptr<EndPoint> Socket::GetLocalEndPoint() const
	{
		SocketAddress addr(sizeof(sockaddr_storage));
		int size = (int)sizeof(sockaddr_storage);
		if (getsockname(this->sock, (sockaddr *)addr.GetData(), &size) == SOCKET_ERROR)
			ThrowSystemError();
		addr.Shrink((std::size_t)size);
		return EndPointUtil::FromSocketAddress(addr);
	}

}