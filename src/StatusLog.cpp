#include "StatusLog.h"
#include <iostream>

string StatusLog::getCurrentStatus(const int statusType){
	if(logMap[statusType].empty()){
		return "";
	} else {
		return logMap[statusType].top();
	}
}

void StatusLog::addToLog(const int statusType, const string status){
	logMap[statusType].push(status);

	if(statusType == SKELETON_LOG)
		std::cout << status << std::endl;
}

StatusLog* StatusLog::getInstance(){
	static StatusLog theInstance;
	return &theInstance;
}
