class listener
{
private:
	
	listener(int list_socket);
	void execute();
	static void* listener_proc(void* data);
	int _list_socket;

public:

	static int start(int port, int back_log);

};
