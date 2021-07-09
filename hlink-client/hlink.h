
#ifndef inc_hlink_h
#define inc_hlink_h

#include <netdb.h>

enum HAction
{
	HA_add_queue    = 0,
	HA_install_id   = 1,
	HA_install_url  = 2,
	HA_install_data = 3,
	HA_nothing      = 4,
};

enum HResponse
{
	HR_accept       = 0,
	HR_busy         = 1,
	HR_untrusted    = 2,
	HR_error        = 3,
	HR_success      = 4,
};

enum HError
{
	HE_notauthed    = 1, /* you didn't call hl_auth() or hl_auth() didn't return 0 */
	HE_tryagain     = 2, /* try again, caused by the server being busy */
};

typedef struct hLink
{
	struct addrinfo *host;
	int isauthed;
} hLink;

/* connects a link, get an error with hl_makelink_geterror */
int hl_makelink(hLink *link, const char *addr);
/* frees memory used by link */
void hl_destroylink(hLink *link);
/* gets a human readable error string from hl_makelink */
const char *hl_makelink_geterror(int errcode);
/* gets a human readable error string */
const char *hl_geterror(int errcode);
/* authenticates with the server */
int hl_auth(hLink *link);
/* sends hshop ids to add to the queue */
int hl_addqueue(hLink *link, uint64_t *ids, size_t amount);

#endif

