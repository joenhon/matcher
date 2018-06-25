#include "RemoveOrderTradeThread.h"



RemoveOrderTradeThread::RemoveOrderTradeThread()
{
}


RemoveOrderTradeThread::~RemoveOrderTradeThread()
{
}

void RemoveOrderTradeThread::remakeOrder()
{
	Config_ configSettings("config.txt");
	string url;
	url = configSettings.Read("remakeOrder.url", url);

	OrderTradeThread OTT;

	Json::Reader readerinfo;
	Json::Value root;
	std::cout << "-------------------remakeOrder线程启动-----------------------" << std::endl;

	//指定zmq 处理I/0事件的thread pool 为1
	void* context = zmq_init(1);
	//连接类型
	void* z_socket = zmq_socket(context, ZMQ_PULL);

	Redis* red((redis.getObj()).get());
	Redis redis_ = *red;

	zmq_bind(z_socket, url.c_str());    // accept connections on a socket
												   //订单
	O order;
	int recvn = 1;
	//循环工作
	while (1)
	{
		//接受部分
		zmq_msg_t recv_msg;
		zmq_msg_init(&recv_msg);
		zmq_msg_recv(&recv_msg, z_socket, 0);//0表示非阻塞
											 //发送部分
		zmq_msg_t send_msg;
		string strData = (char*)zmq_msg_data(&recv_msg);
		if (!strData.empty())
		{
			if (!readerinfo.parse(strData, root))
			{
				cout << (TEXT("json error!")) << endl;
			}
			if (!root["orderId"].isNull())
			{
				try
				{
					order = OTT.toOrder(root);
					if (order.getOrderType().compare("buy") == 0)
					{
						EnterCriticalSection(&removeBuyQuene_CS);
						removeBuyQuene.push(order);//向撤买单队列添加
						LeaveCriticalSection(&removeBuyQuene_CS);

						ToJson<BuyQuene> toJson;
						redis_.open();
						redis_.setRedis("removeBuyQuene", toJson.OToJson(removeBuyQuene));
						toJson.~ToJson();
					}
					else
					{
						EnterCriticalSection(&removeSellQuene_CS);
						removeSellQuene.push(order);//向撤卖单队列添加
						LeaveCriticalSection(&removeSellQuene_CS);

						ToJson<SellQuene> toJson;
						redis_.open();
						redis_.setRedis("removeSellQuene", toJson.OToJson(removeSellQuene));
						toJson.~ToJson();
					}
					//zmq_msg_init_size(&send_msg, 2);
					//memcpy(zmq_msg_data(&send_msg), "ok", 2);
					//zmq_sendmsg(z_socket, &send_msg, 0);
					//zmq_msg_close(&send_msg);
				}
				catch (const std::exception&)
				{
					//zmq_msg_init_size(&send_msg, 5);
					//memcpy(zmq_msg_data(&send_msg), "error", 5);
					//zmq_sendmsg(z_socket, &send_msg, 0);
					//zmq_msg_close(&send_msg);
					cout << "解析json错误：" << root << endl;
				}

			}
			else
			{
				//zmq_msg_init_size(&send_msg, 5);
				//memcpy(zmq_msg_data(&send_msg), "error", 5);
				//zmq_sendmsg(z_socket, &send_msg, 0);
				//zmq_msg_close(&send_msg);
			}
		}
		//std::cout << "第\t" << recvn++ << "\t次收到client信息：\t";
		//std::cout << (char*)zmq_msg_data(&recv_msg) << std::endl;
		zmq_msg_close(&recv_msg);

	}
	zmq_close(z_socket);
	zmq_term(context);
}
