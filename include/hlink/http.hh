
#ifndef inc_hlink_http_hh
#define inc_hlink_http_hh

#include <arpa/inet.h>

#include <string>
#include <map>


namespace hlink
{
	using HTTPParameters = std::map<std::string, std::string>;
	using HTTPHeaders    = std::map<std::string, std::string>;

	class HTTPServer; /* forward decl */
	struct HTTPRequestContext
	{
		HTTPServer *server;
		HTTPHeaders headers; /* headers are always lowercased */
		std::string method; /* method is always lowercased */
		struct sockaddr_in clientaddr;
		HTTPParameters params;
		std::string path;
		char buf[4096];
		size_t buflen;
		bool iseof;
		int fd;

		enum serve_type
		{
			plain, templ, notfound
		};

		inline bool is_get() { return this->method == "get"; }
		void serve_file(int status, const std::string& fname, HTTPHeaders headers);
		void respond(int status, const std::string& data, HTTPHeaders headers);
		void respond_chunked(int status, HTTPHeaders headers);
		void respond(int status, const HTTPHeaders& headers);
		void redirect(const std::string& location);
		void send_chunk(const std::string& data);
		void send(const std::string& data);
		void serve_plain();
		serve_type type(); /* NOTE: Sets this->path on success */
		void close();

		void read_path_content(std::string& buf);

		void serve_400();
		void serve_403();
		void serve_404(const std::string& fname);
		void serve_404();
		void serve_500();
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

