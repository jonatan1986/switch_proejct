/*
 * debug.h
 *
 *  Created on: 10 May 2020
 *      Author: yonathan
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#define debugFile "debug.txt"
#include<fstream>

class debug
{
public:
	debug(){
		ifstream file;
		string line;
		file.open(debugFile);\
		assert(file);
		getline(file,line);
		m_bDebug = line == "yes" ? true : false;
	}
	const bool IsDebug(){ return m_bDebug;}
private:
	bool m_bDebug;

};


#endif /* DEBUG_H_ */

