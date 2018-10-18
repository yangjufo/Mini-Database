#pragma once
#include<string>
#include<vector>
#include"common.h"
#include"buffer.h"
#include"catalog.h"
using namespace std;

extern class catalog cata;


#define SELECT		0
#define CRETAB		1
#define	CREIND		2
#define	DRPTAB		3
#define DRPIND		4
#define DELETE		5
#define INSERT		6
#define QUIT		7
#define EXEFILE		8

#define SELERR		10
#define SEL_NOWHERE 20
#define SEL_WHERE   30
#define COLERR	    40
#define TABEXISTED  11
#define CHARBOUND   21
#define NOPK		31
#define NOUNIQUE    41
#define CRETABERR   51
#define CREINDERR   12
#define INDEXISTED  22
#define NOTABEXIST  32
#define DELERR		15
#define INSERR		16
#define COLNUMERR   26
#define FLOATLONG	36
#define DROPERR		34
#define DROTABERR	13
#define DROINDERR   14
#define NOINDEXIST  24	
#define UNKNOW	    -1


class Interpreter{
public:
	int operation_type;
	int error_info;
	Table tableInfo;
	Index indexInfo;
	Data value; //insert的数据
    Conditiondata  conditions;//where后的情况
	Condition Pkcondition;//pk属性对应值
	Conditiondata Uniqueconditions;//unique属性对应值
	int PKpos;
	int Uniquepos;
	void inittable();
	void initcond();
	void initindex();
	void initvalue();
	Interpreter(){
		operation_type = UNKNOW;
	}
	~Interpreter(){};
	bool Parse(string command);
	bool GetWord(string &src, string &des);
	bool GetSelect(string &query);
	bool GetCreate(string &query);
	bool GetDrop(string &query);
	bool GetDelete(string &query);
	bool GetInsert(string &query);
	bool GetStr(string &src, string &des);
	bool GetFloat(string &src, string &des, int column_length);
	void MakeInitial(){
		operation_type = UNKNOW;
		error_info = UNKNOW;
		initcond();
		initindex();
		inittable();
		initvalue();
	}
	
};
