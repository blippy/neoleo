#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "logging.h"


using std::cout;
using std::endl;
using std::ofstream;

class Log 
{
	public:
		Log();
		void debug(std::string s);
		~Log();
	private:
		ofstream m_ofs;
};

Log m_log;

Log::Log()
{
	//if(!m_ofs.is_open()) {
		(void) system("mkdir -p $HOME/.neoleo/logs");
		std::string logname = std::string(getenv("HOME")) 
			+ "/.neoleo/logs/log.txt";
		m_ofs.open(logname, std::ofstream::out | std::ofstream::app);
	//}
}

void 
log_debug(const char* s)
{
	m_log.debug(s);
}

void
Log::debug(std::string s)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	m_ofs << std::put_time(&tm, "%Y-%m-%d %H:%M:%S ");
	m_ofs << s << endl;

	//auto now(std::chrono::system_clock::now());
	//m_ofs << now;

}

Log::~Log()
{
	if(m_ofs.is_open())
		m_ofs.close();
}
