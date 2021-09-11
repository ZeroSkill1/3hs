
#include "proxy.hh"

#include <3rd/log.hh>
#include <stdio.h>

#include <string>

#include "install.hh"
#include "panic.hh"
#include "i18n.hh"

struct _proxy
{
	std::string host;
	u16 port = 0;

	std::string username;
	std::string password;
} static g_proxy;

// This is for curl.
// If we simple do proxystr().c_str() the
// pointer becomes invalid -> segfault
static std::string g_proxystr;


static std::string proxystr()
{
	if(g_proxy.username != "")
	{
		return "http://" + g_proxy.username + ":" + g_proxy.password + "@"
			+ g_proxy.host + ":" + std::to_string(g_proxy.port);
	}

	return "http://" + g_proxy.host + ":" + std::to_string(g_proxy.port);
}

static bool validate()
{
	if(g_proxy.host == "")
		return true;

	// 0xFFFF = overflow, 0xFFFF+ are invalid ports
	if(g_proxy.port == 0 || g_proxy.port >= 0xFFFF)
		return false;

	if(
		(g_proxy.username != "" && g_proxy.password == "") ||
		(g_proxy.username == "" && g_proxy.password != "")
	) return false;

	return true;
}

void proxy::apply(CURL *curl)
{
	if(g_proxy.host != "")
	{
		curl_easy_setopt(curl, CURLOPT_PROXY, g_proxystr.c_str());
	}
}

Result proxy::apply(httpcContext *context)
{
	return httpcSetProxy(context, g_proxy.port, g_proxy.host,
		g_proxy.username, g_proxy.password);
}

static bool is_CRLF(const std::string& buf, std::string::size_type i)
{
	return buf[i == 0 ? 0 : i - 1] == '\r';
}

static void put_semisep(const std::string& buf, std::string& p1, std::string& p2)
{
	std::string::size_type semi = buf.find(":");
	if(semi == std::string::npos) panic(STRING(invalid_proxy));

	p1 = buf.substr(0, semi);
	// +1 to remove semi
	p2 = buf.substr(semi + 1);
}

void proxy::init()
{
	FILE *proxyfile = fopen("/3ds/3hs/proxy", "r");
	if(proxyfile == NULL) return;

	fseek(proxyfile, 0, SEEK_END);
	size_t fsize = ftell(proxyfile);
	fseek(proxyfile, 0, SEEK_SET);

	if(fsize == 0)
	{
		fclose(proxyfile);
		return;
	}

	char *buf = new char[fsize];
	if(fread(buf, 1, fsize, proxyfile) != fsize)
		panic(STRING(invalid_proxy));
	std::string strbuf(buf, fsize);

	fclose(proxyfile);
	delete [] buf;

	std::string::size_type line = strbuf.find("\n");
	if(line == std::string::npos)
		line = strbuf.size() - 1;

	bool crlf = is_CRLF(strbuf, line);
	std::string proxyport = strbuf.substr(0, crlf ? line - 1 : line);

	if(line != strbuf.size() - 1)
	{
		std::string::size_type lineuser = strbuf.find("\n", line);
		if(line == std::string::npos)
			line = strbuf.size() - 1;
		crlf = is_CRLF(strbuf, lineuser);

		// Skip \n
		std::string userpasswd = strbuf.substr(line + 1, crlf ? line - 1 : line);
		if(userpasswd != "")
			put_semisep(userpasswd, g_proxy.username, g_proxy.password);
	}

	std::string port;
	put_semisep(proxyport, g_proxy.host, port);

	g_proxy.port = strtoul(port.c_str(), nullptr, 10);
	if(!validate()) panic(STRING(invalid_proxy));
	g_proxystr = proxystr();

	llog << "Using proxy: |" << g_proxystr << "|";
}


