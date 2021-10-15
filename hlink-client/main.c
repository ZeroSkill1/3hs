
#include "./hlink.h"

#include <stdio.h>


int main(int argc, char *argv[])
{

	hLink link;
	int res = 0;

	if((res = hl_makelink(&link, "192.168.2.22")) != 0)
		fprintf(stderr, "hl_makelink(): %s\n", hl_makelink_geterror(res));
	if((res = hl_auth(&link)) != 0)
		fprintf(stderr, "hl_auth(): %s\n", hl_geterror(res));

//	hl_launch(&link, 1125900166889728L); // fbi tid
//	hl_launch(&link, 1125900166889721L); // bad tid

//	uint64_t ids[2] = { 1, 2 };
//	if((res = hl_addqueue(&link, ids, 2)) != 0)
//		fprintf(stderr, "hl_addqueue(): %s\n", hl_geterror(res));

	if((res = hl_sleep(&link)) != 0)
		fprintf(stderr, "hl_sleep(): %s\n", hl_geterror(res));

	hl_destroylink(&link);

	return 0;
}

