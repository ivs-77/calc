#include "config.h"
#include "log.h"
#include "cmd_handler.h"
#include "listener.h"

io_service listener::service;

int listener::start()
{
	try
	{
		ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), config::get_port()));
		log::log_info("Listener started. Accepting connections");
		while(true)
		{
			socket_ptr socket(new ip::tcp::socket(service));
			acceptor.accept(*socket);
			cmd_handler::start(std::move(socket));
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




