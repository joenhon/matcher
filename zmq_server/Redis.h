#pragma once
#include "hiredis.h"
#include <iostream>
#include <string>
#include <process.h>
#include <stdio.h>
#include "Config_.h"
using std::string;
class Redis
{
public:
	redisContext* c = NULL;
	redisReply* r = NULL;
	string url;
	int port;
	Redis();
	void setRedis(string key,string val);
	int getcount(string key);
	string getRedis(string key);
	void open();
	void close();
	~Redis();
};

