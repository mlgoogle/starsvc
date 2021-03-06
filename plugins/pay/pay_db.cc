//  Copyright (c) 2017-2018 The SWP Authors. All rights reserved.
//  Created on: 2017年2月12日 Author: kerry
#include <mysql.h>
#include "logic/logic_unit.h"
#include "pay/pay_db.h"
#include "basic/basic_util.h"

namespace pay_logic {

PayDB::PayDB(config::FileConfig* config) {
  mysql_engine_ = base_logic::DataEngine::Create(MYSQL_TYPE);
  mysql_engine_->InitParam(config->mysql_db_list_);
}

PayDB::~PayDB() {
  if (mysql_engine_) {
    delete mysql_engine_;
    mysql_engine_ = NULL;
  }
}

bool PayDB::OnUpdateRechargeOrder(const int64 uid, const int64 rid,
                                  const int32 status, const int32 rtype) {
  bool r = false;
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();

  std::string sql;
  int64 bigr_type = rtype;
  int64 big_status = status;
  //call actuals.proc_UpdateRechargeStatus(68,2119690335832071194,1,1)
  sql = "call proc_UpdateRechargeStatus("
      + base::BasicUtil::StringUtil::Int64ToString(uid) + ","
      + base::BasicUtil::StringUtil::Int64ToString(rid) + ","
      + base::BasicUtil::StringUtil::Int64ToString(status) + ","
      + base::BasicUtil::StringUtil::Int64ToString(bigr_type) + ");";
  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value *) (dict), NULL);

  if (dict) {
    delete dict;
    dict = NULL;
  }
  return true;
}

bool PayDB::OnUpdateCallBackRechargeOrder(const int64 rid, const double price,
                                          const std::string& transaction_id,
                                          const int32 astatus, int64& uid,
                                          double& balance) {
  LOG_DEBUG2("trade_no[%s], trade_status[%d], out_trade_no[%ld]",transaction_id.c_str(), astatus, rid);
  bool r = false;
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  base_logic::DictionaryValue *info_value = NULL;
  std::string sql;
  int64 bigr_type = 0;
  if (astatus == 1) //成功
    bigr_type = 3;
  else
    bigr_type = 4;
  //`proc_UpdateCallBackRechargeOrder`(in i_rid bigint,in i_price double,in i_status int,in i_transaction_id varchar(128))
  sql = "call proc_UpdateCallBackRechargeOrder("
      + base::BasicUtil::StringUtil::Int64ToString(rid) + ","
      + base::BasicUtil::StringUtil::DoubleToString(price) + ","
      + base::BasicUtil::StringUtil::Int64ToString(bigr_type) + ",'"
      + transaction_id + "'" + ");";
  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value *) (dict),
                              CallUpdateCallBackRechargeOrder);

  if (!r)
  {
    if (dict) { delete dict; dict = NULL; }
    return false;
  }
  dict->GetDictionary(L"resultvalue", &info_value);

  r = info_value->GetBigInteger(L"uid", &uid);
  r = info_value->GetReal(L"balance", &balance);
  if (dict) {
    delete dict;
    dict = NULL;
  }
  return true;
}

bool PayDB::OnCreateRechargeOrder(const int64 uid, const int64 rid,
                                  const double price, const int32 rtype) {
  bool r = false;
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();

  std::string sql;
  int64 bigr_type = rtype;
  //call actuals.proc_CreateRechargeOrder(32,12324141412,0.01,1)
  sql = "call proc_CreateRechargeOrder("
      + base::BasicUtil::StringUtil::Int64ToString(uid) + ","
      + base::BasicUtil::StringUtil::Int64ToString(rid) + ","
      + base::BasicUtil::StringUtil::DoubleToString(price) + ","
      + base::BasicUtil::StringUtil::Int64ToString(bigr_type) + ");";
  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value *) (dict), NULL);

  if (dict) {
    delete dict;
    dict = NULL;
  }
  return true;
}

void PayDB::CallCreateRechargeOrder(void* param, base_logic::Value* value) {
  //预留写入数据库是否成功
}

void PayDB::CallUpdateCallBackRechargeOrder(void* param,
                                            base_logic::Value* value) {
  base_logic::DictionaryValue *dict = (base_logic::DictionaryValue *) (value);
  base_logic::ListValue *list = new base_logic::ListValue();
  base_storage::DBStorageEngine *engine =
      (base_storage::DBStorageEngine *) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  base_logic::DictionaryValue *info_value = new base_logic::DictionaryValue();
  info_value->SetInteger(L"result", 0);
  if (num > 0) {
    while (rows = (*(MYSQL_ROW *) (engine->FetchRows())->proc)) {
      if (rows[0] != NULL)
        info_value->SetBigInteger(L"uid", atoll(rows[0]));
      if (rows[1] != NULL)
        info_value->SetReal(L"balance", atof(rows[1]));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value *) (info_value));
}

bool PayDB::OnCheckPayPwd(const int64 uid, std::string& pwd, int32& flag) {
  bool r = false;
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  base_logic::DictionaryValue *info_value = NULL;
  std::string sql;
  sql = "call proc_CheckPayPwd(" + base::BasicUtil::StringUtil::Int64ToString(uid) + 
  	    ",'" + pwd + "');";

  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value *) (dict),
                              CallCheckPayPwd);
  //if (!r)
   // return false;

  if (!r)
  {
    if (dict) { delete dict; dict = NULL; }
    return false;
  }
  dict->GetDictionary(L"resultvalue", &info_value);
  r = info_value->GetInteger(L"result", &flag);

  if (dict) {
    delete dict;
    dict = NULL;
  }
  return r;
}

void PayDB::CallCheckPayPwd(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue *dict = (base_logic::DictionaryValue *) (value);
  base_storage::DBStorageEngine *engine =
      (base_storage::DBStorageEngine *) (param);
  MYSQL_ROW rows;
  base_logic::DictionaryValue *info_value = new base_logic::DictionaryValue();
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW *) (engine->FetchRows())->proc)) {
      if (rows[0] != NULL)
        info_value->SetInteger(L"result", atoi(rows[0]));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value *) (info_value));
}

void PayDB::CallUnionWithdrow(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue *dict = (base_logic::DictionaryValue *) (value);
  base_logic::ListValue *list = new base_logic::ListValue();
  base_storage::DBStorageEngine *engine =
      (base_storage::DBStorageEngine *) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  base_logic::DictionaryValue *info_value = new base_logic::DictionaryValue();
  //info_value->SetInteger(L"result", 0);
  if (num > 0) {
    while (rows = (*(MYSQL_ROW *) (engine->FetchRows())->proc)) {
      if (rows[0] != NULL)
        info_value->SetBigInteger(L"result", atoll(rows[0]));
      if (rows[1] != NULL)
        info_value->SetReal(L"balance", atof(rows[1]));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value *) (info_value));
}

bool PayDB::OnCreateUnionWithDraw(const int64 uid, const int64 rid, const double price)
{
  bool r = false;
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();

  std::string sql;
  //
  sql = "call proc_Withdraw("
      + base::BasicUtil::StringUtil::Int64ToString(uid) + ","
      + base::BasicUtil::StringUtil::Int64ToString(rid) + ","
      + base::BasicUtil::StringUtil::DoubleToString(price) + ");";

  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value *) (dict), CallUnionWithdrow);

  if (!r)
  {
    if (dict) { delete dict; dict = NULL; }
    return false;
  }
  base_logic::DictionaryValue *info_value = NULL;
  int64 result = 0;
  dict->GetDictionary(L"resultvalue", &info_value);

  r = info_value->GetBigInteger(L"result", &result);
  if (r && result == 1) //创建订单成功
      r = true;
  else
      r = false;
  //r = info_value->GetReal(L"balance", &balance);
  if (dict) {
    delete dict;
    dict = NULL;
  }
  return r;
}

}  // namespace history_logic

