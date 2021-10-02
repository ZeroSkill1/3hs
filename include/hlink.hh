
#ifndef inc_hlink_hh
#define inc_hlink_hh

#include <functional>
#include <string>

#include <stddef.h>
#include <stdint.h>


namespace hlink
{
	constexpr char transaction_magic[] = "HLT";
	constexpr size_t transaction_magic_len = 3;
	constexpr int poll_timeout = 1000;
	constexpr int max_timeouts = 3;
	constexpr int port = 37283;
	constexpr int backlog = 2;

	enum class action : uint8_t
	{
		add_queue    = 0,
		install_id   = 1,
		install_url  = 2,
		install_data = 3,
		nothing      = 4,
		launch       = 5,
		sleep        = 6,
	};

	enum class response : uint8_t
	{
		accept       = 0,
		busy         = 1,
		untrusted    = 2,
		error        = 3,
		success      = 4,
		notfound     = 5,
	};

	void create_server(
		std::function<bool(const std::string&)> on_requester,
		std::function<void(const std::string&)> disp_error,
		std::function<void(const std::string&)> on_server_create,
		std::function<bool()> on_poll_exit,
		std::function<void(const std::string&)> disp_req
	);
}

#endif

