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
	
	write_all_block();								//退出数据库系统，则将内存中所有的block写会disk
	
}

/*得到编号为biggestnum的block的要插入的内存中的index,
同时更新LRUvalur
*/
int buffer::get_insert_position(int &biggestnum,int &type)			
{

	int result;
	int flag = -1;
	result = check_block_memory(biggestnum,type);
	if (result == -1){//内存中还没有这个block
		result = update_LR(biggestnum);
	}
	else{//内存中有了这个block
		update_LR(flag);
		LRUvalue[result] = 1;
	}

	return result;


}



/*检测编号为biggestnum的block是否在memory中,
有的话返回内存中的block_index，没有的话返回-1*/
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

/*初始化内存中index为i的block node
*/
void buffer::initialize(int i)
{
	b[i].isWritten = 0;
	b[i].vaild_number = 0;
	
	
}

int buffer::update_LR(int& first)
{
	int result = -1;		//-1是为了标记调用了哪个功能，返回-1说明输入的first也为-1
	//在内存中有这样中的一个block
	if (first == -1){
		for (int i = 0; i < valid_numer_block; i++){
			LRUvalue[i]++;
		}
		//LRUvalue[first] = 1;
		return -1;
	}


	//first==-1,返回的应该是LRUmax的block编号
	
	else{
		
		//内存没满
		if (valid_numer_block < max_number_block_memory){
			for (int i = 0; i < valid_numer_block; i++) {
				LRUvalue[i]++;
			}
			result = valid_numer_block;
			valid_numer_block++;
		}
		else{		//内存满了，找到LRU最大的替换出去
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



/*用于处理record的请求的
输入的直接是biggestnum
*/
int buffer::malloc_block(int& biggestnum)
{
	int result;
	int offset;
	char name[10];
	int type = 2;
	int fileoffset = biggestnum / MAX_NUMBER_BLOCK;
	//biggest num已经在内存中了，直接返回编号，并且更新LRvalue
	if (check_block_memory(biggestnum,type) != -1) {
		result = get_insert_position(biggestnum,type);
	}

	//该block不在内存中，从文件总读取
	else {
		result = get_insert_position(biggestnum,type);
		if (b[result].isWritten == true) {					//在这个位置上的block已经被修改了，将其写回内存
			write_block_index(b[result].biggestnum, result);
		}

		_itoa(fileoffset, name, 10);
		string filepath = name;
		filepath = ".\\record\\" + filepath + ".data";				//获得文件名
		offset = biggestnum - fileoffset*MAX_NUMBER_BLOCK - 1;		//block偏移量是从1开始的
		offset = offset*sizeof(struct node);

		ofstream fout_test;
		ifstream fout;

		fout_test.open(filepath.c_str(), ios::binary | ios::app);		//防止原先data文件没有建立
		fout_test.close();

		fout.open(filepath.c_str(), ios::binary | ios::in);

		fout.seekg(offset, ios::beg);							//移动到对应文件位置，万一超过文件的末尾了呢？
																//	b[result].biggestnum = 0;
		fout.read((char*)(b + result), sizeof(struct node));

		fout.close();
	}

	b[result].biggestnum = biggestnum;
	b[result].type = 2;

	return result;

}


/*用于处理index的请求的，
第一个int 是文件名 第二个biggest num
查询该block是否在内存中，如果在内存中，直接返回，不在，从disk调出*/
int buffer::malloc_block(int & fileoffset, int &biggestnum)
{
	int result;
	int offset;
	char name[10];
	int type = 1;

	//biggest num已经在内存中了，直接返回编号，并且更新LRvalue
	if (check_block_memory(biggestnum,type) != -1) {
		result = get_insert_position(biggestnum,type);
	}

	//该block不在内存中，从文件总读取
	else {
		result = get_insert_position(biggestnum,type);
		if (b[result].isWritten == true) {					//在这个位置上的block已经被修改了，将其写回内存
			write_block_index(b[result].biggestnum, result);
		}

		_itoa(fileoffset, name, 10);
		string filepath = name;
		filepath = ".\\index\\" + filepath + ".data";				//获得文件名
		offset = biggestnum - fileoffset*MAX_NUMBER_BLOCK - 1;		//block偏移量是从1开始的
		offset = offset*sizeof(struct node);

		ofstream fout_test;
		ifstream fout;

		fout_test.open(filepath.c_str(), ios::binary | ios::app);		//防止原先data文件没有建立
		fout_test.close();

		fout.open(filepath.c_str(), ios::binary | ios::in);

		fout.seekg(offset, ios::beg);							//移动到对应文件位置，万一超过文件的末尾了呢？
	//	b[result].biggestnum = 0;
		fout.read((char*)(b + result), sizeof(struct node));

		fout.close();
	}

	b[result].biggestnum = biggestnum;								//在内存中buffer node的总的biggestnum是负数的
	b[result].type = 1;
	return result;
}


/*为index创建一个新的空的block，直接在文件最后面存放,返回的是
注意disk中没有修改，是block被踢出memory的时候再修改的
注意API还要调用*/
extern int totalblock;
int  buffer::create_newblock_index(Index &I)
{
	//return ++totalblock;
	 cata.add_Lblockindex(I) ;
	int  result = cata.get_Lblockindex(I);
	
	return result;
}


/*为record创建一个新的空的block，返回内存中block的偏移量*/
int buffer::create_newblock_table(Table &T)
{

	int result;
	cata.add_Lblockindex(T);
	int  biggestnum = cata.get_Lblockindex(T);

	result = malloc_block( biggestnum);

	return result;

}


/*将record block写回内存（传入参数为第几个block的参数）*/
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
/*将record block写回内存（传入参数为第几个block的参数）*/
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

bool buffer::write_all_block()								//退出数据库系统，则将内存中所有的block写会disk
{
	for (int i = 0; i < valid_numer_block; i++){
		if (b[i].type == 2) {									//type==2,则插入record
			write_block_record(b[i].biggestnum, i);
		}
		else {
			write_block_index(b[i].biggestnum, i);

		}
	}
	return true;
}