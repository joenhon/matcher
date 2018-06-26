#include "TradeCode.h"
#include <thread> 
 priority_queue<O, vector<O>, BuyQuene> buyQuene;
 priority_queue<O, vector<O>, SellQuene> sellQuene;
 priority_queue<O, vector<O>, BuyQuene> removeBuyQuene;
 priority_queue<O, vector<O>, SellQuene> removeSellQuene;
 priority_queue<O, vector<O>, BuyQuene> buyQuene_a1;
 priority_queue<O, vector<O>, SellQuene> sellQuene_a1;
 CRITICAL_SECTION buyQuene_CS, sellQuene_CS, removeBuyQuene_CS, removeSellQuene_CS, RabbitMQ_Send_CS, buyQuene_CS_a1, sellQuene_CS_a1;
 ObjPool<RabbitMQ> RMQ;
 ObjPool<Redis> redis;

TradeCode::TradeCode()
{
	
}


TradeCode::~TradeCode()
{
}



O sellCopy, buyCopy;

void TradeCode::core()
{
		try
		{
			if (!sellQuene.empty()&& !buyQuene.empty())
			{	
				O sell = sellCopy = top_SellQueue();

				O buy = buyCopy = top_BuyQueue();	
				//delete_BuyQueue();
				//delete_SellQueue();
				//币种相等才进撮合
				if (buy.getSymbol().compare(sell.getSymbol()) != 0) {
					EnterCriticalSection(&buyQuene_CS);
					buyQuene.push(buy);
					LeaveCriticalSection(&buyQuene_CS);

					EnterCriticalSection(&sellQuene_CS);
					sellQuene.push(sell);
					LeaveCriticalSection(&sellQuene_CS);
					return;
				}
				if (!sell.getOrderId().empty() && sell.getOrderId().length()>0)
				{
					cpp_dec_float_50 sellPrice = sell.getPrice();
						
						if (!buy.getOrderId().empty() && buy.getOrderId().length()>0)
						{
							cpp_dec_float_50 buyPrice = buy.getPrice();
						
								if (sellPrice.compare(buyPrice) <= 0)
								{
								//冻结出售币数量
								cpp_dec_float_50 sellFreezeAmt = sell.getFreezeAmt();
								//冻结法币
								cpp_dec_float_50 buyFreezeAmt = buy.getFreezeAmt();
								//出售订单数量
								//cpp_dec_float_50 sellQuantity = sellFreezeAmt.operator-=(buyFreezeAmt);
								//cpp_dec_float_50 buyQuantity = buyFreezeAmt.operator-=(sellFreezeAmt);
								
								int r = buyFreezeAmt.compare(sellFreezeAmt);
								cpp_dec_float_50 dealPrice;
								cpp_dec_float_50 dealQuantity;
								cpp_dec_float_50 dealVolumePrice;
								cpp_dec_float_50 buyRemainAmt;
								cpp_dec_float_50 sellRemainAmt;
								string txType;
								//完全成交
								if (r == 0)
								{
									txType = "buy";
									dealPrice = sellPrice;
									dealQuantity = sellFreezeAmt;
									dealVolumePrice = sellRemainAmt;
									buyRemainAmt = sellRemainAmt.is_zero();
								}
								//卖单少了，买单多了
								else if (r > 0)
								{
									txType = "sell";
									dealPrice = sellPrice;
									dealQuantity = sellFreezeAmt;
									//cout << " dealQ  " << dealQuantity.str(100) << endl;
									dealVolumePrice = sellPrice.operator*=(dealQuantity);
									//dealVolumePrice.precision(8);
									sellRemainAmt.is_zero();
									buyRemainAmt = buyFreezeAmt.operator-=(sellFreezeAmt);
									//剩余买单重新丢回去
									//buy.setQuantity(buyRemainAmt);
									buy.setFreezeAmt(buyRemainAmt);
									EnterCriticalSection(&buyQuene_CS);
									buyQuene.push(buy);
									LeaveCriticalSection(&buyQuene_CS);
								}
								//卖单多了，买单少了
								else
								{
									txType = "buy";
									dealPrice = sellPrice;
									dealQuantity = buyFreezeAmt;
									dealVolumePrice = sellPrice.operator*=(dealQuantity);
									buyRemainAmt.is_zero();
									sellRemainAmt = sellFreezeAmt.operator-=(buyFreezeAmt);
									//sell.setQuantity(sellRemainAmt);
									sell.setFreezeAmt(sellRemainAmt);

									EnterCriticalSection(&sellQuene_CS);
									sellQuene.push(sell);
									LeaveCriticalSection(&sellQuene_CS);
								}
								Tx tx;
								tx.setBuyUserId(buy.getUserUuid());
								tx.setBuyOrderId(buy.getOrderId());
								tx.setBuyRemainAmt(buyRemainAmt);
								tx.setDealPrice(dealPrice);
								tx.setDealQuantity(dealQuantity);
								tx.setDealVolumePrice(dealVolumePrice);
								tx.setSellOrderId(sell.getOrderId());
								tx.setSellRemainAmt(sellRemainAmt);
								tx.setSellUserId(sell.getUserUuid());
								tx.setTxType(txType);
								try
								{
									//EnterCriticalSection(&RabbitMQ_Send_CS);
									/*RabbitMQ rmq;
									rmq.send(tx);
									rmq.~RabbitMQ();*/	

									//发送撮合完成订单信息
									c.fs();
									//LeaveCriticalSection(&RabbitMQ_Send_CS);
									/*将数据备份到Redis中（暂定）*/
									//Redis_();
									//在控制台中打印交易信息
									cout << txType << endl;
									//cout << "发送" << endl;
								}
								catch (const std::exception&)
								{
									cout << "撮合RMQ失败！" << endl;
								}
							
								//rmq.~RabbitMQ();
								//cout << new_item << endl;
							}
							}
							
						
					}else
					{
						EnterCriticalSection(&buyQuene_CS);
						buyQuene.push(buy);
						LeaveCriticalSection(&buyQuene_CS);

						EnterCriticalSection(&sellQuene_CS);
						sellQuene.push(sell);
						LeaveCriticalSection(&sellQuene_CS);
					}
				}
			}
			catch (const std::exception&)
			{
				cout << "error:撮合失败！" << endl;
				O sellRollBack = sellQuene.top();
				O buyRollBack = buyQuene.top();
				if (eq(sellCopy, sellRollBack))
				{
					sellQuene.push(sellCopy);
					cout << "sell回滚！" << endl;
				}
				if (eq(buyCopy, buyRollBack))
				{
					buyQuene.push(buyCopy);
					cout << "buy回滚！" << endl;
				}
	}
			
	
}

bool TradeCode::eq(O & o1, O & o2)
{
	if (!o1.getOrderId().empty()&& !o2.getOrderId().empty())
	{
		if ((o1.getOrderId().compare(o2.getOrderId()))>0)
		{
			return true;
		}
	}
	return false;
}

void TradeCode::remaveSell(priority_queue<O, vector<O>, SellQuene>& p)
{
	
	OrderTradeThread OTT;
	priority_queue<O, vector<O>, SellQuene> pCopy;
	while (!p.empty())
	{	
			O o = p.top();
			if ((o.getOrderId().compare(top_RemoveSellQueue().getOrderId()))>0)
			{
				Tx tx;
				tx.setBuyOrderId("");
				tx.setCancelUserId(o.getUserUuid());
				tx.setCancelOrderId(o.getOrderId());
				tx.setCancelRemainAmt(o.getFreezeAmt());
				tx.setTxType("sell");
				try
				{
					EnterCriticalSection(&RabbitMQ_Send_CS);
					RabbitMQ rmq;
					rmq.send(tx);
					rmq.~RabbitMQ();
					LeaveCriticalSection(&RabbitMQ_Send_CS);
				}
				catch (const std::exception&)
				{
					cout << "remaveSell RMQ失败！" << endl;
				}
				//rmq.~RabbitMQ();
			}
			else
			{
				pCopy.push(o);
			}
			p.pop();
	}
	delete_RemoveSellQueue();
	sellQuene = pCopy;
	
}

void TradeCode::remaveBuy(priority_queue<O, vector<O>, BuyQuene>& p)
{
	
	OrderTradeThread OTT;
	priority_queue<O, vector<O>, BuyQuene> pCopy;
	while (!p.empty())
	{
			O o = p.top();
			O buy;
			EnterCriticalSection(&removeBuyQuene_CS);
			if(!removeBuyQuene.empty())
			buy = removeBuyQuene.top();
			LeaveCriticalSection(&removeBuyQuene_CS);
			if ((OTT.eq(o, buy)))
			{
				//RabbitMQ rmq;
				Tx tx;
				tx.setCancelUserId(o.getUserUuid());
				tx.setCancelOrderId(o.getOrderId());
				tx.setCancelRemainAmt(o.getFreezeAmt());
				tx.setTxType("buy");
				try
				{
					EnterCriticalSection(&RabbitMQ_Send_CS);
					RabbitMQ rmq;
					rmq.send(tx);
					rmq.~RabbitMQ();
					LeaveCriticalSection(&RabbitMQ_Send_CS);
				}
				catch (const std::exception&)
				{
					cout << "remaveBuy RMQ失败！" << endl;
				}
				//rmq.~RabbitMQ();
			}
			else
			{
				pCopy.push(o);
			}
			p.pop();
			
	}
	buyQuene = pCopy;
	EnterCriticalSection(&removeBuyQuene_CS);
	if (!removeBuyQuene.empty())
	removeBuyQuene.pop();
	LeaveCriticalSection(&removeBuyQuene_CS);
}


O TradeCode::top_SellQueue()
{
	EnterCriticalSection(&sellQuene_CS);
	O sell = sellQuene.top();
	sellQuene.pop();
	LeaveCriticalSection(&sellQuene_CS);
	return sell;
}
O TradeCode::top_BuyQueue()
{
	EnterCriticalSection(&buyQuene_CS);
	O buy = buyQuene.top();
	buyQuene.pop();
	LeaveCriticalSection(&buyQuene_CS);
	return buy;
}

O TradeCode::top_RemoveSellQueue()
{
	EnterCriticalSection(&removeSellQuene_CS);
	O buy = removeSellQuene.top();
	LeaveCriticalSection(&removeSellQuene_CS);
	return buy;
}

O TradeCode::top_RemoveBuyQueue()
{
	EnterCriticalSection(&removeBuyQuene_CS);
	O buy = removeBuyQuene.top();
	LeaveCriticalSection(&removeBuyQuene_CS);
	return buy;
}

void TradeCode::Redis_()
{
	Redis* red((redis.getObj()).get());
	Redis redis_ = *red;
	ToJson<SellQuene> toJson;
	redis_.open();
	redis_.setRedis("sellQuene", toJson.OToJson(sellQuene));
	toJson.~ToJson();

	ToJson<BuyQuene> toJson_;
	redis_.setRedis("buyQuene", toJson_.OToJson(buyQuene));

	toJson_.~ToJson();
}


void TradeCode::delete_BuyQueue()
{
	EnterCriticalSection(&buyQuene_CS);
	buyQuene.pop();
	LeaveCriticalSection(&buyQuene_CS);
}

void TradeCode::delete_SellQueue()
{
	EnterCriticalSection(&sellQuene_CS);
	sellQuene.pop();
	LeaveCriticalSection(&sellQuene_CS);
}

void TradeCode::delete_RemoveBuyQueue()
{
	EnterCriticalSection(&removeBuyQuene_CS);
	removeBuyQuene.pop();
	LeaveCriticalSection(&removeBuyQuene_CS);
}

void TradeCode::delete_RemoveSellQueue()
{
	EnterCriticalSection(&removeSellQuene_CS);
	removeSellQuene.pop();
	LeaveCriticalSection(&removeSellQuene_CS);
}