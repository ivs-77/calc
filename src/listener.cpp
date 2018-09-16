#include <boost/asio.hpp>
#include "config.h"
#include "log.h"
#include "cmd_handler.h"
#include "listener.h"

using namespace boost::asio;

int listener::start()
{
	try
	{
		io_service service;
		ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), config::get_port()));
		log::log_info("Listener started. Accepting connections");
		while(true)
		{
			std::unique_ptr<cmd_handler> handler(new cmd_handler());
			acceptor.accept(handler->get_socket());
			cmd_handler::start(std::move(handler));
		}
	}
	catch(const std::runtime_error& error)
	{
		log::log_error("Listener error: %s", error.what());
		return -1;
	}
		
	// This point is never reached.
	// Process is exited by cmd_handler when it accepts stop_key command
	return 0;
}




