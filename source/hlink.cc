
#include "hlink.hh"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>

#include <string.h>
#include <panic.hh>
#include <errno.h>

#include <map>

#include "queue.hh"
#include "hs.hh"

#include <curl/curl.h>

// All network byte order (BE)

typedef struct iTransactionHeader
{
	char magic[hlink::transaction_magic_len];
	hlink::action action;
	uint64_t size;
} __attribute__((__packed__)) iTransactionHeader;

typedef struct iTransactionResponse
{
	char magic[hlink::transaction_magic_len];
	hlink::response resp;
	uint64_t size;
} __attribute__((__packed__)) iTransactionResponse;

static bool g_lock = false; // is a hlink transaction going on?

// Why do these not exist already?
static uint64_t htonll(uint64_t n)
{ return __builtin_bswap64(n); }

static uint64_t ntohll(uint64_t n)
{ return __builtin_bswap64(n); }

static const char *action2string(hlink::action action)
{
#define MKS(n) case hlink::action::n: return #n
	switch(action)
	{
		MKS(add_queue);
		MKS(install_id);
		MKS(install_url);
		MKS(install_data);
		MKS(nothing);
		default:
			return "unknown";
	}
#undef MKS
}

static void send_response(int clientfd, hlink::response resp, const std::string& body)
{
	iTransactionResponse respb;
	strncpy(respb.magic, hlink::transaction_magic, hlink::transaction_magic_len + 1);
	respb.size = htonll(body.size());
	respb.resp = resp;

	send(clientfd, &respb, sizeof(iTransactionResponse), 0);
	send(clientfd, body.c_str(), body.size(), 0);
}

static void send_response(int clientfd, hlink::response resp)
{
	iTransactionResponse respb;
	strncpy(respb.magic, hlink::transaction_magic, hlink::transaction_magic_len + 1);
	respb.resp = resp;
	respb.size = 0;

	send(clientfd, &respb, sizeof(iTransactionResponse), 0);
}

static int read_whole_body(int clientfd, std::string& ret, iTransactionHeader header)
{
	ret.reserve(header.size);

	struct pollfd clientpoll;
	clientpoll.fd = clientfd;
	clientpoll.events = POLLIN;

	char buf[1024];
	int seqbad = 0;

	do
	{
		if(seqbad > hlink::max_timeouts) break;
		if(poll(&clientpoll, 1, hlink::poll_timeout) == 0)
		{
			++seqbad;
			continue;
		}
		else seqbad = 0;

		ssize_t recvd = recv(clientfd, buf, 1024, 0);
		if(recvd < 0) break;
		ret += std::string(buf, recvd);
	} while(ret.size() != header.size);

	if(ret.size() < header.size) return errno;
	return 0;
}

static void handle_add_queue(int clientfd, iTransactionHeader header, std::function<void(const std::string&)> disp_error)
{
	std::string body;
	if(read_whole_body(clientfd, body, header) != 0)
		return;

	if(body.size() % sizeof(u64) != 0)
		return send_response(clientfd, hlink::response::error, "body.size() % sizeof(u64) != 0");

	for(size_t i = 0; i < body.size() / sizeof(u64); ++i)
	{
		hs::hid id = ntohll(((const u64 *) body.data())[i]);
		hs::FullTitle meta = hs::title_meta(id);

		// ~~Check if id exists~~ its UB now
		queue_add(meta);
	}

	send_response(clientfd, hlink::response::success);
}

static void handle_request(int clientfd, std::function<void(const std::string&)> disp_error)
{
	iTransactionHeader header;
	ssize_t recvd = recv(clientfd, &header, sizeof(header), 0);
	if(recvd < 0) // error
		goto cleanup;

	header.size = ntohll(header.size);
	if(memcmp(header.magic, hlink::transaction_magic, hlink::transaction_magic_len) != 0)
	{
		send_response(clientfd, hlink::response::error, "invalid magic");
		goto cleanup;
	}

	switch(header.action)
	{
	case hlink::action::add_queue:
		handle_add_queue(clientfd, header, disp_error);
		break;
	case hlink::action::install_id:
	case hlink::action::install_url:
	case hlink::action::install_data:
		send_response(clientfd, hlink::response::error, "stub");
		break;
	case hlink::action::nothing:
		send_response(clientfd, hlink::response::accept);
		break;
	default:
		send_response(clientfd, hlink::response::error, "invalid action");
		break;
	}

cleanup:
	close(clientfd);
	g_lock = false;
}

void hlink::create_server(
		std::function<bool(const std::string&)> on_requester,
		std::function<void(const std::string&)> disp_error,
		std::function<void(const std::string&)> on_server_create,
		std::function<bool()> on_poll_exit
	)
{
	int serverfd = -1;
	if((serverfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		disp_error("socket(): " + std::string(strerror(errno)));
		return;
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0x0, sizeof(servaddr));
	servaddr.sin_family = AF_INET; // IPv4 only (3ds doesn't support IPv6)
	servaddr.sin_addr.s_addr = gethostid();
	servaddr.sin_port = htons(hlink::port);

	if(bind(serverfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		disp_error("bind(): " + std::string(strerror(errno)));
		close(serverfd);
		return;
	}

	if(listen(serverfd, hlink::backlog) < 0)
	{
		disp_error("listen(" + std::to_string(errno) + "): " + std::string(strerror(errno)));
		close(serverfd);
		return;
	}

	// Now we keep polling
	struct pollfd serverpoll;
	serverpoll.fd = serverfd;
	serverpoll.events = POLLIN;

	struct sockaddr_in clientaddr;
	socklen_t clientaddrlen = sizeof(clientaddr);
	memset(&clientaddr, 0x0, sizeof(clientaddr));

	std::map<in_addr_t, bool> truststore;

begin_loop:
	const char *ipaddr = inet_ntoa(servaddr.sin_addr);
	on_server_create(ipaddr); // We might need to redraw the screen

	while(on_poll_exit())
	{
		if(poll(&serverpoll, 1, hlink::poll_timeout) == 0)
			continue; // no events; we do nothing
		if(!(serverpoll.events & POLLIN))
			continue; // this shouldn't happen

		// NOW we need to accept() our connection
		int clientfd = accept(serverfd, (struct sockaddr *) &clientaddr, &clientaddrlen);

		if(clientfd < 0)
		{
			disp_error("accept(): " + std::string(strerror(errno)));
			goto begin_loop;
		}

		if(g_lock) // we can't have more than 2 connections, so we just say bAi
		{
			send_response(clientfd, hlink::response::busy);
			close(clientfd);
			goto begin_loop;
		}

		// Spin off on a seperate thread to not block our next polls

		if(truststore.count(clientaddr.sin_addr.s_addr) > 0 && !truststore[clientaddr.sin_addr.s_addr])
		{
			send_response(clientfd, hlink::response::untrusted);
			close(clientfd);
			goto begin_loop;
		}

		else if(truststore.count(clientaddr.sin_addr.s_addr) == 0)
		{
			const char *clientipaddr = inet_ntoa(clientaddr.sin_addr);
			bool trusted = on_requester(clientipaddr);

			truststore[clientaddr.sin_addr.s_addr] = trusted;
			if(!trusted)
			{
				send_response(clientfd, hlink::response::untrusted);
				close(clientfd);
				goto begin_loop;
			}
		}

		g_lock = true;
		handle_request(clientfd, disp_error);
		goto begin_loop;
	}

	close(serverfd);
	return;
}


