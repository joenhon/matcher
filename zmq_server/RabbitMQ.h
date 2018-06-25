#pragma once
#include <iostream>
#include <process.h>
#include <stdio.h>

#include "O.h"
#include <queue>
#include "Tx.h"
#include "json/json.h"
#include "amqp_tcp_socket.h"  
#include "amqp.h"  
#include "amqp_framing.h" 
#include "Config_.h"
//#include "windows.h"与RMQ冲突
class RabbitMQ
{
public:
	int status;
	//char  *messagebody;
	int sockfd;
	string bindingkey; /* 绑定的队列 */
	string hostname;//IP地址
	int port;//端口
	amqp_socket_t *socket_qmqp = NULL;
	amqp_bytes_t message_bytes;
	Json::Value new_item;
	string new_root;
	RabbitMQ();
	~RabbitMQ();
	bool send(Tx tx);
	string TxtoJSON(Tx tx);
	void open(amqp_connection_state_t& conn);
	void login(amqp_connection_state_t& conn);
	void close(amqp_connection_state_t& conn);
};

