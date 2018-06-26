#pragma once
#include <iostream>
#include <process.h>
#include <thread> 
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include <boost\lexical_cast.hpp>
#include "O.h"
#include <queue>
#include "Tx.h"
#include "json/json.h"
#include "amqp_tcp_socket.h"  
#include "amqp.h"  
#include "amqp_framing.h" 
//#include "windows.h"与RMQ冲突
#include "OrderTradeThread.h"
#include "RabbitMQ.h"
#include "ObjPool.h"
#include "Redis.h"
#include "cs_zmq.h"

using namespace std;
 struct BuyQuene
{
	 bool operator () ( O &o1,  O &o2)const {
		cpp_dec_float_50 bol = o2.getPrice().compare(o1.getPrice());
		if (bol>0)
		{
			return true;
		}
		else if (bol == 0)
		{
			return o1.getTime() < o2.getTime();
		}

		return  false;
	}
};
 struct SellQuene
{
	 bool operator () ( O &o1,  O &o2)const {
		cpp_dec_float_50 bol = o1.getPrice().compare(o2.getPrice());
		if (bol>0)
		{
			return true;
		}
		else if (bol == 0)
		{
			return o1.getTime() < o2.getTime();
		}

		return  false;
	}
};
 //购买优先级队列
 extern priority_queue<O, vector<O>, BuyQuene> buyQuene;
 //出售优先级队列
 extern priority_queue<O, vector<O>, SellQuene> sellQuene;
 //购买撤单优先级队列
 extern priority_queue<O, vector<O>, BuyQuene> removeBuyQuene;
 //出售撤单优先级队列
 extern priority_queue<O, vector<O>, SellQuene> removeSellQuene;

 /*定时同步Quene与Quene_a1*/
 //由定时控制的优先级队列/*定时，将网络繁忙度通过算法的出下一次同步的间隔*/
 extern priority_queue<O, vector<O>, BuyQuene> buyQuene_a1;
 extern priority_queue<O, vector<O>, SellQuene> sellQuene_a1;
 //线程锁，调用需同步的方法或参数执行，阻塞其他的调用，直到调用的方法完成调用
 extern CRITICAL_SECTION buyQuene_CS,sellQuene_CS, removeBuyQuene_CS, removeSellQuene_CS, RabbitMQ_Send_CS, buyQuene_CS_a1, sellQuene_CS_a1;

 extern ObjPool<RabbitMQ> RMQ;
 extern ObjPool<Redis> redis;
 static cs_zmq c;


 class TradeCode
{
public:
	TradeCode();
	~TradeCode();
	
	
	RabbitMQ RMQ;
	/**
	* 交易核心方法（撮合买卖单）
	*/
	static void core();
	static bool eq(O &o1, O &o2);
	static void remaveSell(priority_queue<O, vector<O>, SellQuene>& p);
	static void remaveBuy(priority_queue<O, vector<O>, BuyQuene>& p);
	static void delete_BuyQueue();
	static void delete_SellQueue();
	static O top_SellQueue();
	static O top_BuyQueue();
	static void delete_RemoveBuyQueue();
	static void delete_RemoveSellQueue();
	static O top_RemoveSellQueue();
	static O top_RemoveBuyQueue();
	static void Redis_();
};

