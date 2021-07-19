
#include "./hlink.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>

#define MAGIC_LEN 3
#define MAGIC "HLT"
#define PORT "37283"

#if __BYTEORDER__ == __ORDER_LITTLE
// Why do these not exist already?
static uint64_t htonll(uint64_t n)
{ return __builtin_bswap64(n); }

static uint64_t ntohll(uint64_t n)
{ return __builtin_bswap64(n); }
#else
#define htonll(n) n
#define ntohll(n) n
#endif

typedef struct iTransactionHeader
{
	char magic[MAGIC_LEN];
	uint8_t action; // enum HAction
	uint64_t size;
} __attribute__((__packed__)) iTransactionHeader;

typedef struct iTransactionResponse
{
	char magic[MAGIC_LEN];
	uint8_t resp; // enum HResponse
	uint64_t size;
} __attribute__((__packed__)) iTransactionResponse;

static int makesock(hLink *link)
{
	if(link->host == NULL) return -ENXIO;

	int sock = socket(link->host->ai_family, link->host->ai_socktype, link->host->ai_protocol);
	if(sock < 0) return -errno;

	if(connect(sock, link->host->ai_addr, link->host->ai_addrlen) < 0)
	{ close(sock); return -errno; }

	return sock;
}

static iTransactionHeader makeheader(uint8_t action, uint64_t size)
{
	iTransactionHeader header;
	strncpy(header.magic, MAGIC, MAGIC_LEN + 1);
	header.size = htonll(size);
	header.action = action;
	return header;
}

static int readresp(iTransactionResponse *resp, int sock)
{
	ssize_t recvd = recv(sock, resp, sizeof(iTransactionResponse), 0);
	if(recvd < 0) return -errno;
	resp->size = ntohll(resp->size);
	return 0;
}

const char *hl_makelink_geterror(int errcode)
{
	if(errcode > 0)
		return strerror(errcode);
	else if(errcode < 0)
		return strerror(-errcode);
	else return "success";
}

const char *hl_geterror(int errcode)
{
	if(errcode > 0)
	{
		switch(errcode)
		{
		case HE_notauthed:
			return "not authenticated";
		case HE_tryagain:
			return "server busy. try again";
		case HE_tidnotfound:
			return "title id not found on the host 3ds";
		}
		return "unknown";
	}
	else if(errcode < 0)
		return strerror(-errcode);
	else return "success";
}

int hl_makelink(hLink *link, const char *addr)
{
	struct addrinfo hints;
	memset(&hints, 0x0, sizeof(hints));
	hints.ai_family = AF_INET; // 3ds only supports IPv4
	hints.ai_socktype = SOCK_STREAM;

	int res = getaddrinfo(addr, PORT, &hints, &link->host);
	if(res < 0) { link->host = NULL; return res; }

	link->isauthed = 0;
	return 0;
}

void hl_destroylink(hLink *link)
{
	if(link->host != NULL)
		freeaddrinfo(link->host);
}

int hl_auth(hLink *link)
{
	if(link->isauthed) return 0;
	int sock = makesock(link);
	if(sock < 0) return sock;

	iTransactionHeader header = makeheader(HA_nothing, 0);
	if(send(sock, &header, sizeof(iTransactionHeader), 0) < 0)
	{ close(sock); return -errno; }

	iTransactionResponse resp;
	int ret = readresp(&resp, sock);

	if(ret == 0)
	{
		if(resp.resp == HR_busy)
			ret = HE_tryagain;
		else if(resp.resp == HR_untrusted)
			ret = HE_notauthed;
		else link->isauthed = 1;
	}

	close(sock);
	return ret;
}

int hl_addqueue(hLink *link, uint64_t *ids, size_t amount)
{
	if(!link->isauthed) return HE_notauthed;
	int sock = makesock(link);
	if(sock < 0) return -errno;

	iTransactionHeader header = makeheader(HA_add_queue, amount * sizeof(uint64_t));
	if(send(sock, &header, sizeof(iTransactionHeader), 0) < 0)
	{ close(sock); return -errno; }

	uint64_t *body = malloc(amount * sizeof(uint64_t));
	for(size_t i = 0; i < amount; ++i)
		body[i] = htonll(ids[i]);
	ssize_t sent = send(sock, body, amount * sizeof(uint64_t), 0);
	free(body);

	if(sent < 0)
	{ close(sock); return -errno; }

	// TODO: parse response (check for not auth/busy)

	close(sock);
	return HE_success;
}

int hl_launch(hLink *link, uint64_t tid)
{
	if(!link->isauthed) return HE_notauthed;
	int sock = makesock(link);
	if(sock < 0) return -errno;

	iTransactionHeader header = makeheader(HA_launch, sizeof(uint64_t));
	if(send(sock, &header, sizeof(iTransactionHeader), 0) < 0)
	{ close(sock); return -errno; }

	uint64_t ntid = htonll(tid);
	if(send(sock, &ntid, sizeof(uint64_t), 0) < 0)
	{ close(sock); return -errno; }

	// TODO: parse response (check for not auth/busy)

	close(sock);
	return HE_success;
}

