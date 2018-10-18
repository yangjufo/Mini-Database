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
	string name; //名字
	int type;  //类型
	int length; //长度
	bool isPk;  //Primary key标记
	bool isUnique; //Unique标记
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
	string name; //表的名称 
	int attriNum;  //表中的属性总数
	int totalLength; //一条记录的总长度
	int primaryindex;  //标记哪一个元素为Primary index
	int biggestnum;    //这个表的最后一个的block的编号（文件编号+block编号） 用于insert 操作
	vector<Attribute>attributes; //属性的信息
	Table() :name(""), attriNum(0), totalLength(0){}
	~Table(){}
};

class Index
{
public:
	string index_name;					//索引名
	string table_name;					//对应的表名
	string attribute_name;				//对应的属性名字
	int BBN;							//记录的是文件中最大block的编号
	int columnLength;					//记录的是index对应的value的总长度
	int blockoffset;					//block的偏移量
	int fileoffset;						//这个index是哪一个文件中的  记录他文件偏移
	int type;							//属性对应类型
	Index() : columnLength(0){}
	~Index(){}
};

class Row{
public:
	vector<string> columns; //存储每一个值
	int isDelete;//标记懒惰删除
	int biggestnum; //记录对应文件中block编号
	int blockoffset; //在block中的偏移量
	Row() :isDelete(0), biggestnum(0), blockoffset(0){};
};
class Data{
public:
	vector<Row>rows;
};

enum Comparision{ Lt, Le, Gt, Ge, Eq, Ne, LtLt, LtLe, LeLt, LeLe, Un };
class Condition{
public:
	Comparision op; //操作类型，>,<,>=,<=,<>等
	string column_name; //对应的属性名字
	string minValue; //最小值,>,>=或者左侧为<,<=时使用
	string maxValue; //最大值,<,<=使用
	string eqValue; //等值查询
	string eqValue2; //第二等值，左侧为<=时使用
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
	int blockoffset;//文件里面第几个block
	int fileoffset;//存的是block里面具体第几条记录
	string key;
};

class IndexColumn {
public:
	vector<IndexRow> rows;
};

