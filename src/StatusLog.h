#ifndef __STATUS_LOG_CUST_H
#define __STATUS_LOG_CUST_H

#include <string>
#include <stack>
#include <map>

#define SKELETON_LOG 1
#define KINECT_LOG 2

using namespace std;

class StatusLog {

	map<int, stack<string>> logMap;

	StatusLog(){
	}
	~StatusLog(){
	}

public:

	string getCurrentStatus(const int statusType);
	void addToLog(const int statusType, const string status);
	StatusLog* getInstance();

};

#endif
