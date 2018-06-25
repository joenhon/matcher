#pragma once
#include <iostream>
#include <process.h>
#include <thread> 
#include <stdio.h>
#include "zmq.h"
#include "zmq_utils.h"
#pragma comment(lib,"libzmq-v120-mt-4_0_4.lib")
#pragma comment(lib,"json_vc71_libmtd.lib")
#include "json/json.h"
#include "O.h"
#include "TradeCode.h"
#include "amqp_tcp_socket.h"  
#include "amqp.h"  
#include "amqp_framing.h" 
#include "windows.h"
#include "OrderTradeThread.h"
class RemoveOrderTradeThread
{
public:
	RemoveOrderTradeThread();
	~RemoveOrderTradeThread();
	void remakeOrder();
};

