#include <kore3/network/socket.h>

#include <kore3/log.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)

// Windows 7
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCOMM
#define NOCTLMGR
#define NODEFERWINDOWPOS
#define NODRAWTEXT
#define NOGDI
#define NOGDICAPMASKS
#define NOHELP
#define NOICONS
#define NOKANJI
#define NOKEYSTATES
#define NOMB
#define NOMCX
#define NOMEMMGR
#define NOMENUS
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NONLS
#define NOOPENFILE
#define NOPROFILER
#define NORASTEROPS
#define NOSCROLL
#define NOSERVICE
#define NOSHOWWINDOW
#define NOSOUND
#define NOSYSCOMMANDS
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOUSER
#define NOVIRTUALKEYCODES
#define NOWH
#define NOWINMESSAGES
#define NOWINOFFSETS
#define NOWINSTYLES
#define WIN32_LEAN_AND_MEAN

#include <Ws2tcpip.h>
#include <winsock2.h>
#elif defined(KORE_POSIX) || defined(KORE_EMSCRIPTEN)
#include <arpa/inet.h> // for inet_addr()
#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#if defined(KORE_EMSCRIPTEN)
#include <emscripten.h>
#include <emscripten/posix_socket.h>
#include <emscripten/threading.h>
#include <emscripten/websocket.h>

static EMSCRIPTEN_WEBSOCKET_T bridgeSocket = 0;
#elif defined(KORE_POSIX)
#include <sys/select.h>
#endif

static int counter = 0;

#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)
// Important: Must be cleaned with freeaddrinfo(address) later if the result is 0 in order to prevent memory leaks
static int resolveAddress(const char *url, int port, struct addrinfo **result) {
	struct addrinfo hints = {0};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	char serv[6];
	sprintf(serv, "%u", port);

	return getaddrinfo(url, serv, &hints, result);
}
#endif

bool kore_socket_bind(kore_socket *sock) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)
	struct sockaddr_in address;
	address.sin_family = sock->family == KORE_SOCKET_FAMILY_IP4 ? AF_INET : AF_INET6;
	address.sin_addr.s_addr = sock->host;
	address.sin_port = sock->port;
	if (bind(sock->handle, (const struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Could not bind socket: %s", strerror(errno));
		return false;
	}
	return true;
#else
	return false;
#endif
}

void kore_socket_options_set_defaults(kore_socket_parameters *parameters) {
	parameters->non_blocking = true;
	parameters->broadcast = false;
	parameters->tcp_no_delay = false;
}

void kore_socket_init(kore_socket *sock) {
	sock->handle = 0;

#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)
	sock->host = INADDR_ANY;
	sock->port = htons((unsigned short)8080);
	sock->protocol = KORE_SOCKET_PROTOCOL_TCP;
	sock->family = KORE_SOCKET_FAMILY_IP4;
#endif
	sock->connected = false;

#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
	if (counter == 0) {
		WSADATA WsaData;
		WSAStartup(MAKEWORD(2, 2), &WsaData);
	}
#if defined(KORE_EMSCRIPTEN)
	if (!bridgeSocket) {
		bridgeSocket = emscripten_init_websocket_to_posix_socket_bridge("ws://localhost:8080");
		// Synchronously wait until connection has been established.
		uint16_t readyState = 0;
		do {
			emscripten_websocket_get_ready_state(bridgeSocket, &readyState);
			emscripten_thread_sleep(100);
		} while (readyState == 0);
	}
#endif
#endif
	++counter;
}

bool kore_socket_open(kore_socket *sock, const struct kore_socket_parameters *parameters) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)
	switch (sock->protocol) {
	case KORE_SOCKET_PROTOCOL_UDP:
		sock->handle = socket(sock->family == KORE_SOCKET_FAMILY_IP4 ? AF_INET : AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
		break;
	case KORE_SOCKET_PROTOCOL_TCP:
		sock->handle = socket(sock->family == KORE_SOCKET_FAMILY_IP4 ? AF_INET : AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		break;
	default:
		kore_log(KORE_LOG_LEVEL_ERROR, "Unsupported socket protocol.");
		return false;
	}

	if (sock->handle <= 0) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Could not create socket.");
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
		int errorCode = WSAGetLastError();
		switch (errorCode) {
		case (WSANOTINITIALISED):
			kore_log(KORE_LOG_LEVEL_ERROR, "A successful WSAStartup call must occur before using this function.");
			break;
		case (WSAENETDOWN):
			kore_log(KORE_LOG_LEVEL_ERROR, "The network subsystem or the associated service provider has failed.");
			break;
		case (WSAEAFNOSUPPORT):
			kore_log(KORE_LOG_LEVEL_ERROR,
			         "The specified address family is not supported.For example, an application tried to create a socket for the AF_IRDA address "
			         "family but an infrared adapter and device driver is not installed on the local computer.");
			break;
		case (WSAEINPROGRESS):
			kore_log(KORE_LOG_LEVEL_ERROR,
			         "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
			break;
		case (WSAEMFILE):
			kore_log(KORE_LOG_LEVEL_ERROR, "No more socket descriptors are available.");
			break;
		case (WSAEINVAL):
			kore_log(KORE_LOG_LEVEL_ERROR,
			         "An invalid argument was supplied.This error is returned if the af parameter is set to AF_UNSPEC and the type and protocol "
			         "parameter are unspecified.");
			break;
		case (WSAENOBUFS):
			kore_log(KORE_LOG_LEVEL_ERROR, "No buffer space is available.The socket cannot be created.");
			break;
		case (WSAEPROTONOSUPPORT):
			kore_log(KORE_LOG_LEVEL_ERROR, "The specified protocol is not supported.");
			break;
		case (WSAEPROTOTYPE):
			kore_log(KORE_LOG_LEVEL_ERROR, "The specified protocol is the wrong type for this socket.");
			break;
		case (WSAEPROVIDERFAILEDINIT):
			kore_log(KORE_LOG_LEVEL_ERROR,
			         "The service provider failed to initialize.This error is returned if a layered service provider(LSP) or namespace provider was "
			         "improperly installed or the provider fails to operate correctly.");
			break;
		case (WSAESOCKTNOSUPPORT):
			kore_log(KORE_LOG_LEVEL_ERROR, "The specified socket type is not supported in this address family.");
			break;
		case (WSAEINVALIDPROVIDER):
			kore_log(KORE_LOG_LEVEL_ERROR, "The service provider returned a version other than 2.2.");
			break;
		case (WSAEINVALIDPROCTABLE):
			kore_log(KORE_LOG_LEVEL_ERROR, "The service provider returned an invalid or incomplete procedure table to the WSPStartup.");
			break;
		default:
			kore_log(KORE_LOG_LEVEL_ERROR, "Unknown error.");
		}
#elif defined(KORE_POSIX) && !defined(KORE_EMSCRIPTEN)
		kore_log(KORE_LOG_LEVEL_ERROR, "%s", strerror(errno));
#endif
		return false;
	}
#endif

	if (parameters != NULL) {
		if (parameters->non_blocking) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
			DWORD value = 1;
			if (ioctlsocket(sock->handle, FIONBIO, &value) != 0) {
				kore_log(KORE_LOG_LEVEL_ERROR, "Could not set non-blocking mode.");
				return false;
			}
#elif defined(KORE_POSIX)
			int value = 1;
			if (fcntl(sock->handle, F_SETFL, O_NONBLOCK, value) == -1) {
				kore_log(KORE_LOG_LEVEL_ERROR, "Could not set non-blocking mode.");
				return false;
			}
#endif
		}

		if (parameters->broadcast) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)
			int value = 1;
			if (setsockopt(sock->handle, SOL_SOCKET, SO_BROADCAST, (const char *)&value, sizeof(value)) < 0) {
				kore_log(KORE_LOG_LEVEL_ERROR, "Could not set broadcast mode.");
				return false;
			}
#endif
		}

		if (parameters->tcp_no_delay) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)
			int value = 1;
			if (setsockopt(sock->handle, IPPROTO_TCP, TCP_NODELAY, (const char *)&value, sizeof(value)) != 0) {
				kore_log(KORE_LOG_LEVEL_ERROR, "Could not set no-delay mode.");
				return false;
			}
#endif
		}
	}

	return true;
}

void kore_socket_destroy(kore_socket *sock) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
	closesocket(sock->handle);
#elif defined(KORE_POSIX)
	close(sock->handle);
#endif

	memset(sock, 0, sizeof(kore_socket));

	--counter;
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
	if (counter == 0) {
		WSACleanup();
	}
#endif
}

bool kore_socket_select(kore_socket *sock, uint32_t waittime, bool read, bool write) {
#if !defined(KORE_EMSCRIPTEN) && (defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX))
	fd_set r_fds, w_fds;
	struct timeval timeout;

	FD_ZERO(&r_fds);
	FD_ZERO(&w_fds);

	FD_SET(sock->handle, &r_fds);
	FD_SET(sock->handle, &w_fds);

	timeout.tv_sec = waittime;
	timeout.tv_usec = 0;

	if (select(0, &r_fds, &w_fds, NULL, &timeout) < 0) {
		kore_log(KORE_LOG_LEVEL_ERROR, "kore_socket_select didn't work: %s", strerror(errno));
		return false;
	}

	if (read && write) {
		return FD_ISSET(sock->handle, &w_fds) && FD_ISSET(sock->handle, &r_fds);
	}
	else if (read) {
		return FD_ISSET(sock->handle, &r_fds);
	}
	else if (write) {
		return FD_ISSET(sock->handle, &w_fds);
	}
	else {
		kore_log(KORE_LOG_LEVEL_ERROR, "Calling kore_socket_select with both read and write set to false is useless.");
		return false;
	}
#else
	return false;
#endif
}

bool kore_socket_set(kore_socket *sock, const char *host, int port, kore_socket_family family, kore_socket_protocol protocol) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)

	sock->family = family;
	sock->protocol = protocol;
	sock->port = htons((unsigned short)port);

	if (host == NULL)
		return true;

	if (isdigit(host[0]) || (family == KORE_SOCKET_FAMILY_IP6 && host[4] == ':')) { // Is IPv4 or IPv6 string
		struct in_addr addr;

		if (inet_pton(sock->family == KORE_SOCKET_FAMILY_IP4 ? AF_INET : AF_INET6, host, &addr) == 0) {
			kore_log(KORE_LOG_LEVEL_ERROR, "Invalid %s address: %s\n", sock->family == KORE_SOCKET_FAMILY_IP4 ? "IPv4" : "IPv6", host);
			return false;
		}
		sock->host = addr.s_addr;
		return true;
	}
	else {
		struct addrinfo *address = NULL;
		int res = resolveAddress(host, port, &address);
		if (res != 0) {
			kore_log(KORE_LOG_LEVEL_ERROR, "Could not resolve address.");
			return false;
		}
#if defined(KORE_POSIX)
		sock->host = ((struct sockaddr_in *)address->ai_addr)->sin_addr.s_addr;
#else
		sock->host = ((struct sockaddr_in *)address->ai_addr)->sin_addr.S_un.S_addr;
#endif
		freeaddrinfo(address);

		return true;
	}
#else
	return false;
#endif
}

bool kore_socket_listen(kore_socket *socket, int backlog) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)
	int res = listen(socket->handle, backlog);
	return (res == 0);
#else
	return false;
#endif
}

bool kore_socket_accept(kore_socket *sock, kore_socket *newSocket, unsigned *remoteAddress, unsigned *remotePort) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
	typedef int socklen_t;
#endif
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)
	struct sockaddr_in addr;
	socklen_t addrLength = sizeof(addr);
	newSocket->handle = accept(sock->handle, (struct sockaddr *)&addr, &addrLength);
	if (newSocket->handle <= 0) {
		return false;
	}

	newSocket->connected = sock->connected = true;
	newSocket->host = addr.sin_addr.s_addr;
	newSocket->port = addr.sin_port;
	newSocket->family = sock->family;
	newSocket->protocol = sock->protocol;
	*remoteAddress = ntohl(addr.sin_addr.s_addr);
	*remotePort = ntohs(addr.sin_port);
	return true;
#else
	return false;
#endif
}

bool kore_socket_connect(kore_socket *sock) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)
	struct sockaddr_in addr;
	addr.sin_family = sock->family == KORE_SOCKET_FAMILY_IP4 ? AF_INET : AF_INET6;
	addr.sin_addr.s_addr = sock->host;
	addr.sin_port = sock->port;

	int res = connect(sock->handle, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	return sock->connected = (res == 0);
#else
	return false;
#endif
}

int kore_socket_send(kore_socket *sock, const uint8_t *data, int size) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
	typedef int socklen_t;
#endif
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)
	if (sock->protocol == KORE_SOCKET_PROTOCOL_UDP) {
		struct sockaddr_in addr;
		addr.sin_family = sock->family == KORE_SOCKET_FAMILY_IP4 ? AF_INET : AF_INET6;
		addr.sin_addr.s_addr = sock->host;
		addr.sin_port = sock->port;

		size_t sent = sendto(sock->handle, (const char *)data, size, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
		if (sent != size) {
			kore_log(KORE_LOG_LEVEL_ERROR, "Could not send packet.");
			return -1;
		}
		return (int)sent;
	}
	else {
		if (!sock->connected) {
			kore_log(KORE_LOG_LEVEL_ERROR, "Call kore_sockect_connect/bind before send/recv can be called for TCP sockets.");
			return -1;
		}

		size_t sent = send(sock->handle, (const char *)data, size, 0);
		if (sent != size) {
			kore_log(KORE_LOG_LEVEL_ERROR, "Could not send packet.");
		}
		return (int)sent;
	}
#else
	return 0;
#endif
}

int kore_socket_send_address(kore_socket *sock, unsigned address, int port, const uint8_t *data, int size) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(address);
	addr.sin_port = htons(port);

	size_t sent = sendto(sock->handle, (const char *)data, size, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	if (sent != size) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Could not send packet.");
	}
	return (int)sent;
#else
	return 0;
#endif
}

int kore_socket_send_url(kore_socket *sock, const char *url, int port, const uint8_t *data, int size) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)
	struct addrinfo *address = NULL;
	int res = resolveAddress(url, port, &address);
	if (res != 0) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Could not resolve address.");
		return 0;
	}

	size_t sent = sendto(sock->handle, (const char *)data, size, 0, address->ai_addr, sizeof(struct sockaddr_in));
	if (sent != size) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Could not send packet.");
	}
	freeaddrinfo(address);
	return (int)sent;
#else
	return 0;
#endif
}

int kore_socket_receive(kore_socket *sock, uint8_t *data, int maxSize, unsigned *fromAddress, unsigned *fromPort) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
	typedef int socklen_t;
	typedef int ssize_t;
#endif
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP) || defined(KORE_POSIX)

	if (sock->protocol == KORE_SOCKET_PROTOCOL_UDP) {
		struct sockaddr_in from;
		socklen_t fromLength = sizeof(from);
		ssize_t bytes = recvfrom(sock->handle, (char *)data, maxSize, 0, (struct sockaddr *)&from, &fromLength);
		if (bytes <= 0) {
			return (int)bytes;
		}
		*fromAddress = ntohl(from.sin_addr.s_addr);
		*fromPort = ntohs(from.sin_port);
		return (int)bytes;
	}
	else {

		if (!sock->connected) {
			kore_log(KORE_LOG_LEVEL_ERROR, "Call kore_sockect_connect/bind before send/recv can be called for TCP sockets.");
			return -1;
		}
		ssize_t bytes = recv(sock->handle, (char *)data, maxSize, 0);
		*fromAddress = ntohl(sock->host);
		*fromPort = ntohs(sock->port);
		return (int)bytes;
	}
#else
	return 0;
#endif
}

unsigned kore_url_to_int(const char *url, int port) {
#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
	struct addrinfo *address = NULL;
	int res = resolveAddress(url, port, &address);
	if (res != 0) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Could not resolve address.");
		return -1;
	}

	unsigned fromAddress = ntohl(((struct sockaddr_in *)address->ai_addr)->sin_addr.S_un.S_addr);
	freeaddrinfo(address);

	return fromAddress;
#else
	return 0;
#endif
}
