
#ifndef inc_hlink_http_hh
#define inc_hlink_http_hh

#include <arpa/inet.h>

#include <string>
#include <map>


namespace hlink
{
	using HTTPHeaders = std::map<std::string, std::string>;

	class HTTPServer; /* forward decl */
	struct HTTPRequestContext
	{
		HTTPServer *server;
		HTTPHeaders headers; /* headers are always lowercased */
		std::string method; /* method is always lowercased */
		struct sockaddr_in clientaddr;
		std::string path;
		char buf[4096];
		size_t buflen;
		bool iseof;
		int fd;

		inline bool is_get() { return this->method == "get"; }
		void serve_path(int status, const std::string& name, HTTPHeaders headers);
		void respond(int status, const std::string& data, HTTPHeaders headers);
		void respond_chunked(int status, HTTPHeaders headers);
		void respond(int status, const HTTPHeaders& headers);
		void redirect(const std::string& location);
		void send_chunk(const std::string& data);
		void send(const std::string& data);
		bool try_serve(); /* returns if you need to handle this request yourself */
		void close();

		inline void serve_403() { this->serve_path(403, "/403.html", { }); }
		inline void serve_404() { this->serve_path(404, "/404.html", { }); }
		inline void serve_500() { this->serve_path(500, "/500.html", { }); }
	};

	class HTTPServer
	{
	public:
		/* finds a new fd to make a context with */
		int make_reqctx(HTTPRequestContext& ctx);
		int make_fd();

		void close();

		int fd = -1;

		static std::string errmsg(int code);

		friend class hlink::HTTPRequestContext;


	private:
		std::string root;


	};
}

#endif

