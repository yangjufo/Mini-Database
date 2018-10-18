#pragma once
#include<vector>
#include<string>
#include"common.h"
#include"Interpreter.h"
#include"buffer.h"
#include"catalog.h"

extern class catalog cata;
extern class buffer Buffer;
class RecorderManager
{
public:
	

	RecorderManager(){};
	bool checkDtable(const Table &table, Row &deldata);//判断是否有删除的空位
	Data InsertValue(const Table &table, const Data &data);//插入非删除表
	Data SelectNoIndex(const Table &table,  Conditiondata conditions);//没有经过index筛选的选择
	Data SelectIndex(const Table &table, IndexColumn &indexdata, Conditiondata conditions);//经过index的选择之后再选择
	IndexColumn CreateIndexData(const Table &table, const Index &index);//create index时弹出所有记录的信息
	Data DeleteNoIndex(const Table &table, Conditiondata conditions);//删除记录，无index,返回删除的数据记录
	Data DeleteIndex(const Table &table, const IndexColumn &indexdata, Conditiondata conditions);//有index的删除
	IndexColumn GetIndexData(const Table &table, const Index &index, const Data &data);//在删除和插入后返回index的数据
	bool InsertDelValue(const Table &table, const Data &data);//把被删除的记录插入表，同时把第一个属性改为biggestnum+blockoffset
	int calculate(int type, string s1, string s2);
};