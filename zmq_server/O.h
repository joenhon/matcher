#pragma once
using namespace std;
#include <string>
#include <boost/math/constants/constants.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
using boost::multiprecision::cpp_dec_float_50;
using boost::long_long_type;
class O
{
public:
	
	O();
	~O();
	/**
	* 订单ID
	*/
	void setOrderId(string orderId) 
	{
		O::orderId = orderId;
	}
	/**
	* 订单类型（buy/sell）
	*/
	void setOrderType(string orderType) 
	{
		O::orderType = orderType;
	}
	/**
	* 订单价格
	*/
	void setPrice(cpp_dec_float_50  price) 
	{
		O::price = price;
	}
	/**
	* 订单数量
	*/
	void setQuantity(cpp_dec_float_50 quantity) 
	{
		O::quantity = quantity;
	}
	/**
	* 交易通道（货币对）
	*/
	void setSymbol(string symbol)
	{
		O::symbol = symbol;
	}
	/**
	* 冻结数量（buy冻结法币/sell冻结出售的币）
	*/
	void setFreezeAmt(cpp_dec_float_50 freezeAmt)
	{
		O::freezeAmt = freezeAmt;
	}
	/**
	* 用户ID
	*/
	void setUserUuid(string userUuid)
	{
		O::userUuid = userUuid;
	}
	/**
	* 交易类型（市价/限价）
	*/
	void setTradeType(string tradeType)
	{
		O::tradeType = tradeType;
	}
	/**
	* 取消标志（0:正常、1:取消）
	*/
	void setIsCancel(int isCancel)
	{
		O::isCancel = isCancel;
	}
	/**
	* 订单发送时间
	*/
	void setTime(string time)
	{
		O::time = time;
	}
	/**
	* 订单ID
	*/
	string getOrderId() 
	{
		return orderId;
	}
	/**
	* 订单类型（buy/sell）
	*/
	string getOrderType() 
	{
		return orderType;
	}
	/**
	* 订单价格
	*/
	cpp_dec_float_50  getPrice() const
	{
		return price;
	}
	/**
	* 订单数量
	*/
	cpp_dec_float_50 getQantity() 
	{
		return quantity;

	}
	/**
	* 交易通道（货币对）
	*/
	string getSymbol() 
	{
		return symbol;
	}
	/**
	* 冻结数量（buy冻结法币/sell冻结出售的币）
	*/
	cpp_dec_float_50 getFreezeAmt() 
	{
		return freezeAmt;
	}
	/**
	* 用户ID
	*/
	string getUserUuid() 
	{
		return userUuid;
	}
	/**
	* 交易类型（市价/限价）
	*/
	string getTradeType() 
	{
		return tradeType;
	}
	/**
	* 取消标志（0:正常、1:取消）
	*/
	int getIsCancel() 
	{
		return isCancel;
	}
	/**
	* 订单发送时间
	*/
	string getTime() const
	{
		return time;
	}
private:
	/**
	* 订单ID
	*/
	string orderId;
	/**
	* 订单类型（buy/sell）
	*/
	string orderType;
	/**
	* 订单价格
	*/
	cpp_dec_float_50  price;
	/**
	* 订单数量
	*/
	cpp_dec_float_50 quantity;
	/**
	* 交易通道（货币对）
	*/
	string symbol;
	/**
	* 冻结数量（buy冻结法币/sell冻结出售的币）
	*/
	cpp_dec_float_50 freezeAmt;
	/**
	* 用户ID
	*/
	string userUuid;
	/**
	* 交易类型（市价/限价）
	*/
	string tradeType;
	/**
	* 取消标志（0:正常、1:取消）
	*/
	int isCancel;
	/**
	* 订单发送时间
	*/
	string time;
	
};

