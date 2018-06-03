
class log
{
public:
	static void log_errno(const char* message);
	static void log_error(const char* message, ...);
	static void log_console(const char* message, ...);
private:
	log();
	static char error_log_file_name[20];
	static log instance;
};