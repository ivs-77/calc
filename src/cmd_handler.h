

class cmd_handler
{
private:
	
	cmd_handler(int connfd);
	void execute();
	static void* handler_proc(void* data);
	int _connfd;

public:
	
	static void start(int connfd);
};