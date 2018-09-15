#include <boost/asio.hpp>

using namespace boost::asio;

class listener
{
private:

	static io_service service;

public:

	static int start();

};
