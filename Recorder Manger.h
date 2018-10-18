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
	bool checkDtable(const Table &table, Row &deldata);//�ж��Ƿ���ɾ���Ŀ�λ
	Data InsertValue(const Table &table, const Data &data);//�����ɾ����
	Data SelectNoIndex(const Table &table,  Conditiondata conditions);//û�о���indexɸѡ��ѡ��
	Data SelectIndex(const Table &table, IndexColumn &indexdata, Conditiondata conditions);//����index��ѡ��֮����ѡ��
	IndexColumn CreateIndexData(const Table &table, const Index &index);//create indexʱ�������м�¼����Ϣ
	Data DeleteNoIndex(const Table &table, Conditiondata conditions);//ɾ����¼����index,����ɾ�������ݼ�¼
	Data DeleteIndex(const Table &table, const IndexColumn &indexdata, Conditiondata conditions);//��index��ɾ��
	IndexColumn GetIndexData(const Table &table, const Index &index, const Data &data);//��ɾ���Ͳ���󷵻�index������
	bool InsertDelValue(const Table &table, const Data &data);//�ѱ�ɾ���ļ�¼�����ͬʱ�ѵ�һ�����Ը�Ϊbiggestnum+blockoffset
	int calculate(int type, string s1, string s2);
};