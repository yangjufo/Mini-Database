#pragma once
using namespace std;




#include<iostream>
#include <string>
#include"common.h"
#include"catalog.h"

#define block_size 4096
#define max_number_block_memory 300

typedef struct node Block;
struct node{
	
	
	//string filename;	//index or recod  1表示index， 2表示record
	int biggestnum;		// block的实际编号，文件编号+block的偏移量
	int blockOffset;	//which block offset in file， 写回文件中要用
	int type;			//标记是index还是record，写回内存中要用， type=1是index，type=2是record
	int blockindex;		//block index in memory   内存中的第几号block
	bool isWritten;		//是否被修改过，如果被修改过就要被写会内存
	int vaild_number;   //block 中有效记录到哪里
	char block[block_size];
};


class buffer{
public:
	string filename;

	Block b[max_number_block_memory];
	int valid_numer_block;

	buffer();
	~buffer();

	void initialize(int );							//初始化内存中的一个block

	//用于处理record的请求的   直接是biggestnum
	int malloc_block(int& biggestnum);
	//用于处理index的请求的，第一个int 是文件名 第二个biggest num
	int malloc_block(int &, int &);	//查询该block是否在内存中，如果在内存中，直接返回，不在，从disk调出
	


	int create_newblock_table(Table &T);
	int  create_newblock_index(Index &I);


	//int  create_newblock_index(Index & I);			    //为index创建一个新的空的block，直接在文件最后面存放，注意disk中没有修改，是block被踢出memory的时候再修改的
    //int  create_newblock_table(Table & );				//为record创建一个新的空的block，返回内存中block的偏移量， 
	
	bool write_block_record(int &biggestnum,int &index);					//将record block写回内存（传入参数为第几个block的参数）
	bool write_block_index(int &biggestnum, int &index);					//将index block写回内存（传入参数为第几个block的参数）

	bool write_all_block();								//退出数据库系统，则将内存中所有的block写会disk


	int  get_insert_position(int &biggestnum,int &type);			//得到编号为biggestnum的block的要插入的内存中的index,是第几个block
	int check_block_memory(int &biggestnum,int &type);			//检测编号为biggestnum的block是否在memory中,有的话返回第几个block，没有的话返回-1
	int  update_LR(int& first);							//if(first!=-1) 将下标first编号的block的LR置为1，并且更新其他对应的LR Value
														//else则找出LRU最大的编号，并将其返回，并更新其他的LRU
	
private:
	int LRUvalue[max_number_block_memory];//用于实现LRU算法,the lower, the better

};




