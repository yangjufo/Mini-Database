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
	
	
	//string filename;	//index or recod  1��ʾindex�� 2��ʾrecord
	int biggestnum;		// block��ʵ�ʱ�ţ��ļ����+block��ƫ����
	int blockOffset;	//which block offset in file�� д���ļ���Ҫ��
	int type;			//�����index����record��д���ڴ���Ҫ�ã� type=1��index��type=2��record
	int blockindex;		//block index in memory   �ڴ��еĵڼ���block
	bool isWritten;		//�Ƿ��޸Ĺ���������޸Ĺ���Ҫ��д���ڴ�
	int vaild_number;   //block ����Ч��¼������
	char block[block_size];
};


class buffer{
public:
	string filename;

	Block b[max_number_block_memory];
	int valid_numer_block;

	buffer();
	~buffer();

	void initialize(int );							//��ʼ���ڴ��е�һ��block

	//���ڴ���record�������   ֱ����biggestnum
	int malloc_block(int& biggestnum);
	//���ڴ���index������ģ���һ��int ���ļ��� �ڶ���biggest num
	int malloc_block(int &, int &);	//��ѯ��block�Ƿ����ڴ��У�������ڴ��У�ֱ�ӷ��أ����ڣ���disk����
	


	int create_newblock_table(Table &T);
	int  create_newblock_index(Index &I);


	//int  create_newblock_index(Index & I);			    //Ϊindex����һ���µĿյ�block��ֱ�����ļ�������ţ�ע��disk��û���޸ģ���block���߳�memory��ʱ�����޸ĵ�
    //int  create_newblock_table(Table & );				//Ϊrecord����һ���µĿյ�block�������ڴ���block��ƫ������ 
	
	bool write_block_record(int &biggestnum,int &index);					//��record blockд���ڴ棨�������Ϊ�ڼ���block�Ĳ�����
	bool write_block_index(int &biggestnum, int &index);					//��index blockд���ڴ棨�������Ϊ�ڼ���block�Ĳ�����

	bool write_all_block();								//�˳����ݿ�ϵͳ�����ڴ������е�blockд��disk


	int  get_insert_position(int &biggestnum,int &type);			//�õ����Ϊbiggestnum��block��Ҫ������ڴ��е�index,�ǵڼ���block
	int check_block_memory(int &biggestnum,int &type);			//�����Ϊbiggestnum��block�Ƿ���memory��,�еĻ����صڼ���block��û�еĻ�����-1
	int  update_LR(int& first);							//if(first!=-1) ���±�first��ŵ�block��LR��Ϊ1�����Ҹ���������Ӧ��LR Value
														//else���ҳ�LRU���ı�ţ������䷵�أ�������������LRU
	
private:
	int LRUvalue[max_number_block_memory];//����ʵ��LRU�㷨,the lower, the better

};




