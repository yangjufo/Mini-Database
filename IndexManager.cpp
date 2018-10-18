#include"IndexManager.h"
extern int count1;
void Branch::insert(IndexBranch node) {
	recordNum++;
	list<IndexBranch>::iterator i = nodelist.begin();
	if (this->IntorString == 0) {
		if (this->StringorFloat == 1) {
			if (nodelist.size() == 0)
				nodelist.insert(i, node);
			else {
				for (i = nodelist.begin(); i != nodelist.end(); i++)
					if ((*i).key > node.key) break;
				nodelist.insert(i, node);
			}
		}
		else if (this->StringorFloat == 0) {
			if (nodelist.size() == 0)
				nodelist.insert(i, node);
			else {
				for (i = nodelist.begin(); i != nodelist.end(); i++)
					if (StringtoFloat((*i).key) > StringtoFloat(node.key)) break;
				nodelist.insert(i, node);
			}
		}
	}
	if (this->IntorString == 1) {
		if (nodelist.size() == 0)
			nodelist.insert(i, node);
		else {
			for (i = nodelist.begin(); i != nodelist.end(); i++)
				if (StringtoInt((*i).key) > StringtoInt(node.key)) break;
			nodelist.insert(i, node);
		}
	}
}

void IndexManager::changefather(Index indexinfor, Branch Tbranch) {
	int blockoffset;
	int blocknum;
	Branch getb;
	Leaf getl;
	Branch cb;
	Leaf cl;
	cb.createBranch(0, indexinfor.columnLength, indexinfor.type);
	cl.createLeaf(indexinfor.columnLength, indexinfor.type);
	getb.createBranch(0, indexinfor.columnLength, indexinfor.type);
	getl.createLeaf(indexinfor.columnLength, indexinfor.type);
	list<IndexBranch>::iterator i;
	int j = Tbranch.nodelist.size();
	int k;
	i = Tbranch.nodelist.begin();
	for (k = 1; k <= j; k++, i++) {
		getb = cb;
		getl = cl;
		blocknum = Buffer.malloc_block(indexinfor.fileoffset, (*i).ptrChild);
		Buffer.b[blocknum].isWritten = 1;
		//Buffer.b[blocknum].block[0] = 1;
		if (Buffer.b[blocknum].block[4091] == 'B') {
			MemorytoBranch(Buffer.b[blocknum].block, getb);
			getb.ptrFather = Tbranch.blockNum;
			translateBranch(Buffer.b[blocknum].block, getb);
		}
		else if (Buffer.b[blocknum].block[4091] == 'L') {
			MemorytoLeaf(Buffer.b[blocknum].block, getl);
			getl.ptrFather = Tbranch.blockNum;
			translateLeaf(Buffer.b[blocknum].block, getl);
		}
	}
	blocknum = Buffer.malloc_block(indexinfor.fileoffset, Tbranch.lastChild);
	Buffer.b[blocknum].isWritten = 1;
	getb = cb;
	getl = cl;
	if (Buffer.b[blocknum].block[4091] == 'B') {
		MemorytoBranch(Buffer.b[blocknum].block, getb);
		getb.ptrFather = Tbranch.blockNum;
		translateBranch(Buffer.b[blocknum].block, getb);
	}
	else if (Buffer.b[blocknum].block[4091] == 'L') {
		MemorytoLeaf(Buffer.b[blocknum].block, getl);
		getl.ptrFather = Tbranch.blockNum;
		translateLeaf(Buffer.b[blocknum].block, getl);
	}
}

IndexBranch Branch::pop() {
	recordNum--;
	IndexBranch tmpt = nodelist.back();
	nodelist.pop_back();
	return tmpt;
}

IndexBranch Branch::getfront() {
	return nodelist.front();
}

void Branch::clearall() {
	while (nodelist.size() != 0)
		nodelist.pop_back();
}

Branch::Branch(char memory[], int lengthOFcolumn, int type) {
	//this->isRoot = IsRoot;
	if (type == 11)
		this->StringorFloat = 0;
	else if (type == 12)
		this->StringorFloat = 1;
	this->columnLength = lengthOFcolumn;
	MemorytoBranch(memory, *this);
	int totallength = LENGTHOFPOINTER;
	totallength += this->columnLength;
	this->Max = 4081 / totallength;
}

void Leaf::insert(IndexLeaf node) {
	recordNum++;
	list<IndexLeaf>::iterator i = nodelist.begin();
	if (this->IntorString == 0) {
		if (this->StringorFloat == 1) {
			if (nodelist.size() == 0)
				nodelist.insert(i, node);
			else {
				for (i = nodelist.begin(); i != nodelist.end(); i++)
					if ((*i).key > node.key) break;
				nodelist.insert(i, node);
			}
		}
		else if (this->StringorFloat == 0) {
			if (nodelist.size() == 0)
				nodelist.insert(i, node);
			else {
				for (i = nodelist.begin(); i != nodelist.end(); i++)
					if (StringtoFloat((*i).key) > StringtoFloat(node.key)) break;
				nodelist.insert(i, node);
			}
		}
	}
	if (this->IntorString == 1) {
		if (nodelist.size() == 0) {
			nodelist.insert(i, node);
		}
		else {
			for (i = nodelist.begin(); i != nodelist.end(); i++)
				if (StringtoInt((*i).key) > StringtoInt(node.key)) break;
			nodelist.insert(i, node);
		}
	}
}

IndexLeaf Leaf::pop() {
	recordNum--;
	IndexLeaf tmpt = nodelist.back();
	nodelist.pop_back();
	return tmpt;
}

IndexLeaf Leaf::getfront() {//this is the smallest of all the keys of the list
	return nodelist.front();
}

void Leaf::clearall() {
	while (nodelist.size() != 0)
		nodelist.pop_back();
}

Leaf::Leaf(char memory[], int lengthOFcolumn, int type) {
	if (type == 11)
		this->StringorFloat = 0;
	else if (type == 12)
		this->StringorFloat = 1;
	int length = LENGTHOFPOINTER;
	this->columnLength = lengthOFcolumn;
	MemorytoLeaf(memory, *this);
	int totallength = this->columnLength + length;
	this->Max = 4078 / totallength;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void IndexManager::createIndex(Index& indexinfor, IndexColumn DataofIndex) {
	Branch root;
	root.createBranch(1, indexinfor.columnLength, indexinfor.type);
	/*string Data;
	int Nint = INT;
	int Nfloat = FLOAT;
	int Nchar = CHAR;
	int length = LENGTHOFPOINTER;*/
	int blocknum;
	int i;
	/*root.columnLength = indexinfor.columnLength;
	root.lastChild = Buffer.create_newblock_index(indexinfor.fileoffset);
	blocknum = Buffer.malloc_block(indexinfor.fileoffset, root.lastChild);
	InttoChar(indexinfor.fileoffset, 0, length, Buffer.b[blocknum]->block);
	InttoChar(0, 4, length, Buffer.b[blocknum]->block);
	InttoChar(0, 4087, length, Buffer.b[blocknum]->block);
	InttoChar(0, 4092, length, Buffer.b[blocknum]->block);
	if (tableinfor.attriNum == Nint)
	root.IntorString = 1;
	else if (tableinfor.attriNum == Nfloat || tableinfor.attriNum == Nchar)
	root.isRoot = 'R';
	root.recordNum = 0;
	blocknum = Buffer.malloc_block(indexinfor.fileoffset, indexinfor.blockoffset);
	translateBranch(Buffer.b[blocknum]->block, root);*/
	root.blockNum = indexinfor.blockoffset;
	for (i = 0; i < DataofIndex.rows.size(); i++)
		insertValue(root, DataofIndex.rows[i], indexinfor);
	blocknum = Buffer.malloc_block(indexinfor.fileoffset, indexinfor.blockoffset);
	Buffer.b[blocknum].isWritten = 1;
	translateBranch(Buffer.b[blocknum].block, root);
}

void IndexManager::translateLeaf(char neicun[], Leaf needt) {
	int i, length, totallength, k, j, keynumber, intlength;
	length = LENGTHOFPOINTER;
	//intlength = INT;
	totallength = 2 * length + needt.columnLength;
	InttoChar(needt.ptrFather, 0, length, neicun);
	InttoChar(needt.recordNum, 4, length, neicun);
	if (needt.IntorString == 1) {
		neicun[2 * length] = 'I';
		for (i = 0; i < needt.recordNum; i++) {
			InttoChar(needt.getfront().offsetinBlock, 2 * length + 1 + i*totallength, length, neicun);
			InttoChar(StringtoInt(needt.getfront().key), 2 * length + 1 + i*totallength + length, needt.columnLength, neicun);
			InttoChar(needt.getfront().offsetinFile, 2 * length + 1 + i*totallength + 2 * length, length, neicun);
			needt.writeclear();
		}
	}
	else {
		neicun[2 * length] = 'S';
		for (i = 0; i < needt.recordNum; i++) {
			InttoChar(needt.getfront().offsetinBlock, 2 * length + 1 + i*totallength, length, neicun);
			for (j = 2 * length + 1 + i*totallength + length, k = 0; k < needt.columnLength; k++, j++) {
				neicun[j] = needt.getfront().key[k];
			}
			InttoChar(needt.getfront().offsetinFile, 2 * length + 1 + i*totallength + length + needt.columnLength, length, neicun);
			needt.writeclear();
		}
	}
	InttoChar(needt.lastSibling, 4087, length, neicun);
	InttoChar(needt.nextSibling, 4092, length, neicun);
	neicun[4091] = 'L';
}

void IndexManager::translateBranch(char neicun[], Branch needt) {
	int i, length, totallength, k, j, keynumber, intlength;
	length = LENGTHOFPOINTER;
	//intlength = INT;
	if (needt.isRoot == 1)
		neicun[4090] = 'R';
	else
		neicun[4090] = 'N';
	totallength = length + needt.columnLength;
	InttoChar(needt.ptrFather, 0, length, neicun);
	InttoChar(needt.recordNum, 4, length, neicun);
	if (needt.IntorString == 1) {
		neicun[2 * length] = 'I';
		for (i = 0; i < needt.recordNum; i++) {
			InttoChar(needt.getfront().ptrChild, 2 * length + 1 + i*totallength, length, neicun);
			InttoChar(StringtoInt(needt.getfront().key), 2 * length + 1 + i*totallength + length, needt.columnLength, neicun);
			needt.writeclear();
		}
	}
	else {
		neicun[2 * length] = 'S';
		for (i = 0; i < needt.recordNum; i++) {
			InttoChar(needt.getfront().ptrChild, 2 * length + 1 + i*totallength, length, neicun);
			for (j = 2 * length + 1 + i*totallength + length, k = 0; k < needt.columnLength; k++, j++) {
				neicun[j] = needt.getfront().key[k];
				//needt.writeclear();
			}
			needt.writeclear();
		}
	}
	InttoChar(needt.lastChild, 4092, length, neicun);
	neicun[4091] = 'B';
}

void IndexManager::InttoChar(int number, int startpos, int length, char neicun[]) {
	int i, flag, str;
	str = startpos;
	if (number < 0) {
		number = -number;
		flag = -1;
	}
	else
		flag = 1;
	for (i = 0; i < length; i++) {
		startpos--;
		neicun[startpos + length] = number % 256;
		number /= 256;
	}
	if (flag == -1)
		neicun[str] = neicun[str] + 128;
}

int IndexManager::StringtoInt(string key) {
	int i, number;
	i = 0;
	number = 0;
	int flag = -1;
	if (key[0] == '-') {
		i++;
		flag = -1;
	}
	else
		flag = 1;
	for (; i < key.length(); i++) {
		number = number * 10 + key.at(i) - '0';
	}
	return number * flag;
}

string IndexManager::minkey(Index indexinfor, Branch Tbranch) {
	int next;
	int block;
	Branch tbranch;
	Branch cbranch;
	Leaf tleaf;
	tbranch.createBranch(0, indexinfor.columnLength, indexinfor.type);
	cbranch = tbranch;
	tleaf.createLeaf(indexinfor.columnLength, indexinfor.type);
	next = (*Tbranch.nodelist.begin()).ptrChild;
	block = Buffer.malloc_block(indexinfor.fileoffset, next);
	while (Buffer.b[block].block[4091] != 'L') {
		tbranch = cbranch;
		MemorytoBranch(Buffer.b[block].block, tbranch);
		next = (*tbranch.nodelist.begin()).ptrChild;
		block = Buffer.malloc_block(indexinfor.fileoffset, next);
	}
	MemorytoLeaf(Buffer.b[block].block, tleaf);
	return (*tleaf.nodelist.begin()).key;
}

void IndexManager::index_insertValue(Index indexinfor, IndexColumn data){
	IndexRow insertdata;
	int block, fileoffset;
	insertdata = data.rows[0];
	Branch root;
	root.createBranch(1, indexinfor.columnLength, indexinfor.type);
	fileoffset = indexinfor.blockoffset / 1000000;
	block = Buffer.malloc_block(fileoffset, indexinfor.blockoffset);
	MemorytoBranch(Buffer.b[block].block, root);
	insertValue(root, insertdata, indexinfor);
	translateBranch(Buffer.b[block].block, root);
	Buffer.b[block].isWritten = 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//注意检查最后一个lastchild！！转移，因为每个都要有才行
void IndexManager::insertValue(Branch& Searchnode, IndexRow dataRecord, Index& indexinfor) {
	//Branch root(1);
	bool *valid;
	valid = new bool;
	*valid = 0;
	int *ptrChild;
	ptrChild = new int;
	*ptrChild = 0;
	int Fp = indexinfor.blockoffset;
	IndexBranch tbranch1, tbranch2;
	Branch Tbranch1, Tbranch2;
	Tbranch1.createBranch(0, indexinfor.columnLength, indexinfor.type);
	Tbranch2.createBranch(0, indexinfor.columnLength, indexinfor.type);
	string formal;
	int i, block1, blockoffset1, block2, blockoffset2;
	list<IndexBranch>::iterator j;
	IndexLeaf readyinsert(dataRecord.key, dataRecord.fileoffset, dataRecord.blockoffset);
	insertvalue(Searchnode, readyinsert, indexinfor, Fp, valid, ptrChild, formal);
	if (*valid) {
		tbranch1.key = formal;
		tbranch1.ptrChild = *ptrChild;
		if (Searchnode.Max > Searchnode.recordNum) {
			Searchnode.insert(tbranch1);
		}
		else if (Searchnode.Max == Searchnode.recordNum) {//lastchild可能有变动，维护好lastchild
			j = Searchnode.nodelist.begin();
			block1 = Buffer.create_newblock_index(indexinfor);
			blockoffset1 = Buffer.malloc_block(indexinfor.fileoffset, block1);
			block2 = Buffer.create_newblock_index(indexinfor);
			blockoffset2 = Buffer.malloc_block(indexinfor.fileoffset, block2);
			Buffer.b[blockoffset1].isWritten = 1;
			Buffer.b[blockoffset2].isWritten = 1;
			for (i = 1; i <= Searchnode.Max / 2 + 1; i++, j++) {
				Tbranch1.insert((*j));
			}
			for (; i <= Searchnode.Max; i++, j++) {
				Tbranch2.insert((*j));
			}
			if (comparefi(indexinfor.type, (*Tbranch2.nodelist.begin()).key, tbranch1.key) > 0) {
				Tbranch1.insert(tbranch1);
			}
			else
				Tbranch2.insert(tbranch1);
			Tbranch1.lastChild = Tbranch1.pop().ptrChild;//维护lastchild
			Tbranch2.lastChild = Searchnode.lastChild;
			j = Tbranch2.nodelist.begin();
			tbranch2.key = minkey(indexinfor, Tbranch2);
			tbranch2.ptrChild = block1;
			Searchnode.clearall();
			Searchnode.insert(tbranch2);
			Searchnode.lastChild = block2;
			Tbranch1.ptrFather = indexinfor.blockoffset;
			Tbranch2.ptrFather = indexinfor.blockoffset;
			translateBranch(Buffer.b[blockoffset1].block, Tbranch1);
			translateBranch(Buffer.b[blockoffset2].block, Tbranch2);
			Tbranch1.blockNum = block1;
			Tbranch2.blockNum = block2;
			//接下来每个孩子的ptrchild都要改成当前的值
			/*for (int w = 1000001; w <= 1000030; w++) {
			int xy = 1;
			Leaf Tleaf;
			Tleaf.createLeaf(indexinfor.columnLength, indexinfor.type);
			int d = Buffer.malloc_block(xy, w);
			MemorytoLeaf(Buffer.b[d].block, Tleaf);
			}*/
			changefather(indexinfor, Tbranch1);
			changefather(indexinfor, Tbranch2);
			Searchnode.recordNum = 1;
		}
	}
	//int block;
	//block = Buffer.malloc_block(indexinfor.fileoffset, indexinfor.blockoffset);
	//MemorytoBranch(Buffer.b[block]->block, root);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void IndexManager::insertvalue(Branch& Searchnode, IndexLeaf& data, Index& indexinfor, int Fp, bool* valid, int* ptrChild, string& ckey) {//这个valid是说我现在返回的多的这个节点是否是有效的
	int block1, block2, blockoffset1, blockoffset2, blockoffsetnext, i;
	int *nextptrch;
	nextptrch = new int;
	*nextptrch = 0;
	int Rmax = Searchnode.Max / 2;
	int fp;
	string nkey;
	bool *last;
	bool *next;
	last = new bool;
	next = new bool;
	*last = 1;
	*next = 0;
	Leaf Tleaf1, Tleaf2;
	IndexBranch tbranch1, tbranch2;
	Branch Tbranch1, Tbranch2, Tbranch3;
	Tbranch1.createBranch(0, indexinfor.columnLength, indexinfor.type);
	Tbranch2.createBranch(0, indexinfor.columnLength, indexinfor.type);
	Tbranch3.createBranch(0, indexinfor.columnLength, indexinfor.type);
	list<IndexBranch>::iterator j;
	list<IndexBranch>::iterator location;
	//int typeint, typechar, typefloat;
	//typeint = INT;
	//typechar = CHAR;
	//typefloat = FLOAT;
	if (Searchnode.isRoot == 1 && Searchnode.lastChild == 0) {
		block1 = Buffer.create_newblock_index(indexinfor);
		//tbranch1.key = data.key;
		//tbranch1.ptrChild = block1;
		indexinfor.fileoffset = block1 / MAX_NUMBER_BLOCK;
		blockoffset1 = Buffer.malloc_block(indexinfor.fileoffset, block1);

		Tleaf1.createLeaf(indexinfor.columnLength, indexinfor.type);
		Tleaf1.insert(data);
		Tleaf1.ptrFather = indexinfor.blockoffset;
		Buffer.b[blockoffset1].isWritten = 1;
		translateLeaf(Buffer.b[blockoffset1].block, Tleaf1);
		//blockoffset1 = Buffer.malloc_block(indexinfor.fileoffset, indexinfor.blockoffset);
		Searchnode.lastChild = block1;
		//translateBranch(Buffer.b[blockoffset1].block, Searchnode);

	}
	else {
		location = SearchlocaB(Searchnode, data, next);
		if (*next)
			blockoffset1 = (*location).ptrChild;
		else
			blockoffset1 = Searchnode.lastChild;
		indexinfor.fileoffset = blockoffset1 / 1000000;
		block1 = Buffer.malloc_block(indexinfor.fileoffset, blockoffset1);
		Buffer.b[block1].isWritten = 1;
		if (Buffer.b[block1].block[4091] == 'B') {
			Tbranch1.createBranch(0, indexinfor.columnLength, indexinfor.type);
			MemorytoBranch(Buffer.b[block1].block, Tbranch1);
			Tbranch1.blockNum = blockoffset1;
			fp = Tbranch1.blockNum;
			insertvalue(Tbranch1, data, indexinfor, fp, next, nextptrch, nkey);
			if (*(next)) {
				tbranch1.key = nkey;
				tbranch1.ptrChild = *nextptrch;
				if (Tbranch1.Max > Tbranch1.recordNum) {
					*valid = 0;
					Tbranch1.insert(tbranch1);
					translateBranch(Buffer.b[block1].block, Tbranch1);
				}
				else if (Tbranch1.Max == Tbranch1.recordNum) {
					/*if (Searchnode.isRoot == 1) {
					*valid = 1;
					*ptrChild = *next;
					ckey = nkey;*/
					/*j = Searchnode.nodelist.begin();
					block1 = Buffer.create_newblock_index(indexinfor);
					blockoffset1 = Buffer.malloc_block(indexinfor.fileoffset, block1);
					block2 = Buffer.create_newblock_index(indexinfor);
					blockoffset2 = Buffer.malloc_block(indexinfor.fileoffset, block2);
					for (i = 1; i <= Searchnode.Max / 2 + 1; i++, j++) {
					Tbranch1.insert((*j));
					}
					for (; i <= Searchnode.Max; i++, j++) {
					Tbranch2.insert((*j));
					}
					Tbranch2.insert(tbranch1);
					j = Tbranch2.nodelist.begin();
					tbranch2.key = (*j).key;
					tbranch2.ptrChild = (*j).ptrChild;
					Searchnode.clearall();
					Searchnode.insert(tbranch2);
					Searchnode.lastChild = block2;*/
					/*}
					else {
					*valid = 1;
					for (int k = 1; k <= Rmax; k++) {
					tbranch2 = Searchnode.pop();
					Tbranch1.insert(tbranch2);
					}
					Tbranch1.insert(tbranch1);
					j = Tbranch1.nodelist.begin();
					*ptrChild = (*j).ptrChild;
					ckey = (*j).key;
					}*/
					j = Tbranch1.nodelist.begin();
					block2 = Buffer.create_newblock_index(indexinfor);
					blockoffset2 = Buffer.malloc_block(indexinfor.fileoffset, block2);
					Buffer.b[blockoffset2].isWritten = 1;
					for (i = 1; i <= Tbranch1.Max / 2 + 1; i++) {
						j = Tbranch1.nodelist.begin();
						Tbranch2.insert((*j));
						Tbranch1.nodelist.pop_front();
						Tbranch1.recordNum--;
					}
					if (comparefi(indexinfor.type, (*Tbranch2.nodelist.begin()).key, tbranch1.key) > 0) {
						Tbranch1.insert(tbranch1);
					}
					else
						Tbranch2.insert(tbranch1);
					//Tbranch1.lastChild = Tbranch1.pop().ptrChild;//维护lastchild
					Tbranch2.lastChild = Tbranch2.pop().ptrChild;
					j = Tbranch2.nodelist.begin();
					*valid = 1;
					*ptrChild = block2;
					ckey = minkey(indexinfor, Tbranch1);
					Tbranch2.ptrFather = Tbranch1.ptrFather;//给父亲赋值
					translateBranch(Buffer.b[block1].block, Tbranch1);
					translateBranch(Buffer.b[blockoffset2].block, Tbranch2);
					Tbranch2.blockNum = block2;
					//接下来每个孩子的ptrchild都要改成当前的值
					//changefather(indexinfor, Tbranch1);
					changefather(indexinfor, Tbranch2);
				}
			}
			else
				*valid = 0;
		}
		else if (Buffer.b[block1].block[4091] == 'L') {
			Tleaf1.createLeaf(indexinfor.columnLength, indexinfor.type);
			MemorytoLeaf(Buffer.b[block1].block, Tleaf1);
			Tleaf1.blockNum = blockoffset1;
			insertvalueL(Tleaf1, data, indexinfor, next, nextptrch, nkey);
			translateLeaf(Buffer.b[block1].block, Tleaf1);
			Buffer.b[block1].isWritten = 1;
			if (*(next)) {
				*valid = 1;
				*ptrChild = *nextptrch;
				ckey = nkey;
			}
			else
				*valid = 0;
		}
	}
	/*else if (Searchnode.Max == Searchnode.recordNum) {
	if (Searchnode.isRoot == 1) {
	j = Searchnode.nodelist.begin();
	block1 = Buffer.create_newblock_index(indexinfor.fileoffset);
	blockoffset1 = Buffer.malloc_block(indexinfor.fileoffset, block1);
	block2 = Buffer.create_newblock_index(indexinfor.fileoffset);
	blockoffset2 = Buffer.malloc_block(indexinfor.fileoffset, block2);
	for (i = 1; i < Searchnode.Max / 2; i++, j++) {
	Tbranch1.insert((*j));
	}
	tbranch1.key = (*j).key;
	tbranch1.ptrChild = block1;
	if (comparefi(indexinfor.type, (*j).key, data.key) > 0) {
	//insertvalue()
	}
	for (; i <= Searchnode.Max; i++, j++) {
	Tbranch2.insert((*j));
	}
	Searchnode.clearall();
	Searchnode.insert(tbranch1);
	Searchnode.lastChild = block2;
	}
	else {
	*valid = 1;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	}
	}
	else {
	*valid = 0;
	list<IndexBranch>::iterator i = SearchlocaB(Searchnode, data, last);
	if (*last)
	blockoffsetnext = Searchnode.lastChild;
	else blockoffsetnext = (*i).ptrChild;
	block1 = Buffer.malloc_block(indexinfor.fileoffset, blockoffsetnext);
	if (Buffer.b[block1]->block[4091] == 'B') {
	MemorytoBranch(Buffer.b[block1]->block, Tbranch1);

	}
	else if (Buffer.b[block1]->block[4091] == 'L') {
	MemorytoLeaf(Buffer.b[block1]->block, Tleaf1);
	if (Tleaf1.Max == Tleaf1.recordNum) {
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	}
	else {
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	}
	}
	(*i).ptrChild;
	}*/
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void IndexManager::insertvalueL(Leaf& SearchnodeL, IndexLeaf& data, Index& indexinfor, bool* valid, int* prrChild, string& ckey) {//有分裂的话valid变成1返回地址
	int blockoffset;
	int blocknum, blocknum1;
	Leaf tleaf, ztleaf;
	Leaf tranleaf;
	IndexLeaf get;
	int trans = SearchnodeL.Max / 2;
	if (SearchnodeL.Max == SearchnodeL.recordNum) {
		*valid = 1;
		tleaf.columnLength = indexinfor.columnLength;
		tleaf.IntorString = SearchnodeL.IntorString;
		tleaf.StringorFloat = SearchnodeL.StringorFloat;
		tleaf.ptrFather = SearchnodeL.ptrFather;
		blockoffset = Buffer.create_newblock_index(indexinfor);
		blocknum = Buffer.malloc_block(indexinfor.fileoffset, blockoffset);
		tleaf.nextSibling = SearchnodeL.nextSibling;
		tleaf.lastSibling = blockoffset;
		SearchnodeL.nextSibling = SearchnodeL.blockNum;
		SearchnodeL.lastSibling = SearchnodeL.lastSibling;
		for (int i = 0; i < trans; i++) {
			get = SearchnodeL.pop();
			tleaf.insert(get);
		}
		//tleaf.insert(data);
		if (comparefi(indexinfor.type, (*tleaf.nodelist.begin()).key, data.key)>0)
			SearchnodeL.insert(data);
		else
			tleaf.insert(data);
		ckey = (*(tleaf.nodelist.begin())).key;
		*prrChild = blockoffset;
		Buffer.b[blocknum].isWritten = 1;
		tranleaf = tleaf;
		tleaf = SearchnodeL;
		SearchnodeL = tranleaf;
		SearchnodeL.recordNum = SearchnodeL.nodelist.size();//注意维护recordnum!!
		if (tleaf.lastSibling != 0) {
			blocknum1 = Buffer.malloc_block(indexinfor.fileoffset, tleaf.lastSibling);
			Buffer.b[blocknum1].isWritten = 1;
			ztleaf.createLeaf(indexinfor.columnLength, indexinfor.type);
			MemorytoLeaf(Buffer.b[blocknum1].block, ztleaf);
			ztleaf.nextSibling = blockoffset;
			translateLeaf(Buffer.b[blocknum1].block, ztleaf);
		}
		translateLeaf(Buffer.b[blocknum].block, tleaf);
		Buffer.b[blocknum].isWritten = 1;
	}
	else {
		*valid = 0;
		SearchnodeL.insert(data);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
IndexColumn IndexManager::selectEqual(Index indexinfor, string key/*, int blockoffset*/) {
	IndexColumn Result;
	IndexRow result;
	int tempblockoffset, nextblock;
	bool *valid;
	valid = new bool;
	*valid = 1;
	list<IndexBranch>::iterator i;
	list<IndexLeaf>::iterator j;
	Branch tbranch, ctbranch;
	Leaf tleaf;
	tbranch.createBranch(0, indexinfor.columnLength, indexinfor.type);
	ctbranch = tbranch;
	tleaf.createLeaf(indexinfor.columnLength, indexinfor.type);
	IndexLeaf get;
	get.key = key;
	tempblockoffset = Buffer.malloc_block(indexinfor.fileoffset, indexinfor.blockoffset);
	//MemorytoBranch(Buffer.b[tempblockoffset]->block, tbranch);
	while (Buffer.b[tempblockoffset].block[4091] != 'L') {
		tbranch = ctbranch;
		MemorytoBranch(Buffer.b[tempblockoffset].block, tbranch);
		i = SearchlocaB(tbranch, get, valid);
		if (*valid)
			nextblock = (*i).ptrChild;
		else
			nextblock = tbranch.lastChild;
		tempblockoffset = Buffer.malloc_block(indexinfor.fileoffset, nextblock);
	}
	MemorytoLeaf(Buffer.b[tempblockoffset].block, tleaf);
	j = SearchlocaL(tleaf, get, valid);
	if (*valid) {
		result.blockoffset = (*j).offsetinBlock;
		result.fileoffset = (*j).offsetinFile;
		result.key = (*j).key;
	}
	else
		result.key = "";
	Result.rows.push_back(result);
	return Result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
IndexColumn IndexManager::selectBetween(Index indexinfor, string keyFrom, string keyTo/*, int& blockoffset*/) {//如果一个结果也没有返回值无效那么把blockoffset置为-1！！！
	IndexColumn Result;
	IndexRow resultR;
	int blockoffsetR;
	int tempblockoffset, nextblock;
	bool *valid;
	valid = new bool;
	*valid = 1;
	list<IndexBranch>::iterator i;
	list<IndexLeaf>::iterator j;
	Branch tbranch, ctbranch;
	Leaf tleaf, ctleaf;
	tbranch.createBranch(0, indexinfor.columnLength, indexinfor.type);
	ctbranch = tbranch;
	tleaf.createLeaf(indexinfor.columnLength, indexinfor.type);
	ctleaf.createLeaf(indexinfor.columnLength, indexinfor.type);
	IndexLeaf get;
	get.key = keyFrom;
	tempblockoffset = Buffer.malloc_block(indexinfor.fileoffset, indexinfor.blockoffset);
	//MemorytoBranch(Buffer.b[tempblockoffset]->block, tbranch);
	while (Buffer.b[tempblockoffset].block[4091] != 'L') {
		tbranch = ctbranch;
		MemorytoBranch(Buffer.b[tempblockoffset].block, tbranch);
		i = SearchlocaB(tbranch, get, valid);
		if (*valid)
			nextblock = (*i).ptrChild;
		else
			nextblock = tbranch.lastChild;
		tempblockoffset = Buffer.malloc_block(nextblock, nextblock);
	}
	MemorytoLeaf(Buffer.b[tempblockoffset].block, tleaf);
	j = SearchlocaLM(tleaf, get, valid);
	if (!(*valid)) {
		blockoffsetR = (*i).ptrChild;
		tempblockoffset = Buffer.malloc_block(indexinfor.blockoffset, tleaf.nextSibling);
		MemorytoLeaf(Buffer.b[tempblockoffset].block, tleaf);
		j = tleaf.nodelist.begin();
	}
	while (comparefi(indexinfor.type, keyTo, (*j).key) > 0) {
		if (comparefi(indexinfor.type, keyFrom, (*j).key) < 0) {
			resultR.blockoffset = (*j).offsetinBlock;
			resultR.fileoffset = (*j).offsetinFile;
			resultR.key = (*j).key;
			Result.rows.push_back(resultR);
		}
		j++;
		if (j == tleaf.nodelist.end()) {
			if (tleaf.nextSibling == 0)
				break;
			tempblockoffset = Buffer.malloc_block(indexinfor.blockoffset, tleaf.nextSibling);
			tleaf = ctleaf;
			MemorytoLeaf(Buffer.b[tempblockoffset].block, tleaf);
			j = tleaf.nodelist.begin();
		}
	}
	return Result;
}

list<IndexLeaf> IndexManager::whichDelete(Index indexinfor, string keyFrom, string keyTo, int& blockoffset) {//如果一个结果也没有返回值无效那么把blockoffset置为-1！！！
	list<IndexLeaf> fi;
	int blockoffsetR;
	int tempblockoffset, nextblock;
	bool *valid;
	valid = new bool;
	*valid = 1;
	list<IndexBranch>::iterator i;
	list<IndexLeaf>::iterator j;
	Branch tbranch;
	Leaf tleaf;
	IndexLeaf get;
	get.key = keyFrom;
	tempblockoffset = Buffer.malloc_block(indexinfor.fileoffset, indexinfor.blockoffset);
	//MemorytoBranch(Buffer.b[tempblockoffset]->block, tbranch);
	while (Buffer.b[tempblockoffset].block[4091] != 'L') {
		MemorytoBranch(Buffer.b[tempblockoffset].block, tbranch);
		i = SearchlocaB(tbranch, get, valid);
		if (*valid)
			nextblock = (*i).ptrChild;
		else
			nextblock = tbranch.lastChild;
		tempblockoffset = Buffer.malloc_block(nextblock, indexinfor.blockoffset);
	}
	MemorytoLeaf(Buffer.b[tempblockoffset].block, tleaf);
	j = SearchlocaLM(tleaf, get, valid);
	if (!(*valid)) {
		blockoffsetR = (*i).ptrChild;
		tempblockoffset = Buffer.malloc_block(indexinfor.blockoffset, tleaf.nextSibling);
		MemorytoLeaf(Buffer.b[tempblockoffset].block, tleaf);
		j = tleaf.nodelist.begin();
	}
	while (comparefi(indexinfor.type, keyTo, (*j).key) >= 0) {
		if (comparefi(indexinfor.type, keyFrom, (*j).key) < 0)
			fi.push_back((*j));
		if (j == tleaf.nodelist.end()) {
			tempblockoffset = Buffer.malloc_block(indexinfor.blockoffset, tleaf.nextSibling);
			MemorytoLeaf(Buffer.b[tempblockoffset].block, tleaf);
			j = tleaf.nodelist.begin();
		}
		else
			j++;
	}
	return fi;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void IndexManager::deleteValue(Index indexinfor, IndexColumn dc/*, int& blockOffset*/) {
	list<IndexLeaf> deletevalues;
	int blockoffset;
	IndexLeaf temp;
	for (int i = 0; i < dc.rows.size(); i++) {
		temp.key = dc.rows[i].key;
		deletevalues.push_back(temp);
	}
	//deletevalues = whichDelete(indexinfor, keyFrom, keyTo, blockoffset);
	//if (blockoffset != -1) {
	while (deletevalues.size() != 0) {
		temp = *(deletevalues.begin());
		deletevalues.pop_front();
		deletevalueL(indexinfor, temp);
	}
	//}
}

int IndexManager::indextoleaf(Index indexinfor, IndexLeaf data, Leaf& getleaf, list<IndexLeaf>::iterator &location) {
	int blockoffsetR;
	int tempblockoffset, nextblock;
	bool *valid;
	valid = new bool;
	//*valid = 1;
	list<IndexBranch>::iterator i;
	//list<IndexLeaf>::iterator j;
	Branch tbranch, cbranch;
	tbranch.createBranch(0, indexinfor.columnLength, indexinfor.type);
	cbranch = tbranch;
	//IndexLeaf get;
	//get.key = key;
	tempblockoffset = Buffer.malloc_block(indexinfor.fileoffset, indexinfor.blockoffset);
	//MemorytoBranch(Buffer.b[tempblockoffset]->block, tbranch);
	while (Buffer.b[tempblockoffset].block[4091] != 'L') {
		tbranch = cbranch;
		MemorytoBranch(Buffer.b[tempblockoffset].block, tbranch);
		i = SearchlocaB(tbranch, data, valid);
		if (*valid)
			nextblock = (*i).ptrChild;
		else
			nextblock = tbranch.lastChild;
		tempblockoffset = Buffer.malloc_block(nextblock, nextblock);
	}
	MemorytoLeaf(Buffer.b[tempblockoffset].block, getleaf);
	location = SearchlocaL(getleaf, data, valid);
	//return blockoffsetR;
	return  tempblockoffset;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void IndexManager::deletevalueL(Index indexinfor, IndexLeaf &data) {
	int blockoffset;
	int blockoffsetofleaf;
	int blocknum;
	bool*valid;
	valid = new bool;
	//IndexLeaf temp;
	Branch Father, Root;
	Branch cb;
	cb.createBranch(0, indexinfor.columnLength, indexinfor.type);
	IndexLeaf rin;
	IndexRow total;
	Leaf Dnode;
	list<IndexLeaf>::iterator location;
	Dnode.createLeaf(indexinfor.columnLength, indexinfor.type);
	Father.createBranch(0, indexinfor.columnLength, indexinfor.type);
	Root.createBranch(1, indexinfor.columnLength, indexinfor.type);
	blockoffsetofleaf = indextoleaf(indexinfor, data, Dnode, location);
	list<IndexBranch>::iterator i;
	if (Dnode.recordNum <= Dnode.Max / 2) {
		Dnode.nodelist.erase(location);
		blocknum = Buffer.malloc_block(indexinfor.fileoffset, Dnode.ptrFather);
		MemorytoBranch(Buffer.b[blocknum].block, Father);
		i = SearchlocaB(Father, data, valid);
		if (*valid) {
			Father.nodelist.erase(i);
		}
		else {
			Father.lastChild = (*i).ptrChild;
			Father.nodelist.erase(i);
		}
		Father.recordNum = Father.nodelist.size();
		translateBranch(Buffer.b[blocknum].block, Father);
		Buffer.b[blocknum].isWritten = 1;
		blocknum = Buffer.malloc_block(indexinfor.fileoffset, indexinfor.blockoffset);
		MemorytoBranch(Buffer.b[blocknum].block, Root);
		Buffer.b[blocknum].isWritten = 1;
		while (Dnode.nodelist.size() > 0) {
			rin = Dnode.pop();
			total.fileoffset = rin.offsetinBlock;
			total.blockoffset = rin.offsetinBlock;
			total.key = rin.key;
			insertValue(Root, total, indexinfor);
		}
		blocknum = Buffer.malloc_block(indexinfor.fileoffset, Dnode.ptrFather);
		Father = cb;
		MemorytoBranch(Buffer.b[blocknum].block, Father);
		if (Father.recordNum < Father.Max / 2) {
			Father.blockNum = Dnode.ptrFather;
			deletevalueB(indexinfor, Father, data);
			translateBranch(Buffer.b[blocknum].block, Father);
		}
	}
	else {
		Dnode.nodelist.erase(location);
		Dnode.recordNum--;
		Buffer.b[blockoffsetofleaf].isWritten = 1;
		translateLeaf(Buffer.b[blockoffsetofleaf].block, Dnode);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void IndexManager::deletevalueB(Index indexinfor, Branch& Dnode, IndexLeaf &data) {
	bool*valid;
	int ayi;
	int zuihougaibianb;
	Branch zuihouB;
	zuihouB.createBranch(0, indexinfor.columnLength, indexinfor.type);
	valid = new bool;
	int blocknum, Ablocknum;
	int blockoffset;
	Branch Father, Brother;
	Father.createBranch(0, indexinfor.columnLength, indexinfor.type);
	Brother = Father;
	IndexBranch temp;
	list<IndexBranch>::iterator i, j;
	if (!(Dnode.isRoot)) {
		blocknum = Buffer.malloc_block(indexinfor.fileoffset, Dnode.ptrFather);
		Buffer.b[blocknum].isWritten = 1;
		MemorytoBranch(Buffer.b[blocknum].block, Father);
		Father.blockNum = Dnode.ptrFather;
		i = SearchlocaB(Father, data, valid);//找到这个位置，如果旁边的孩子有空位删掉之后插进去，如果没有，借一个然后插入就不需要删掉了。最后如果是删了的情况就检查现在这个Father符合情况不，不符合就递归进行。
		j = i;
		if (*valid) {//不是lastchild的情况
			i++;
			Ablocknum = Buffer.malloc_block(indexinfor.fileoffset, (*i).ptrChild);
			Buffer.b[Ablocknum].isWritten = 1;
			MemorytoBranch(Buffer.b[Ablocknum].block, Brother);
			if (Brother.recordNum + Dnode.nodelist.size() <= Brother.Max) {
				temp.ptrChild = Dnode.lastChild;
				temp.key = minkey(indexinfor, Brother);
				Brother.insert(temp);
				while (Dnode.nodelist.size() != 0) {
					temp = Dnode.pop();
					Brother.insert(temp);
				}
				translateBranch(Buffer.b[Ablocknum].block, Brother);
				Brother.blockNum = (*i).ptrChild;
				changefather(indexinfor, Brother);
				Father.nodelist.erase(j);
				Father.recordNum--;
				deletevalueB(indexinfor, Father, data);
				translateBranch(Buffer.b[blocknum].block, Father);
			}
			else {
				temp = Brother.getfront();
				Brother.nodelist.pop_front();
				Brother.recordNum--;
				ayi = Dnode.lastChild;
				Dnode.lastChild = temp.ptrChild;
				temp.ptrChild = ayi;
				Dnode.insert(temp);
				temp = Brother.getfront();
				(*i).key = minkey(indexinfor, Brother);
				//改变转移的那一个节点的father
				changefather(indexinfor, Dnode);
				translateBranch(Buffer.b[Ablocknum].block, Brother);
				translateBranch(Buffer.b[blocknum].block, Father);
				//translateBranch(Buffer.b[Ablocknum]->block, Brother);
			}
		}
		else {
			i = Father.nodelist.end()--;
			Ablocknum = Buffer.malloc_block(indexinfor.fileoffset, (*i).ptrChild);//现在的i就是lastch前面一个，正好
			Buffer.b[Ablocknum].isWritten = 1;
			MemorytoBranch(Buffer.b[Ablocknum].block, Brother);//这个brother是前一个brother
			if (Brother.recordNum + Dnode.nodelist.size() <= Brother.Max) {
				temp.ptrChild = Brother.lastChild;
				temp.key = minkey(indexinfor, Dnode);
				Brother.lastChild = Dnode.lastChild;
				Brother.insert(temp);
				while (Dnode.nodelist.size() != 0) {
					temp = Dnode.pop();
					Brother.insert(temp);
				}
				Brother.blockNum = (*i).ptrChild;
				changefather(indexinfor, Brother);
				translateBranch(Buffer.b[Ablocknum].block, Brother);
				Father.lastChild = (*i).ptrChild;
				Father.nodelist.erase(i);
				Father.recordNum--;
				deletevalueB(indexinfor, Father, data);
				translateBranch(Buffer.b[blocknum].block, Father);
			}
			else {
				temp = Brother.pop();
				Dnode.insert(temp);
				temp = Dnode.getfront();
				(*i).key = minkey(indexinfor, Dnode);
				//改变父亲
				changefather(indexinfor, Dnode);
				translateBranch(Buffer.b[Ablocknum].block, Brother);
				translateBranch(Buffer.b[blocknum].block, Father);
				//translateBranch(Buffer.b[Ablocknum]->block, Brother);
			}
		}
	}
}

list<IndexLeaf>::iterator IndexManager::SearchlocaL(Leaf& searched, IndexLeaf node, bool* valid) {
	*valid = 1;
	int j;
	list<IndexLeaf>::iterator i = searched.nodelist.begin();
	if (searched.IntorString == 0) {
		if (searched.StringorFloat == 1) {
			if (searched.nodelist.size() == 0) {
				*valid = 0;
				return i;
			}
			else {
				for (i = searched.nodelist.begin(); i != searched.nodelist.end(); i++)
					if ((*i).key == node.key) break;

				if (i == searched.nodelist.end())
					*valid = 0;
				else
					*valid = 1;
				return i;
			}
		}
		else if (searched.StringorFloat == 0) {
			if (searched.nodelist.size() == 0) {
				*valid = 0;
				return i;
			}
			else {
				for (i = searched.nodelist.begin(); i != searched.nodelist.end(); i++)
					if (StringtoFloat((*i).key) == StringtoFloat(node.key)) break;
				if (i == searched.nodelist.end())
					*valid = 0;
				else
					*valid = 1;
				return i;
			}
		}
	}
	if (searched.IntorString == 1) {
		if (searched.nodelist.size() == 0) {
			*valid = 0;
			return i;
		}
		else {
			for (i = searched.nodelist.begin(); i != searched.nodelist.end(); i++)
				if (StringtoInt((*i).key) == StringtoInt(node.key)) break;
			if (i == searched.nodelist.end())
				*valid = 0;
			else
				*valid = 1;
			return i;
		}
	}
}

list<IndexLeaf>::iterator IndexManager::SearchlocaLM(Leaf& searched, IndexLeaf node, bool* valid) {
	*valid = 1;
	int j;
	list<IndexLeaf>::iterator i = searched.nodelist.begin();
	if (searched.IntorString == 0) {
		if (searched.StringorFloat == 1) {
			if (searched.nodelist.size() == 0) {
				*valid = 0;
				return i;
			}
			else {
				for (i = searched.nodelist.begin(); i != searched.nodelist.end(); i++)
					if ((*i).key > node.key) break;
				if (i == searched.nodelist.end())
					*valid = 0;
				else
					*valid = 1;
				return i;
			}
		}
		else if (searched.StringorFloat == 0) {
			if (searched.nodelist.size() == 0) {
				*valid = 0;
				return i;
			}
			else {
				for (i = searched.nodelist.begin(); i != searched.nodelist.end(); i++)
					if (StringtoFloat((*i).key) > StringtoFloat(node.key)) break;
				if (i == searched.nodelist.end())
					*valid = 0;
				else
					*valid = 1;
				return i;
			}
		}
	}
	if (searched.IntorString == 1) {
		if (searched.nodelist.size() == 0) {
			*valid = 0;
			return i;
		}
		else {
			for (i = searched.nodelist.begin(); i != searched.nodelist.end(); i++)
				if (StringtoInt((*i).key) > StringtoInt(node.key)) break;
			if (i == searched.nodelist.end())
				*valid = 0;
			else
				*valid = 1;
			return i;
		}
	}
}

list<IndexBranch>::iterator IndexManager::SearchlocaB(Branch& searched, IndexLeaf node, bool* valid) {
	list<IndexBranch>::iterator i = searched.nodelist.begin();
	if (searched.IntorString == 0) {
		if (searched.StringorFloat == 1) {
			if (searched.nodelist.size() == 0) {
				*valid = 0;
				return i;
			}
			else {
				for (i = searched.nodelist.begin(); i != searched.nodelist.end(); i++)
					if ((*i).key > node.key)break;
				if (i == searched.nodelist.end())
					*valid = 0;
				else
					*valid = 1;
				return i;
			}
		}
		else if (searched.StringorFloat == 0) {
			if (searched.nodelist.size() == 0) {
				*valid = 0;
				return i;
			}
			else {
				for (i = searched.nodelist.begin(); i != searched.nodelist.end(); i++)
					if (StringtoFloat((*i).key) > StringtoFloat(node.key)) break;
				if (i == searched.nodelist.end())
					*valid = 0;
				else
					*valid = 1;
				return i;
			}
		}
	}
	if (searched.IntorString == 1) {
		if (searched.nodelist.size() == 0) {
			*valid = 0;
			return i;
		}
		else {
			for (i = searched.nodelist.begin(); i != searched.nodelist.end(); i++)
				if (StringtoInt((*i).key) > StringtoInt(node.key)) break;
			if (i == searched.nodelist.end())
				*valid = 0;
			else
				*valid = 1;
			return i;
		}
	}
}

string getvalues(int startpos, int endpos, char block[]) {
	char temp[4096];
	int i, j;
	for (i = startpos, j = 0; i <= endpos; i++, j++)
		temp[j] = block[i];
	temp[j] = '\0';//warning!
	string result(temp);
	return result;
}

int getPtr(int pos, char memory[]) {
	int ptr = 0;
	int i, length;
	int temp;
	int fuhao;
	length = LENGTHOFPOINTER;
	length += pos;
	if (memory[pos] < 0) {
		fuhao = -1;
		ptr = memory[pos] + 128;
	}
	else {
		fuhao = 1;
		ptr = memory[pos];
	}
	for (i = pos + 1; i < length; i++) {
		if (memory[i] < 0)
			temp = 256 + memory[i];
		else
			temp = memory[i];
		ptr = ptr * 256 + temp;
	}
	return ptr * fuhao;
}

void MemorytoLeaf(char memory[], Leaf& receptor) {
	int length, totallength, turn;
	char turn1[50];
	int i;
	int NumRecord;
	int j;
	IndexLeaf temp;
	length = LENGTHOFPOINTER;
	totallength = 2 * length + receptor.columnLength;
	receptor.ptrFather = getPtr(0, memory);
	receptor.recordNum = 0;
	NumRecord = getPtr(length, memory);
	receptor.recordNum = 0;
	receptor.lastSibling = getPtr(4087, memory);
	receptor.nextSibling = getPtr(4092, memory);
	if (memory[2 * length] == 'I') {
		receptor.IntorString = 1;
		for (i = 0, j = NumRecord - 1; i < NumRecord; i++, j--) {
			temp.offsetinBlock = getPtr(2 * length + 1 + j*totallength, memory);
			turn = getPtr(3 * length + 1 + j*totallength, memory);
			sprintf(turn1, "%d", turn);
			temp.key = turn1;
			temp.offsetinFile = getPtr(4 * length + 1 + j * totallength, memory);
			receptor.insert(temp);
		}
	}
	else if (memory[2 * length] == 'S') {
		receptor.IntorString = 0;
		for (i = 0; i < NumRecord; i++) {
			temp.offsetinBlock = getPtr(2 * length + 1 + i*totallength, memory);
			temp.key = getvalues(3 * length + 1 + i*totallength, 3 * length + 1 + i*totallength + receptor.columnLength - 1, memory);
			temp.offsetinFile = getPtr(3 * length + 1 + i*totallength + receptor.columnLength, memory);
			receptor.insert(temp);
		}
	}
}

void MemorytoBranch(char memory[], Branch& receptor) {//使用之前必须知道columnlength
	int length, totallength, turn;
	char turn1[50];
	int i;
	int NumRecord;
	IndexBranch temp;
	length = LENGTHOFPOINTER;
	totallength = length + receptor.columnLength;
	if (memory[4090] == 'R')
		receptor.isRoot = 1;
	else if (memory[4090] == 'N')
		receptor.isRoot = 0;
	receptor.ptrFather = getPtr(0, memory);
	NumRecord = getPtr(length, memory);
	receptor.lastChild = getPtr(4092, memory);
	receptor.recordNum = 0;
	if (memory[2 * length] == 'I') {
		receptor.IntorString = 1;
		for (i = 0; i < NumRecord; i++) {
			temp.ptrChild = getPtr(2 * length + 1 + i*totallength, memory);
			turn = getPtr(3 * length + 1 + i*totallength, memory);
			sprintf(turn1, "%d", turn);
			temp.key = turn1;
			receptor.insert(temp);
		}
	}
	else if (memory[2 * length] == 'S') {
		receptor.IntorString = 0;
		for (i = 0; i < NumRecord; i++) {
			temp.ptrChild = getPtr(2 * length + 1 + i*totallength, memory);
			temp.key = getvalues(3 * length + 1 + i*totallength, 3 * length + 1 + i*totallength + receptor.columnLength - 1, memory);
			receptor.insert(temp);
		}
	}
}

int StringtoInt(string get) {
	int length = get.size();
	int result = 0;
	int i = 0;
	int fuhao;
	if (get[i] == '-') {
		i++;
		fuhao = -1;
	}
	else {
		fuhao = 1;
	}
	for (; i < length; i++) {
		result = result * 10 + get[i] - '0';
	}
	return result * fuhao;
}

float StringtoFloat(string get) {
	float result, subresult;
	result = subresult = 0;
	int flag, j;
	j = flag = 0;
	for (int i = 0; i < get.length(); i++) {
		if (flag == 0) {
			if (get[i] == '.')
				flag = 1;
			else if (get[i] == '-')
				flag = -1;
			else
				result = result * 10 + get[i] - '0';
		}
		else if (flag != 0) {
			subresult = subresult * 10 + get[i] - '0';
			j++;
		}
	}
	subresult = subresult / pow(10, j);
	if (flag == 1)
		return result + subresult;
	else
		return -(result + subresult);
}

int compare(int IntorString, int StringorFloat, string key1, string key2) {
	if (IntorString == 0) {
		if (StringorFloat == 1) {
			if (key1 > key2)
				return 1;
			else if (key1<key2)
				return -1;
			else return 0;
		}
		else if (StringorFloat == 0) {
			if (StringtoFloat(key1) > StringtoFloat(key2))
				return 1;
			else if (StringtoFloat(key1) < StringtoFloat(key2))
				return -1;
			else return 0;
		}
	}
	if (IntorString == 1) {
		if (StringtoInt(key1) > StringtoInt(key2))
			return 1;
		else if (StringtoInt(key1) < StringtoInt(key2))
			return -1;
		else return 0;
	}
}

int comparefi(int type, string key1, string key2) {
	int typeint, typechar, typefloat;
	typeint = INT;
	typechar = CHAR;
	typefloat = FLOAT;
	if (type == typeint) {
		return compare(1, 0, key1, key2);
	}
	else if (type == typechar) {
		return compare(0, 1, key1, key2);
	}
	else if (type == typefloat) {
		return compare(0, 0, key1, key2);
	}
}