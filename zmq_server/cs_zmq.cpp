#include "cs_zmq.h"



cs_zmq::cs_zmq()
{
	Config_ configSettings("config.txt");
	string str;
	str = configSettings.Read("cs_zmq.url", str);
	zmq_connect(z_socket,str.c_str());
}


cs_zmq::~cs_zmq()
{
	zmq_msg_close(&send_msg);
}
