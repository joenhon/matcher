#pragma once
#include <iostream>
#include <process.h>
#include <thread> 
#include <stdio.h>
#include <zmq.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
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
#include "Redis.h"
#include "ToJson.h"
#include "Config_.h"
using std::shared_ptr;
class OrderTradeThread
{
public:
	friend class boost::serialization::access;
	OrderTradeThread();
	~OrderTradeThread();
	bool eq(O o1, O o2);
	void addOrder();
	O toOrder(Json::Value root);
};

