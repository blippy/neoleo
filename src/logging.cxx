module;

export module logging;
import std;

namespace fs = std::filesystem;




//using std::cout;
//using std::endl;
//using std::ofstream;

class Log 
{
	public:
		Log();
		void debug(std::string s);
		~Log();
	private:
		std::ofstream m_ofs;
};

Log m_log;

Log::Log()
{
	auto dir = std::string(std::getenv("HOME")) + "/.neoleo";
	fs::create_directories(dir);
	auto logname = dir + "/log.txt";
	m_ofs.open(logname, std::ofstream::out | std::ofstream::app);
}

void log_debug(const std::string& s)
{
	m_log.debug(s);
}

void log_debug(const char* s)
{
	m_log.debug(s);
}


void Log::debug(std::string s)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	m_ofs << std::put_time(&tm, "%Y-%m-%d %H:%M:%S ");
	m_ofs << s << std::endl;

}

Log::~Log()
{
	if(m_ofs.is_open())
		m_ofs.close();
}


export template<typename... Args>
void log(Args ... args) {
	std::ostringstream ss;
	(ss << ... << args);
	log_debug(ss.str());
}
