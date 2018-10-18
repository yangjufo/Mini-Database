#pragma once
#include<iostream>
#include<list>
#include"buffer.h"
#include"common.h"
#define LENGTHOFPOINTER 4;
//#define INT 4;

using namespace std;
extern class catalog cata;
extern class buffer Buffer;

class IndexLeaf {
public:
	string key;
	int offsetinFile;
	int offsetinBlock;
	IndexLeaf() :
		key(""), offsetinFile(0), offsetinBlock(0) {}
	IndexLeaf(string k, int oif, int oib) :
		key(k), offsetinFile(oif), offsetinBlock(oib) {}
};

class IndexBranch {
public:
	string key;
	int ptrChild;
	IndexBranch() :
		key(""), ptrChild(0) {}
	IndexBranch(string k, int ptrC) :
		key(k), ptrChild(ptrC) {}
};

class BPlusTree {
public:
	bool IntorString;
	bool StringorFloat;
	int blockNum;
	int ptrFather;		//block pointer, if is root, this pointer is useless
	int recordNum;
	int Max;
	int columnLength;
	BPlusTree() {}
	//BPlusTree() : recordNum(0) {}
};

class Branch : public BPlusTree
{
public:
	list<IndexBranch> nodelist;
	int lastChild;
	bool isRoot;
	Branch() {};
	void createBranch(bool IsRoot, int lengthOFcolumn, int type) {//先create之后再赋值ptrfather等
		this->isRoot = IsRoot;
		lastChild = 0;
		if (type == 10)
			IntorString = 1;
		else if (type == 11) {
			StringorFloat = 0;
			IntorString = 0;
		}
		else if (type == 12) {
			IntorString = 0;
			StringorFloat = 1;
		}
		ptrFather = 0;
		recordNum = 0;
		columnLength = lengthOFcolumn;
		int totallength = this->columnLength + LENGTHOFPOINTER;
		this->Max = 4081 / totallength;
	}
	Branch(char memory[], int lengthOFcolumn, int type);
	~Branch() {};
	void insert(IndexBranch node);
	IndexBranch pop();
	IndexBranch getfront();
	void writeclear() {
		nodelist.pop_front();
	}
	void clearall();
};

class Leaf : public BPlusTree
{
public:
	int nextSibling;	//block pointer
	int lastSibling;	//block pointer
	list<IndexLeaf> nodelist;
	Leaf() {};
	void createLeaf(int lengthOFcolumn, int type) {
		ptrFather = 0;
		recordNum = 0;
		nextSibling = 0;
		lastSibling = 0;
		this->columnLength = lengthOFcolumn;
		if (type == 10)
			IntorString = 1;
		else if (type == 11) {
			IntorString = 0;
			StringorFloat = 0;
		}
		else if (type == 12) {
			IntorString = 0;
			StringorFloat = 1;
		}
		int totallength = 2 * this->columnLength + LENGTHOFPOINTER;
		this->Max = 4078 / totallength;
	}
	Leaf(char memory[], int lengthOFcolumn, int type);
	~Leaf() {};
	void insert(IndexLeaf node);
	IndexLeaf pop();
	IndexLeaf getfront();
	void writeclear() {
		nodelist.pop_front();
	}
	void clearall();
};

class IndexManager {
public:
	void createIndex(Index& indexinfor, IndexColumn DataofIndex);
	void index_insertValue(Index indexinfor, IndexColumn data);
	IndexColumn selectEqual(Index indexinfor, string key/*, int blockoffset = 0*/);
	IndexColumn selectBetween(Index indexinfor, string keyFrom, string keyTo/*, int& blockoffset*/);
	//void dropIndex(Index& indexinfor);
	void deleteValue(Index indexinfor, IndexColumn dc/*, int&blockoffset*/);
	//void translateLeaf(char neicun[], Leaf needt);
	//void translateBranch(char neicun[], Branch needt);
	//list<IndexBranch>::iterator SearchlocaB(Branch& searched, IndexLeaf node);
	//list<IndexLeaf>::iterator SearchlocaL(Leaf& searched, IndexLeaf node, bool* valid);
private:
	//Row splitRow(Table tableinfor, string row);
	//string getColumnValue(const Table& tableinfor, const Index& indexinfor, string row);
	void insertValue(Branch& Searchnode, IndexRow dataRecord, Index& indexinfor);
	void translateLeaf(char neicun[], Leaf needt);
	void translateBranch(char neicun[], Branch needt);
	void InttoChar(int number, int startpos, int length, char neicun[]);
	int StringtoInt(string key);
	//void MemorytoLeaf(char memory[], Leaf receptor);
	//void MemorytoBranch(char memory[], Branch receptor);
	void insertvalue(Branch& Searchnode, IndexLeaf& data, Index& indexinfor, int Fp, bool* valid, int* ptrChild, string& ckey);
	void insertvalueL(Leaf& SearchnodeL, IndexLeaf& data, Index& indexinfor, bool* valid, int* prrChild, string& ckey);
	list<IndexBranch>::iterator SearchlocaB(Branch& searched, IndexLeaf node, bool* valid);
	list<IndexLeaf>::iterator SearchlocaL(Leaf& searched, IndexLeaf node, bool* valid);
	list<IndexLeaf>::iterator SearchlocaLM(Leaf& searched, IndexLeaf node, bool* valid);
	void deletevalueL(Index indexinfor, IndexLeaf &data);
	void deletevalueB(Index indexinfor, Branch& Dnode, IndexLeaf &data);
	list<IndexLeaf> whichDelete(Index indexinfor, string keyFrom, string keyTo, int& blockoffset);
	int indextoleaf(Index indexinfor, IndexLeaf data, Leaf& getleaf, list<IndexLeaf>::iterator &location);
	void changefather(Index indexinfor, Branch Tbranch);
	string minkey(Index indexinfor, Branch Tbranch);
};

int comparefi(int type, string key1, string key2);
int compare(int IntorString, int StringorFloat, string key1, string key2);
float StringtoFloat(string get);
int StringtoInt(string get);
void MemorytoBranch(char memory[], Branch& receptor);
void MemorytoLeaf(char memory[], Leaf& receptor);
int getPtr(int pos, char memory[]);