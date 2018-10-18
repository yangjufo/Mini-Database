#include "buffer.h"
#include<fstream>
#include"catalog.h"

extern catalog cata;

buffer::buffer()
{
	for (int i = 0; i < max_number_block_memory; i++){
		initialize(i);
		LRUvalue[i] = 0;
	}
	valid_numer_block = 0;

}
int add(){
	for (int i = 0; i < 100; i++){

	}
	return 0;
}

buffer::~buffer()
{
	
	write_all_block();								//�˳����ݿ�ϵͳ�����ڴ������е�blockд��disk
	
}

/*�õ����Ϊbiggestnum��block��Ҫ������ڴ��е�index,
ͬʱ����LRUvalur
*/
int buffer::get_insert_position(int &biggestnum,int &type)			
{

	int result;
	int flag = -1;
	result = check_block_memory(biggestnum,type);
	if (result == -1){//�ڴ��л�û�����block
		result = update_LR(biggestnum);
	}
	else{//�ڴ����������block
		update_LR(flag);
		LRUvalue[result] = 1;
	}

	return result;


}



/*�����Ϊbiggestnum��block�Ƿ���memory��,
�еĻ������ڴ��е�block_index��û�еĻ�����-1*/
int buffer::check_block_memory(int &biggestnum,int &type)				
{
	int result = -1;
	for (int i=0; i < valid_numer_block; i++){
		if (b[i].biggestnum == biggestnum&&b[i].type==type){
			result = i;
			break;
		}
	}

	return result;
}

/*��ʼ���ڴ���indexΪi��block node
*/
void buffer::initialize(int i)
{
	b[i].isWritten = 0;
	b[i].vaild_number = 0;
	
	
}

int buffer::update_LR(int& first)
{
	int result = -1;		//-1��Ϊ�˱�ǵ������ĸ����ܣ�����-1˵�������firstҲΪ-1
	//���ڴ����������е�һ��block
	if (first == -1){
		for (int i = 0; i < valid_numer_block; i++){
			LRUvalue[i]++;
		}
		//LRUvalue[first] = 1;
		return -1;
	}


	//first==-1,���ص�Ӧ����LRUmax��block���
	
	else{
		
		//�ڴ�û��
		if (valid_numer_block < max_number_block_memory){
			for (int i = 0; i < valid_numer_block; i++) {
				LRUvalue[i]++;
			}
			result = valid_numer_block;
			valid_numer_block++;
		}
		else{		//�ڴ����ˣ��ҵ�LRU�����滻��ȥ
			int max = -1;
			for (int i = 0; i < valid_numer_block; i++){	
				if (LRUvalue[i]>max){
					max = LRUvalue[i];
					result = i;
					LRUvalue[result]++;				
				}
				else{
					LRUvalue[i]++;
				}
			}
			LRUvalue[result] = 1;
		}
	}
	return result;
}



/*���ڴ���record�������
�����ֱ����biggestnum
*/
int buffer::malloc_block(int& biggestnum)
{
	int result;
	int offset;
	char name[10];
	int type = 2;
	int fileoffset = biggestnum / MAX_NUMBER_BLOCK;
	//biggest num�Ѿ����ڴ����ˣ�ֱ�ӷ��ر�ţ����Ҹ���LRvalue
	if (check_block_memory(biggestnum,type) != -1) {
		result = get_insert_position(biggestnum,type);
	}

	//��block�����ڴ��У����ļ��ܶ�ȡ
	else {
		result = get_insert_position(biggestnum,type);
		if (b[result].isWritten == true) {					//�����λ���ϵ�block�Ѿ����޸��ˣ�����д���ڴ�
			write_block_index(b[result].biggestnum, result);
		}

		_itoa(fileoffset, name, 10);
		string filepath = name;
		filepath = ".\\record\\" + filepath + ".data";				//����ļ���
		offset = biggestnum - fileoffset*MAX_NUMBER_BLOCK - 1;		//blockƫ�����Ǵ�1��ʼ��
		offset = offset*sizeof(struct node);

		ofstream fout_test;
		ifstream fout;

		fout_test.open(filepath.c_str(), ios::binary | ios::app);		//��ֹԭ��data�ļ�û�н���
		fout_test.close();

		fout.open(filepath.c_str(), ios::binary | ios::in);

		fout.seekg(offset, ios::beg);							//�ƶ�����Ӧ�ļ�λ�ã���һ�����ļ���ĩβ���أ�
																//	b[result].biggestnum = 0;
		fout.read((char*)(b + result), sizeof(struct node));

		fout.close();
	}

	b[result].biggestnum = biggestnum;
	b[result].type = 2;

	return result;

}


/*���ڴ���index������ģ�
��һ��int ���ļ��� �ڶ���biggest num
��ѯ��block�Ƿ����ڴ��У�������ڴ��У�ֱ�ӷ��أ����ڣ���disk����*/
int buffer::malloc_block(int & fileoffset, int &biggestnum)
{
	int result;
	int offset;
	char name[10];
	int type = 1;

	//biggest num�Ѿ����ڴ����ˣ�ֱ�ӷ��ر�ţ����Ҹ���LRvalue
	if (check_block_memory(biggestnum,type) != -1) {
		result = get_insert_position(biggestnum,type);
	}

	//��block�����ڴ��У����ļ��ܶ�ȡ
	else {
		result = get_insert_position(biggestnum,type);
		if (b[result].isWritten == true) {					//�����λ���ϵ�block�Ѿ����޸��ˣ�����д���ڴ�
			write_block_index(b[result].biggestnum, result);
		}

		_itoa(fileoffset, name, 10);
		string filepath = name;
		filepath = ".\\index\\" + filepath + ".data";				//����ļ���
		offset = biggestnum - fileoffset*MAX_NUMBER_BLOCK - 1;		//blockƫ�����Ǵ�1��ʼ��
		offset = offset*sizeof(struct node);

		ofstream fout_test;
		ifstream fout;

		fout_test.open(filepath.c_str(), ios::binary | ios::app);		//��ֹԭ��data�ļ�û�н���
		fout_test.close();

		fout.open(filepath.c_str(), ios::binary | ios::in);

		fout.seekg(offset, ios::beg);							//�ƶ�����Ӧ�ļ�λ�ã���һ�����ļ���ĩβ���أ�
	//	b[result].biggestnum = 0;
		fout.read((char*)(b + result), sizeof(struct node));

		fout.close();
	}

	b[result].biggestnum = biggestnum;								//���ڴ���buffer node���ܵ�biggestnum�Ǹ�����
	b[result].type = 1;
	return result;
}


/*Ϊindex����һ���µĿյ�block��ֱ�����ļ��������,���ص���
ע��disk��û���޸ģ���block���߳�memory��ʱ�����޸ĵ�
ע��API��Ҫ����*/
extern int totalblock;
int  buffer::create_newblock_index(Index &I)
{
	//return ++totalblock;
	 cata.add_Lblockindex(I) ;
	int  result = cata.get_Lblockindex(I);
	
	return result;
}


/*Ϊrecord����һ���µĿյ�block�������ڴ���block��ƫ����*/
int buffer::create_newblock_table(Table &T)
{

	int result;
	cata.add_Lblockindex(T);
	int  biggestnum = cata.get_Lblockindex(T);

	result = malloc_block( biggestnum);

	return result;

}


/*��record blockд���ڴ棨�������Ϊ�ڼ���block�Ĳ�����*/
bool buffer::write_block_record(int &biggestnum, int &index)
{
//	int result;
	int offset;
	char name[10];
	int fileoffset = biggestnum / MAX_NUMBER_BLOCK;
	long begin, endof;



	_itoa(fileoffset, name, 10);
	string filepath = name;
	filepath = ".\\record\\" + filepath + ".data";
	offset = biggestnum - fileoffset*MAX_NUMBER_BLOCK - 1;
	offset = offset*sizeof(struct node);

	fstream fout1;
	fout1.open(filepath.c_str(), ios::in | ios::app);
	fout1.close();

	fstream fout;
	fout.open(filepath.c_str(), ios::in | ios::out);


	fout.seekp(0, ios::beg);
	begin = fout.tellp();
	fout.seekp(0, ios::end);
	endof = fout.tellp();
	if ((endof - begin) >= offset){
		fout.seekp(offset, ios::beg);
		fout.write((char*)(b + index), sizeof(struct node));
	}
	else{
		fout.seekp(0, ios::end);
		for (int i = 0; i <= (offset-(endof - begin) ) / sizeof(struct node); i++){
			
			fout.write((char*)(b + index), sizeof(struct node));
		}
	}

	return true;
}
/*��record blockд���ڴ棨�������Ϊ�ڼ���block�Ĳ�����*/
bool buffer::write_block_index(int &biggestnum, int &index)
{
	//	int result;
	int offset;
	char name[10];
	int fileoffset = biggestnum / MAX_NUMBER_BLOCK;
	long begin, endof;



	_itoa(fileoffset, name, 10);
	string filepath = name;
	filepath = ".\\index\\" + filepath + ".data";
	offset = biggestnum - fileoffset*MAX_NUMBER_BLOCK - 1;
	offset = offset*sizeof(struct node);

	fstream fout1;
	fout1.open(filepath.c_str(), ios::in | ios::app);
	fout1.close();

	fstream fout;
	fout.open(filepath.c_str(), ios::in | ios::out);


	fout.seekp(0, ios::beg);
	begin = fout.tellp();
	fout.seekp(0, ios::end);
	endof = fout.tellp();
	if ((endof - begin) >= offset) {
		fout.seekp(offset, ios::beg);
		fout.write((char*)(b + index), sizeof(struct node));
	}
	else {
		fout.seekp(0, ios::end);
		for (int i = 0; i <= (offset - (endof - begin)) / sizeof(struct node); i++) {

			fout.write((char*)(b + index), sizeof(struct node));
		}
	}
	endof = fout.tellp();
	return true;
}

bool buffer::write_all_block()								//�˳����ݿ�ϵͳ�����ڴ������е�blockд��disk
{
	for (int i = 0; i < valid_numer_block; i++){
		if (b[i].type == 2) {									//type==2,�����record
			write_block_record(b[i].biggestnum, i);
		}
		else {
			write_block_index(b[i].biggestnum, i);

		}
	}
	return true;
}