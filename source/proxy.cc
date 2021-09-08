
#include "proxy.hh"

#include <stdio.h>

#include <string>

static std::string g_proxystr;


void proxy::apply(CURL *curl)
{
	if(g_proxystr != "")
		curl_easy_setopt(curl, CURLOPT_PROXY, g_proxystr.c_str());
}

void proxy::init()
{
	FILE *proxyfile = fopen("/3ds/3hs/proxy", "r");
	if(proxyfile == NULL) return;

	fseek(proxyfile, 0, SEEK_END);
	size_t fsize = ftell(proxyfile);
	fseek(proxyfile, 0, SEEK_SET);

	char *buf = new char[fsize];
	if(fread(buf, 1, fsize, proxyfile) == fsize)
		g_proxystr = std::string(buf, fsize);

	fclose(proxyfile);
	delete [] buf;
}

