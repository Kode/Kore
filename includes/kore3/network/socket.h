#ifndef KORE_NETWORK_SOCKET_HEADER
#define KORE_NETWORK_SOCKET_HEADER

#include <kinc/global.h>

/*! \file socket.h
    \brief Provides low-level network-communication via UDP or TCP-sockets.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef enum kore_socket_protocol { KORE_SOCKET_PROTOCOL_UDP, KORE_SOCKET_PROTOCOL_TCP } kore_socket_protocol;

typedef enum kore_socket_family { KORE_SOCKET_FAMILY_IP4, KORE_SOCKET_FAMILY_IP6 } kore_socket_family;

#ifdef KINC_MICROSOFT
#if defined(_WIN64)
typedef unsigned __int64 UINT_PTR, *PUINT_PTR;
#else
#if !defined _W64
#define _W64
#endif
typedef _W64 unsigned int UINT_PTR, *PUINT_PTR;
#endif
typedef UINT_PTR SOCKET;
#endif

typedef struct kore_socket {
#ifdef KINC_MICROSOFT
	SOCKET handle;
#else
	int handle;
#endif
	uint32_t host;
	uint32_t port;
	kore_socket_protocol protocol;
	kore_socket_family family;
	bool connected;
} kore_socket;

typedef struct kore_socket_parameters {
	bool non_blocking;
	bool broadcast;
	bool tcp_no_delay;
} kore_socket_parameters;

/// <summary>
/// Initializes a socket-options-object to the default parameters
/// </summary>
/// <param name="options">The new default options</param>
KORE_FUNC void kore_socket_parameters_set_defaults(kore_socket_parameters *parameters);

/// <summary>
/// Initializes a socket-object. To set the host and port use kinc_socket_set.
/// Host will be localhost
/// Port will be 8080
/// Family will be IPv4
/// Protocol will be TCP
/// </summary>
/// <param name="socket">The socket to initialize</param>
KORE_FUNC void kore_socket_init(kore_socket *socket);

/// <summary>
/// Sets the sockets properties.
/// </summary>
/// <param name="socket">The socket-object to use</param>
/// <param name="host">The host to use as IP or URL</param>
/// <param name="port">The port to use</param>
/// <param name="family">The IP-family to use</param>
/// <param name="protocol">The protocol to use</param>
/// <returns>Whether the socket was set correctly</returns>
KORE_FUNC bool kore_socket_set(kore_socket *socket, const char *host, int port, kore_socket_family family, kore_socket_protocol protocol);

/// <summary>
/// Destroys a socket-object.
/// </summary>
/// <param name="socket">The socket to destroy</param>
KORE_FUNC void kore_socket_destroy(kore_socket *socket);

/// <summary>
/// Opens a socket-connection.
/// </summary>
/// <param name="socket">The socket-object to use</param>
/// <param name="protocol">The protocol to use</param>
/// <param name="port">The port to use</param>
/// <param name="options">The options to use</param>
/// <returns>Whether the socket-connection could be opened</returns>
KORE_FUNC bool kore_socket_open(kore_socket *socket, const kore_socket_parameters *parameters);

/// <summary>
/// For use with non-blocking sockets to try to see if we are connected.
/// </summary>
/// <param name="socket">The socket-object to use</param>
/// <param name="waittime">The amount of time in seconds the select function will timeout.</param>
/// <param name="read">Check if the socket is ready to be read from.</param>
/// <param name="write">Check if the socket is ready to be written to.</param>
/// <returns>Whether the socket-connection can read or write or checks both.</returns>
KORE_FUNC bool kore_socket_select(kore_socket *socket, uint32_t waittime, bool read, bool write);

/*Typically these are server actions.*/
KORE_FUNC bool kore_socket_bind(kore_socket *socket);
KORE_FUNC bool kore_socket_listen(kore_socket *socket, int backlog);
KORE_FUNC bool kore_socket_accept(kore_socket *socket, kore_socket *new_socket, unsigned *remote_address, unsigned *remote_port);

/*Typically this is a client action.*/
KORE_FUNC bool kore_socket_connect(kore_socket *socket);

KORE_FUNC int kore_socket_send(kore_socket *socket, const uint8_t *data, int size);
KORE_FUNC int kore_socket_send_address(kore_socket *socket, unsigned address, int port, const uint8_t *data, int size);
KORE_FUNC int kore_socket_send_url(kore_socket *socket, const char *url, int port, const uint8_t *data, int size);
KORE_FUNC int kore_socket_receive(kore_socket *socket, uint8_t *data, int maxSize, unsigned *from_address, unsigned *from_port);

/// <summary>
/// Resolves a DNS-entry to an IP and returns its integer representation.
/// </summary>
/// <param name="url"></param>
/// <param name="port"></param>
/// <returns></returns>
KORE_FUNC unsigned kore_url_to_int(const char *url, int port);

#ifdef __cplusplus
}
#endif

#endif
