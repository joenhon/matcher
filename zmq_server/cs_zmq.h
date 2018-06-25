#pragma once
#include <fstream>
#include <process.h>
#include <iostream>
#include "Config_.h"
#include "zmq.h"
#include "zmq_utils.h"

using std::string;
class cs_zmq
{	
	void* context = zmq_init(1);    //指定zmq 处理I/0事件的thread pool 为1
	void* z_socket = zmq_socket(context, ZMQ_PUSH);
	zmq_msg_t send_msg;
	
public:
	
	void fs() {
		
		
		zmq_msg_init_size(&send_msg, 6);
		string s = "1";
		string* ss = &s;
		memcpy(zmq_msg_data(&send_msg), ss, 1);
		zmq_msg_send(&send_msg, z_socket, 0);
		
	}
	cs_zmq();
	~cs_zmq();
};

