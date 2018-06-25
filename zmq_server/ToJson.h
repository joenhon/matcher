#pragma once
#include "O.h"
#include <iostream>
#include <queue>
#include "json/json.h"
#include <boost/property_tree/json_parser.hpp>
using namespace std;
using namespace boost::property_tree;
template <typename T>
class ToJson
{
	ptree OtoJSON(O o) {
		ptree root;
		root.put("orderId", o.getOrderId());
		root.put("orderType", o.getOrderType());
		root.put("freezeAmt", o.getFreezeAmt());
		root.put("cancel", o.getIsCancel());
		root.put("price", o.getPrice());
		root.put("quantity", o.getQantity());
		root.put("symbol", o.getSymbol());
		root.put("time", o.getTime());
		root.put("tradeType", o.getTradeType());
		root.put("userUuid", o.getUserUuid());
		return root;
	}

	O OJson(Json::Value root)
	{
		O order;
		if (!root["orderId"].isNull())
		{
			string orId = root["orderId"].asString();
			order.setOrderId(orId);

		}
		if (!root["orderType"].isNull())
		{
			order.setOrderType(root["orderType"].asCString());

		}
		if (!root["freezeAmt"].isNull())
		{
			cpp_dec_float_50 freezeAmt = cpp_dec_float_50(root["freezeAmt"].asString());
			order.setFreezeAmt(freezeAmt);

		}
		if (!root["cancel"].isNull())
		{
			order.setIsCancel(boost::lexical_cast<int>(root["cancel"].asString()));

		}
		if (!root["price"].isNull())
		{
			cpp_dec_float_50 price = cpp_dec_float_50(root["price"].asString());
			order.setPrice(price);

		}
		if (!root["quantity"].isNull())
		{
			cpp_dec_float_50 quantity = cpp_dec_float_50(root["quantity"].asString());
			order.setQuantity(quantity);

		}
		if (!root["symbol"].isNull())
		{
			order.setSymbol(root["symbol"].asCString());

		}
		if (!root["time"].isNull())
		{
			try
			{
				order.setTime(root["time"].asCString());
			}
			catch (const std::exception&)
			{
				order.setTime("");
			}


		}
		if (!root["tradeType"].isNull())
		{
			order.setTradeType(root["tradeType"].asCString());

		}
		if (!root["userUuid"].isNull())
		{
			order.setUserUuid(root["userUuid"].asCString());

		}

		return order;
	}

public:
	ToJson() {}
	
	string OToJson(priority_queue<O, vector<O>, T> queue) {
		ptree arr , queue_redis;
		while (!queue.empty()) {
			arr.push_back(make_pair("", OtoJSON(queue.top())));
			queue.pop();
		}
		queue_redis.put_child("array", arr);
		stringstream s;
		write_json(s, queue_redis, false);
		return s.str();
	}

	priority_queue<O, vector<O>, T> JsonObject(string json)
	{
 		Json::Reader readerinfo;
		Json::Value root,root_;
		if (!readerinfo.parse(json, root))
		{
			cout << (TEXT("json error!")) << endl;
		}
		priority_queue<O, vector<O>, T> queue;
		try
		{
			if (!root["array"].isNull())
			{
				root_ = root["array"];
				if (root_["orderId"].isNull())
				{
					for (int j = 0; j < root_.size(); j++)
					{
						O order = OJson(root_[j]);
						queue.push(order);

					}
				}
				else
				{
					O order = OJson(root_);
					queue.push(order);
				}

			}
		}
		catch (const std::exception&)
		{
			root_ = root["array"];
			for (int j = 0; j < root_.size(); j++)
			{
				O order = OJson(root_[j]);
				queue.push(order);

			}
		}
		
		
		return queue;
		
	}
	
	~ToJson(){}
	
};

