#include "Redis.h"


Redis::Redis()
{
	
}

void Redis::setRedis(string key, string val) {
	
	if (c->err) {
		redisFree(c);
		return;
	}
	string cm = ("set " + key + " " + val);
	//std::cout << "执行向redis添加数据  " << cm<<std::endl;
	const char* command1 =cm.c_str();
	r = (redisReply*)redisCommand(c, command1);
	if (NULL == r) {
		redisFree(c);
		return;
	}
	if (!(r->type == REDIS_REPLY_STATUS && (strcmp(r->str, "OK")) == 0 || strcmp(r->str, "ok") == 0)) {
		std::cout << "未能执行命令" << command1 << std::endl;
		freeReplyObject(r);
		redisFree(c);
		return;
	}
	freeReplyObject(r);
	//std::cout << "成功执行新增 " << key << std::endl;

}

int Redis::getcount(string key)
{

	const char* command2 = ("get " + key).c_str();
	r = (redisReply*)redisCommand(c, command2);
	if (r->type != REDIS_REPLY_INTEGER) {
		std::cout << "未能执行命令" << key << std::endl;
		freeReplyObject(r);
		redisFree(c);
		return 0;
	}
	int length = r->integer;
	freeReplyObject(r);
	//std::cout << "值是 " << length << std::endl;
	//std::cout << "成功执行命令 " << key << std::endl;
	return length;

}

string Redis::getRedis(string key)
{
	string str;
	//std::cout << " key:" << key << std::endl;
	try
	{
		string cm = ("get " + key);
		const char* command3 = cm.c_str();
		r = (redisReply*)redisCommand(c, command3);
		if (r->type != REDIS_REPLY_STRING) {
			std::cout << "未能执行命令 " << key << std::endl;
			freeReplyObject(r);
			redisFree(c);
			return "";
		}
		str = r->str;
		freeReplyObject(r);
		std::cout << "成功执行查询 " << key << std::endl;
	}
	catch (const std::exception&)
	{
		return "";
	}

	return  str;
}


void Redis::open()
{
	Config_ configSettings("config.txt");
	url = configSettings.Read("Redis.url", url);
	port = configSettings.Read("Redis.port", port);

	c = redisConnect(url.c_str(), port);
	if (c->err) {
		redisFree(c);
		return;
	}
}

void Redis::close()
{
	/*freeReplyObject(r);
	redisFree(c);*/
}


Redis::~Redis()
{
	
}
