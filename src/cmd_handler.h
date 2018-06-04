
enum handler_state
{
	initial,
	handling,
	exit
};

class cmd_handler
{
private:
	
	cmd_handler(int connfd);
	void execute();
	static void* handler_proc(void* data);
	int _connfd;
	
	handler_state login();
	handler_state handle_command();

public:
	
	static void start(int connfd);
};