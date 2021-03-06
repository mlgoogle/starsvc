//  Copyright (c) 2017-2018 The SWP Authors. All rights reserved.
//  Created on: 2017年1月12日 Author: kerry

#include "pay/wx_order.h"

#include <iostream>
#include <sstream>
#include "basic/md5sum.h"
#include "http/http_method.h"
#include "logic/logic_unit.h"
#include "logic/logic_comm.h"

namespace pay_logic {

WXOrder::WXOrder() {
  total_fee = 0;
}

WXOrder::~WXOrder() {
}

void WXOrder::InitWxVerify(const std::string& id, const std::string& m_id,
                           const std::string& t_type, const std::string& k_key,
                           const std::string& o_id) {
  appid = id;
  mch_id = m_id;
  notify_url = NOTIFY_URL;
  trade_type = t_type;
  if (trade_type == APP_TRADE_TYPE)
    package = WX_PACKAGE;
  else
    //prepay_id=微信订单id
    package = "prepay_id=" + out_trade_no;

  key = k_key;
  open_id = o_id;
  nonce_str = logic::SomeUtils::RandomString(32);
  //out_trade_no += logic::SomeUtils::RandomString(6);
}

void WXOrder::InitWxVerify() {
  appid = APPID;
  mch_id = MCH_ID;
  notify_url = NOTIFY_URL;
  trade_type = APP_TRADE_TYPE;
  package = WX_PACKAGE;
  key = APP_KEY;
  nonce_str = logic::SomeUtils::RandomString(32);
  //out_trade_no += logic::SomeUtils::RandomString(6);
}

void WXOrder::PlaceOrderSign() {
  std::stringstream ss;
  ss << "appid=" << appid << "&body=" << body << "&mch_id=" << mch_id
      << "&nonce_str=" << nonce_str << "&notify_url=" << notify_url;
  if (!open_id.empty())
    ss << "&openid=" << open_id.c_str();
  ss << "&out_trade_no=" << out_trade_no << "&spbill_create_ip="
      << spbill_create_ip << "&total_fee=" << total_fee << "&trade_type="
      << trade_type << "&key=" << key;

  LOG_DEBUG2("WX_ORDER_SIGN before: %s",ss.str().c_str());
  base::MD5Sum md5sum(ss.str());
  LOG_DEBUG2("WX_ORDER_SIGN_MD5 after: %s",md5sum.GetHash().c_str());
  sign = md5sum.GetHash();
}

//微信APP充值
void WXOrder::PreSign() {
  //重新赋值 nonce_str
  nonce_str = logic::SomeUtils::RandomString(32);
  std::stringstream ss;
  ss << time(NULL);
  timestamp = ss.str();
  ss.str("");
  ss.clear();
  if (trade_type == APP_TRADE_TYPE) {
    ss << "appid=" << appid << "&noncestr=" << nonce_str << "&package=Sign=WXPay"
          << "&partnerid=" << mch_id << "&prepayid=" << prepayid << "&timestamp="
          << timestamp << "&key=" << key;
  } else {
    ss << "appId=" << appid << "&nonceStr=" << nonce_str << "&package="
        << "prepayid=" << prepayid << "&signType=MD5&timeStamp=" << timestamp
        << "&key=" << key;
  }
  //LOG(INFO)<< "WX_PRE_SIGN before:" << ss.str();
  LOG_DEBUG2("WX_ORDER_SIGN before: %s",ss.str().c_str());
  base::MD5Sum md5sum(ss.str());
  //LOG(INFO)<< "WX_PRE_SIGN_MD5 after:" << md5sum.GetHash();
  LOG_DEBUG2("WX_ORDER_SIGN_MD5 after: %s",md5sum.GetHash().c_str());
  prepaysign = md5sum.GetHash();
}

std::string WXOrder::PostFiled() {
  base_logic::DictionaryValue dic;
  dic.SetString(L"appid", appid);
  dic.SetString(L"body", body);
  dic.SetString(L"mch_id", mch_id);
  dic.SetString(L"nonce_str", nonce_str);
  dic.SetString(L"notify_url", notify_url);
  dic.SetString(L"out_trade_no", out_trade_no);
  dic.SetString(L"spbill_create_ip", spbill_create_ip);
  dic.SetBigInteger(L"total_fee", total_fee);
  dic.SetString(L"trade_type", trade_type);
  dic.SetString(L"sign", sign);
  if (!open_id.empty())
    dic.SetString(L"openid", open_id);
  std::string filed = "";
  base_logic::ValueSerializer* serializer = base_logic::ValueSerializer::Create(
      base_logic::IMPL_XML, &filed);
  serializer->Serialize(dic);
  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_XML,
                                                serializer);
  return filed;
}

std::string WXOrder::PlaceOrder(const std::string& id, const std::string& m_id,
                                const std::string& trade_type,
                                const std::string& k_key, const int32 ptype,
                                const std::string& open_id) {
  InitWxVerify(id, m_id, trade_type, k_key, open_id);
  //PlaceOrderSign(id, m_id, trade_type, k_key, ptype, open_id);
  PlaceOrderSign();
  http::HttpMethodPost hmp(WX_URL);
  std::string headers = "Content-Type: text/xml";
  hmp.SetHeaders(headers);
  hmp.Post(PostFiled().c_str());
  std::string result;
  hmp.GetContent(result);
  //LOG(INFO)<< "http post result:" << result;
  LOG_DEBUG2("http post result: %s", result.c_str());
  return result;
}

std::string WXOrder::PlaceOrder() {
  InitWxVerify();
  PlaceOrderSign();
  http::HttpMethodPost hmp(WX_URL);
  std::string headers = "Content-Type: text/xml";
  hmp.SetHeaders(headers);
  hmp.Post(PostFiled().c_str());
  std::string result;
  hmp.GetContent(result);
  //LOG(INFO)<< "http post result:" << result;
  LOG_DEBUG2("http post result: %s", result.c_str());
  return result;
}

void WXOrder::PreSerialize(base_logic::DictionaryValue* dic) {
  if (dic != NULL) {
    dic->SetString(L"appid", appid);
    dic->SetString(L"partnerid", mch_id);
    dic->SetString(L"prepayid", prepayid);
    if (trade_type == "APP")
      dic->SetString(L"package", package);
    else {
      std::string str_package;
      str_package = "prepayid=" + prepayid;
      dic->SetString(L"package", str_package);
    }
    dic->SetString(L"noncestr", nonce_str);
    dic->SetString(L"timestamp", timestamp);
    dic->SetString(L"sign", prepaysign);
  }
}
}

