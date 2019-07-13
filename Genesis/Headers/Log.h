
#ifndef log_h
#define log_h

#include "ConsoleCommand.h"

#include <clocale>
#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <ctime>
#include <stdint.h>
using namespace std;

void GeneralLog(string mas)
{
	static const uint32_t SIZE_LOG_FILE = 50 * 1024 * 1024;

	static mutex mtx;
	mtx.lock();

	auto s = time(0);
	tm *t = localtime(&s);
	char time[22];
	strftime(time, 22, "[%d.%m.%Y %H:%M:%S]", t);
	string CurrentTime = time;

	static string startTime = string(time).erase(11) + "]";
	system((mkdir_r + "Log/" + startTime).c_str());
	
	static uint16_t file = 0;
	static fstream logFile("Log/" + startTime + "/0.log", ios::out | ios::app);
	
	if (logFile.tellg() >= SIZE_LOG_FILE)
	{
		file++;
		logFile.close();
		logFile.open("Log/" + startTime + "/" + to_string(file) + ".log", ios::out | ios::app);
	}
	logFile << CurrentTime << " " << mas << endl;

	mtx.unlock();
}
#endif
