#pragma once
#include"common.h"

#define MAX_NUMBER_BLOCK 1000000  //表示一个table最多使用这么多的block

class catalog{
public:
	int number; //表示有几个table，用于译码所用，为了便于维护只加不减
	int number_index; //表示有几个index 用于文件编号
	vector<Table> Ta;
	vector<Index> In;

	catalog();					//从disk中读取index和table的信息并一直在内存中维护
	~catalog();				//将内存中的index 和table中的信息再次写回disk中

	bool CreateTable_catalog(const Table &); //创建关于表的记录（不含index，初始index个数为0），
	bool CreateIndex_catalog(const Index&);		//创建关于index的记录信息

	bool DropTable_catalog(const Table &);		//删除表
	bool Dropindex_catalog(const Index &);		//删除index

	//table
	int get_Lblockindex(const Table &);			//得到这个表的最后面的一个block的编号
	bool add_Lblockindex(Table &);				//对表这个最后面的block编号更新


	//index
	int get_Lblockindex(const Index &);			//得到这个表的最后面的一个block的编号
	bool add_Lblockindex(Index &);				//对表这个最后面的block编号更新




	bool check_PK(const Table&);			//检查attribute是否是PK
	bool check_Table(const Table &);			//check whether table exist

	bool check_attribute(const Table&);				//检查table里面是否有这个属性，输入的Table里只有一个attribute

	
	bool check_index(const Index &);			//check whether index exist
	vector<Index> check_index(Table &);	//查看这个table中哪一个元素是index


	Table gettable(const Index&);				//返回index所属的table的information

	Table get_table_info(const Table&);				//输入table的名字，返回table的详细information


};


