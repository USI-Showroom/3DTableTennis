#include "Configuration.h"

Configuration* Configuration::getInstance(){
	static Configuration theInstance;
	return &theInstance;
}

double Configuration::get(const string valueName){
	return conf[valueName];
}

void Configuration::set(const string ValueName, const double value){
	conf[ValueName] = value;
}

void Configuration::printAll(){
	map<string, double>::iterator it;
	for(it=conf.begin(); it != conf.end(); it++){
		cout << ((*it).first) << " => " << ((*it).second) << endl;
	}
}
