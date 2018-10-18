#pragma once
#include<string>
#include<vector>/*
#include"buffer.h"
#include"catalog.h"*/
using namespace std;


#define INT 10
#define FLOAT 11
#define CHAR 12
#define INTLEN 10
#define FLOATLEN 20
#define MAX "1999999999"
#define MIN "0"
#define COMLEN		400 
#define INPUTLEN	200 
#define WORDLEN		100
#define VALLEN		300
#define NAMELEN		100

class Attribute{
public:
	string name; //����
	int type;  //����
	int length; //����
	bool isPk;  //Primary key���
	bool isUnique; //Unique���
	Attribute(){
		isPk = false;
		isUnique = false;
	}
	Attribute(string n, int t, int l, bool p, bool u)
		:name(n), type(t), length(l), isPk(p), isUnique(u){}
	~Attribute(){}
};

class Table{
public:
	string name; //������� 
	int attriNum;  //���е���������
	int totalLength; //һ����¼���ܳ���
	int primaryindex;  //�����һ��Ԫ��ΪPrimary index
	int biggestnum;    //���������һ����block�ı�ţ��ļ����+block��ţ� ����insert ����
	vector<Attribute>attributes; //���Ե���Ϣ
	Table() :name(""), attriNum(0), totalLength(0){}
	~Table(){}
};

class Index
{
public:
	string index_name;					//������
	string table_name;					//��Ӧ�ı���
	string attribute_name;				//��Ӧ����������
	int BBN;							//��¼�����ļ������block�ı��
	int columnLength;					//��¼����index��Ӧ��value���ܳ���
	int blockoffset;					//block��ƫ����
	int fileoffset;						//���index����һ���ļ��е�  ��¼���ļ�ƫ��
	int type;							//���Զ�Ӧ����
	Index() : columnLength(0){}
	~Index(){}
};

class Row{
public:
	vector<string> columns; //�洢ÿһ��ֵ
	int isDelete;//�������ɾ��
	int biggestnum; //��¼��Ӧ�ļ���block���
	int blockoffset; //��block�е�ƫ����
	Row() :isDelete(0), biggestnum(0), blockoffset(0){};
};
class Data{
public:
	vector<Row>rows;
};

enum Comparision{ Lt, Le, Gt, Ge, Eq, Ne, LtLt, LtLe, LeLt, LeLe, Un };
class Condition{
public:
	Comparision op; //�������ͣ�>,<,>=,<=,<>��
	string column_name; //��Ӧ����������
	string minValue; //��Сֵ,>,>=�������Ϊ<,<=ʱʹ��
	string maxValue; //���ֵ,<,<=ʹ��
	string eqValue; //��ֵ��ѯ
	string eqValue2; //�ڶ���ֵ�����Ϊ<=ʱʹ��
	Condition(){
		op = Un;
		column_name = "";
		minValue = MIN;
		maxValue = MAX;
		eqValue = "";
	}
};

class Conditiondata{
public:
	vector<Condition>conds;
};

class IndexRow {
public:
	int blockoffset;//�ļ�����ڼ���block
	int fileoffset;//�����block�������ڼ�����¼
	string key;
};

class IndexColumn {
public:
	vector<IndexRow> rows;
};

