#include <iostream>
#include <process.h>
#include <thread> 
#include <stdio.h>
#include <time.h>  
#include <math.h> 
#include "zmq.h"
#include "zmq_utils.h"
#pragma comment(lib,"libzmq-v120-mt-4_0_4.lib")
#pragma comment(lib,"lib/json_vc71_libmt.lib")
#include "json/json.h"
#include "O.h"
#include "TradeCode.h"
#include "amqp_tcp_socket.h"  
#include "amqp.h"  
#include "amqp_framing.h" 
#include "OrderTradeThread.h"
#include "RemoveOrderTradeThread.h"

using boost::multiprecision::cpp_dec_float_50;
using namespace Json;
using namespace std;

static TradeCode TC;
void addOrder();
void addOrder();
void Syn_RMQ();
void SynchronousQueue();
void sellRevoke();
void buyRevoke();
void remakeOrder();
//void PrintfNode(O o);
OrderTradeThread OTT;
RemoveOrderTradeThread ROTT;
Config_ configSettings("config.txt");
enum Type
{
	nullValue = 0, ///< 'null' value  
	intValue,      ///< signed integer value  
	uintValue,     ///< unsigned integer value  
	realValue,     ///< double value  
	stringValue,   ///< UTF-8 string value  
	booleanValue,  ///< bool value  
	arrayValue,    ///< array value (ordered list)  
	objectValue    ///< object value (collection of name/value pairs).  
};
int main()
{
	//Redis* red((redis.getObj()).get());
	//Redis redis_ = *red;
	system("pause");
	cout << "" << endl;
	InitializeCriticalSection(&sellQuene_CS);
	InitializeCriticalSection(&buyQuene_CS);
	InitializeCriticalSection(&removeSellQuene_CS);
	InitializeCriticalSection(&removeBuyQuene_CS);
	InitializeCriticalSection(&RabbitMQ_Send_CS);
	InitializeCriticalSection(&buyQuene_CS_a1);
	InitializeCriticalSection(&sellQuene_CS_a1);
	InitializeCriticalSection(&RMQ_Quene_CS);
	ToJson<BuyQuene> toJson_buyQuene;
	ToJson<SellQuene> toJson_sellQuene;
	ToJson<BuyQuene> toJson_removeBuyQuene;
	ToJson<SellQuene> toJson_removeSellQuene;
	//启动撮合是读取Redis数据（暂定）
	/*
	redis_.open();
	EnterCriticalSection(&sellQuene_CS);
	EnterCriticalSection(&buyQuene_CS);
	EnterCriticalSection(&removeSellQuene_CS);
	EnterCriticalSection(&removeBuyQuene_CS);

	removeBuyQuene = toJson_removeBuyQuene.JsonObject(redis_.getRedis("removeBuyQuene"));//将数据从redis读出
	toJson_removeBuyQuene.~ToJson();

	redis_.open();
	removeSellQuene = toJson_removeSellQuene.JsonObject(redis_.getRedis("removeSellQuene"));
	toJson_removeSellQuene.~ToJson();

	redis_.open();
	buyQuene = toJson_buyQuene.JsonObject(redis_.getRedis("buyQuene"));
	toJson_buyQuene.~ToJson();

	redis_.open();
	sellQuene = toJson_sellQuene.JsonObject(redis_.getRedis("sellQuene"));
	toJson_sellQuene.~ToJson();

	LeaveCriticalSection(&sellQuene_CS);
	LeaveCriticalSection(&buyQuene_CS);
	LeaveCriticalSection(&removeSellQuene_CS);
	LeaveCriticalSection(&removeBuyQuene_CS);
	*/
	thread addOrder(addOrder), remakeOrder(remakeOrder), sellRevokeThreda(sellRevoke), buyRevokeThreda(buyRevoke), SynchronousQueue(SynchronousQueue),SynchronousRMQ_Queue(Syn_RMQ);
	//HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, addOrder, 0, 0, NULL);
	//核心（撮合）
	while (true)
	{
		TC.core();
		//Sleep(1);
	}
	//while (!buyQuene.empty()) {
		//OTT.PrintfNode(buyQuene.top());
		//buyQuene.pop();
	//}
	//WaitForSingleObject(hThread, INFINITE);
	return 1;
}
/*
挂单队列线程
*/
void addOrder() 
{
	OTT.addOrder();
}
/*
撤单队列线程
*/
void  remakeOrder()
{
	ROTT.remakeOrder();
}
/*
定时发送完成订单
*/
void Syn_RMQ() 
{
	int cs = 0;
	int synTime,number;
	synTime=configSettings.Read("RMQ.synTime", synTime);
	number= configSettings.Read("RMQ.number", synTime);
	while (true)
	{
		if (RMQ_Quene.size()>0)
		{
			cs++;
			EnterCriticalSection(&RMQ_Quene_CS);
			rmq.send(RMQ_Quene.front());
			RMQ_Quene.pop();
			LeaveCriticalSection(&RMQ_Quene_CS);
		}
		if (cs>number)
		{
			cs = 0;
			Sleep(synTime);
			cout << RMQ_Quene.size() << endl;
		}
		
	}
}
/*
同步队列
*/
void SynchronousQueue()
{
	int number = 10, old = 10;
	int synTime;
	synTime = configSettings.Read("Queue.synTime", synTime);
	while (true) {
		if (number==0)
		{
			number = 10;
			old = 10;
		}
		int syn = ceil((number / old + 1)*synTime);
		Sleep(syn);
		old = syn;
		//执行同步
		//开始线程锁
		EnterCriticalSection(&sellQuene_CS_a1);
		EnterCriticalSection(&sellQuene_CS);
		number = sellQuene_a1.size();
		while (sellQuene_a1.empty()==0)
		{
			sellQuene.push(sellQuene_a1.front());
			sellQuene_a1.pop();
		}
		//关闭线程锁
		LeaveCriticalSection(&sellQuene_CS_a1);
		LeaveCriticalSection(&sellQuene_CS);

		EnterCriticalSection(&buyQuene_CS_a1);
		EnterCriticalSection(&buyQuene_CS);
		number+= buyQuene_a1.size();
		while (buyQuene_a1.empty() == 0)
		{
			buyQuene.push(buyQuene_a1.front());
			buyQuene_a1.pop();
		}
		LeaveCriticalSection(&buyQuene_CS_a1);
		LeaveCriticalSection(&buyQuene_CS);
	}
}
/*
sell撤单核心方法
*/
void sellRevoke()
{
	while (true)
	{

		if (!removeSellQuene.empty())
		{
			if (removeSellQuene.size() < 5)
			{
				Sleep(10);
			}
			EnterCriticalSection(&sellQuene_CS);
			TC.remaveSell(sellQuene);//调用撤卖单方法
			LeaveCriticalSection(&sellQuene_CS);
		}
		else
		{
			Sleep(1000);
		}
	}
}
/*
buy撤单核心方法
*/
void buyRevoke()
{
	while (true)
	{

		if (!removeBuyQuene.empty())
		{
			if (removeBuyQuene.size() < 5)
			{
				Sleep(10);
			}
			EnterCriticalSection(&buyQuene_CS);
			TC.remaveBuy(buyQuene);//调用撤买单方法
			LeaveCriticalSection(&buyQuene_CS);
		}
		else
		{
			//buyRevokeThreda.~thread
			Sleep(1000);
		}

	}
}