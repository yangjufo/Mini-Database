#pragma once
#include"common.h"

#define MAX_NUMBER_BLOCK 1000000  //��ʾһ��table���ʹ����ô���block

class catalog{
public:
	int number; //��ʾ�м���table�������������ã�Ϊ�˱���ά��ֻ�Ӳ���
	int number_index; //��ʾ�м���index �����ļ����
	vector<Table> Ta;
	vector<Index> In;

	catalog();					//��disk�ж�ȡindex��table����Ϣ��һֱ���ڴ���ά��
	~catalog();				//���ڴ��е�index ��table�е���Ϣ�ٴ�д��disk��

	bool CreateTable_catalog(const Table &); //�������ڱ�ļ�¼������index����ʼindex����Ϊ0����
	bool CreateIndex_catalog(const Index&);		//��������index�ļ�¼��Ϣ

	bool DropTable_catalog(const Table &);		//ɾ����
	bool Dropindex_catalog(const Index &);		//ɾ��index

	//table
	int get_Lblockindex(const Table &);			//�õ�������������һ��block�ı��
	bool add_Lblockindex(Table &);				//�Ա����������block��Ÿ���


	//index
	int get_Lblockindex(const Index &);			//�õ�������������һ��block�ı��
	bool add_Lblockindex(Index &);				//�Ա����������block��Ÿ���




	bool check_PK(const Table&);			//���attribute�Ƿ���PK
	bool check_Table(const Table &);			//check whether table exist

	bool check_attribute(const Table&);				//���table�����Ƿ���������ԣ������Table��ֻ��һ��attribute

	
	bool check_index(const Index &);			//check whether index exist
	vector<Index> check_index(Table &);	//�鿴���table����һ��Ԫ����index


	Table gettable(const Index&);				//����index������table��information

	Table get_table_info(const Table&);				//����table�����֣�����table����ϸinformation


};


