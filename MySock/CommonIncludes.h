#pragma once
#ifdef _WIN32
#pragma comment(lib, "wsock32.lib")
#pragma comment (lib, "Ws2_32.lib")
//#undef UNICODE

#include <WinSock2.h>
#include <Ws2tcpip.h>
typedef char raw_type;
typedef short sa_family_t;
typedef unsigned short in_port_t;
typedef int buf_size_type;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>
typedef void raw_type;
typedef std::size_t buf_size_type;
#endif // !_WIN32