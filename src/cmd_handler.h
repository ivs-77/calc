
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
	~cmd_handler();
	void execute();
	static void* handler_proc(void* data);
	int _connfd;
	char* _command_buf;
	
	void hello();
	int wait();
	int read_command();
	handler_state login();
	handler_state handle_command();

public:
	
	static void start(int connfd);
};