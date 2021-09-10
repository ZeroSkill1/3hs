
#include "proxy.hh"

#include <3rd/log.hh>
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

	std::string::size_type nl = g_proxystr.find("\n");
	if(nl != std::string::npos) g_proxystr.erase(g_proxystr.begin() + nl, g_proxystr.end());
	llog << "Using proxy: |" << g_proxystr << "|";

	fclose(proxyfile);
	delete [] buf;
}

