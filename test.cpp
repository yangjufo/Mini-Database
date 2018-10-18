#include"catalog.h"
#include"tinyxml.h"
#include<iostream>
#include"buffer.h"
#include"IndexManager.h"

buffer Buffer;
int totalblock = 1000001;
IndexManager im;

void write_app_settings_doc()
{
	TiXmlDocument doc;
	TiXmlElement* msg;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	TiXmlElement * root = new TiXmlElement("MyApp");
	doc.LinkEndChild(root);

	TiXmlComment * comment = new TiXmlComment();
	comment->SetValue(" Settings for MyApp ");
	root->LinkEndChild(comment);

	TiXmlElement * msgs = new TiXmlElement("Messages");
	root->LinkEndChild(msgs);

	msg = new TiXmlElement("Welcome");
	msg->LinkEndChild(new TiXmlText("Welcome to MyApp"));
	msgs->LinkEndChild(msg);

	msg = new TiXmlElement("Farewell");
	msg->LinkEndChild(new TiXmlText("Thank you for using MyApp"));
	msgs->LinkEndChild(msg);

	TiXmlElement * windows = new TiXmlElement("Windows");
	root->LinkEndChild(windows);

	TiXmlElement * window;
	window = new TiXmlElement("Window");
	windows->LinkEndChild(window);
	window->SetAttribute("name", "MainFrame");
	window->SetAttribute("x", 5);
	window->SetAttribute("y", 15);
	window->SetAttribute("w", 400);
	window->SetAttribute("h", 250);

	TiXmlElement * cxn = new TiXmlElement("Connection");
	root->LinkEndChild(cxn);
	cxn->SetAttribute("ip", "192.168.0.1");
	cxn->SetDoubleAttribute("timeout", 123.456); // floating point attrib

	//dump_to_stdout(&doc);
	doc.SaveFile(".\\1\\2appsettings2.xml");
}

catalog cal;

int main(void)
{
	Index I;
	Table T;
	T.name = "table1";
	//cal.CreateTable_catalog(T);
	I.index_name = "index1";
	I.table_name = "table1";
	cal.In[0].BBN = 1000001;
	int result;
	int biggestnum;
	int fileoffset;
	//cal.CreateIndex_catalog(I);
	/*for (int i = 0; i < 10; i++) {
		biggestnum=Buffer.create_newblock_index(I);
		fileoffset = biggestnum / MAX_NUMBER_BLOCK;
		result = Buffer.malloc_block(fileoffset, biggestnum);
		Buffer.b[result].isWritten = 1;
		Buffer.b[result].block[0] = 'A';
	}*/
	IndexColumn testcolumn;
	IndexRow testrow;
	IndexRow drow;
	IndexColumn dc;

	testrow.key = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

	for (int j = 0; j < 30; j++) {
		testrow.key[j % 740]++;
		testrow.blockoffset = j % 740;
		testrow.fileoffset = j % 740;
		testcolumn.rows.push_back(testrow);
	}
	string testrow1;
	testrow.key = "bbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	testrow1 = "bbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	/*
	IndexManager woqu;
	int blocknum;
	int x = 1000001;
	int y = 1;
	Branch tbranch, tbranch1;
	IndexBranch insertbranch1, insertbranch2, insertbranch3;
	insertbranch1.key = "13.7";
	insertbranch1.ptrChild = 21;
	insertbranch2.key = "12-5";
	insertbranch2.ptrChild = 98;
	insertbranch3.key = "1.23";
	insertbranch3.ptrChild = 56;
	tbranch.createBranch(0, 4, FLOAT);
	tbranch.lastChild = 52;
	tbranch.ptrFather = 4;
	tbranch.insert(insertbranch1);
	tbranch.insert(insertbranch2);
	tbranch.insert(insertbranch3);
	//woqu.translateBranch(Buffer.b[blocknum].block, tbranch);
	tbranch1.columnLength = 4;
	tbranch1.IntorString = 0;
	tbranch1.StringorFloat = 0;
	for (int k = 0; k <= 5; k++) {
	blocknum = Buffer.malloc_block(y, x);
	woqu.translateBranch(Buffer.b[blocknum].block, tbranch1);
	tbranch1.ptrFather++;
	Buffer.b[blocknum].isWritten = 1;
	x++;
	}*/
	
	//Index indexinfor;
	//indexinfor.blockoffset = 1000001;
	I.fileoffset = 1;
	I.blockoffset = 1000001;
	I.columnLength = 740;
	I.type = 12;
	im.createIndex(I, testcolumn);

	/*for (int w = 1000001; w <= 1000030; w++) {
	int xy = 1;
	Leaf Tleaf;
	Branch Tbranch;
	Tbranch.createBranch(0, I.columnLength, I.type);
	Tleaf.createLeaf(I.columnLength, I.type);
	int d = Buffer.malloc_block(xy, w);
	if (Buffer.b[d].block[4091] == 'L')
		MemorytoLeaf(Buffer.b[d].block, Tleaf);
	else
		MemorytoBranch(Buffer.b[d].block, Tbranch);
	}*/

	
	int test;
	im.selectEqual(I, testrow.key);
	im.selectBetween(I,testrow.key,testrow1,test);

	for (int k = 4; k <= 9; k++) {
		dc.rows.push_back(testcolumn.rows[k]);
	}
	im.deleteValue(I, dc, test);
}