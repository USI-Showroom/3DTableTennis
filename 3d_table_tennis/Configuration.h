#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "Defines.h"

using namespace std;

class Configuration{

private:
	string fileName;
	map<string, double> conf;

	Configuration(){
		fileName = Path::projectRoot()+"\\conf.ini";
		ifstream file;
		file.open(fileName.c_str(), ios::in);

		if(!file.fail()){

			string line = "";
	
			while(!file.eof()){
				file >> line;

				// ignore comments and sections lines
				if(line.at(0) == ';' || line.at(0) == '[')
					continue;

				size_t found;
				found = line.find_first_of(';');

				// ignore comments on other lines
				if(found != string::npos){
					line = line.substr(0, found);
				}

				found = line.find_first_of('=');

				string valueName = line.substr(0, found);
				double value = atof(line.substr(found+1).c_str());

				conf[valueName] = value;

			}

			file.close();

		} else 
			cout << "Error Opening File." << endl;
	}

	~Configuration(){
	}


public:

	static Configuration* getInstance();
	double get(const string valueName);
	void set(const string ValueName, const double value);
	void printAll();

};

#endif
