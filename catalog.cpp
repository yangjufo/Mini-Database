#include "catalog.h"
#include"tinyxml.h"
#include<fstream>

//#include"tinyxml.cpp"
/*调用tinyxml文件库存储信息
将tinyxml文件存储在当前目录的calatog文件下
只有一张个xml文件
格式为
MYcalatog---(number表示有几个table number_index)
<table name>
Tableinfo（name,attributenum，PI（是一个数值，表示第几个属性是primary key），biggest_blocknum,TotalLength,fi）
attributename（name,type(字段类型),length, IsUnique(是否唯一),IsPK）
....
Index()
name( indexname,attributename (对应的是第几个元素),BBN（biggest block number）， columnLength,blockoffset(根节点的对应第几个文件的block),type)
.....
<>......
*/

/*
table的biggest_block的译码规则    x_000000 后6为是block的偏移量，第一位是文件的偏移量
*/

//初始化calatog，将catalog.xml文件中的内容读到内存中来
catalog::catalog()
{
	string file(".\\catalog\\catalog.xml");

	string name;		//XML 中index 的name， 表示第几个index
	TiXmlDocument cal;
	cal.LoadFile("catalog.xml");
	//数据库第一次运行，没有生成过xml文件，故要这个文件
	if (false == cal.LoadFile(file.c_str())){
		TiXmlDocument cal;

		number = 0;
		number_index = 0;
		//声明
		TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
		cal.LinkEndChild(decl);

		//根节点
		TiXmlElement * root = new TiXmlElement("MyCalatog");
		cal.LinkEndChild(root);
		root->SetAttribute("number", 0);
		//文件保存
		//cal.SaveFile(file.c_str());
	}


	//数据库已经在运行过了，xml文件已经生成了，则将xml文件读入内存
	else{
		TiXmlHandle docHandle(&cal);
		number = atoi(docHandle.FirstChild("MyCalatog").ToElement()->Attribute("number"));//得到number
		number_index = atoi(docHandle.FirstChild("MyCalatog").ToElement()->Attribute("number_index"));
		//得到table这一个结点
		TiXmlElement *tab = docHandle.FirstChild("MyCalatog").FirstChild().ToElement();

		//TiXmlElement *ind = tab->FirstChildElement("Index");	//找到index这个结点
		//为什么这样错ind = tab->FirstChild("Index")->ToElement()
		//遍历tab的兄弟结点，注意tab的返回可能不是0
		for (; tab != 0; tab = tab->NextSiblingElement()){
			//	TiXmlElement *ind = tab->FirstChildElement("Index");
			TiXmlElement *tabinfo = tab->FirstChildElement("Tableinfo");
			TiXmlElement *ind = tabinfo->NextSiblingElement();
			Table T;						//要有构造函数，不然初始化就为0 了
			T.name = tabinfo->Attribute("name");
			T.attriNum = atoi(tabinfo->Attribute("attributenum"));
			T.primaryindex = atoi(tabinfo->Attribute("PI"));
			T.biggestnum = atoi(tabinfo->Attribute("biggest_blocknum"));
			T.totalLength = atoi(tabinfo->Attribute("TotalLength"));
			TiXmlElement *attribute = tabinfo->FirstChildElement();

			//遍历这个表中的attribute结点，将信息存储
			for (; attribute != 0; attribute = attribute->NextSiblingElement()){
				Attribute a;
				a.name = attribute->Attribute("name");
				a.type = atoi(attribute->Attribute("type"));
				a.length = atoi(attribute->Attribute("length"));
				a.isUnique = atoi(attribute->Attribute("IsUnique"));
				a.isPk = atoi(attribute->Attribute("IsPK"));
				T.attributes.push_back(a);
			}
			Ta.push_back(T);				//将表存入
			if (ind != 0){
				//ind = ind->FirstChild()->ToElement();
				for (; ind != 0; ind = ind->NextSiblingElement()){
					Index i;
					i.table_name = T.name;
					i.index_name = ind->Attribute("indexname");
					i.BBN = atoi(ind->Attribute("BBN"));
					i.attribute_name = ind->Attribute("attributename");
					i.blockoffset = atoi(ind->Attribute("blockoffset"));
					i.columnLength = atoi(ind->Attribute("columnLength"));
					i.type = atoi(ind->Attribute("type"));
					In.push_back(i);
				}
			}
		}

	}


}

/*
将内存中的index和table写会disk中
*/
catalog::~catalog()
{
	string file(".\\catalog\\catalog.xml");
	string name;		//XML 中index 的name， 表示第几个index
	TiXmlDocument cal;

	vector<Table>::iterator iter1;
	vector<Index>::iterator iter2;
	vector<Attribute>::iterator iter3;

	//声明
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	cal.LinkEndChild(decl);

	//根节点
	TiXmlElement * root = new TiXmlElement("MyCalatog");
	cal.LinkEndChild(root);
	root->SetAttribute("number", number);
	root->SetAttribute("number_index", number_index);


	//将table 插入xml文件中
	for (iter1 = Ta.begin(); iter1 != Ta.end(); iter1++){

		TiXmlElement *tab_node = new TiXmlElement((*iter1).name.c_str());
		root->LinkEndChild(tab_node);
		TiXmlElement *tab_info = new TiXmlElement("Tableinfo");
		tab_node->LinkEndChild(tab_info);
		tab_info->SetAttribute("name", (*iter1).name.c_str());
		tab_info->SetAttribute("attributenum", (*iter1).attriNum);
		tab_info->SetAttribute("PI", (*iter1).primaryindex);
		tab_info->SetAttribute("biggest_blocknum", (*iter1).biggestnum);
		tab_info->SetAttribute("TotalLength", (*iter1).totalLength);

		//插入元素 attriute node
		for (iter3 = (*iter1).attributes.begin(); iter3 != (*iter1).attributes.end(); iter3++){
			TiXmlElement *attri_node = new TiXmlElement((*iter3).name.c_str());
			tab_info->LinkEndChild(attri_node);
			attri_node->SetAttribute("name", (*iter3).name.c_str());
			attri_node->SetAttribute("type", (*iter3).type);
			attri_node->SetAttribute("length", (*iter3).length);
			attri_node->SetAttribute("IsUnique", (*iter3).isUnique);
			attri_node->SetAttribute("IsPK", (*iter3).isPk);


		}

		int i = 0;
		//插入所属这个表的index node
		for (iter2 = In.begin(); iter2<In.end();){
			//找到了这个表
			if ((*iter2).table_name == (*iter1).name){
				TiXmlElement *index_node = new TiXmlElement((*iter2).index_name.c_str());
				tab_node->LinkEndChild(index_node);
				index_node->SetAttribute("indexname", (*iter2).index_name.c_str());
				/*Index()




				name(indexname, attributename(对应的是第几个元素),BBN, columnLength, blockoffset(根节点的对应第几个文件的block), type)
				.....*/
				index_node->SetAttribute("attributename", (*iter2).attribute_name.c_str());
				index_node->SetAttribute("BBN", (*iter2).BBN);
				index_node->SetAttribute("columnLength", (*iter2).columnLength);
				index_node->SetAttribute("blockoffset", (*iter2).blockoffset);
				index_node->SetAttribute("type", (*iter2).type);

				In.erase(iter2);
				iter2 = In.begin();
			}
			else{
				iter2++;
			}

		}
	}
	cal.Print();
	//文件保存
	cal.SaveFile(file.c_str());

}

/*创建插入表的记录，直接在内存中修改，硬盘中只有退出数据库的时候再修改
注意 创建一个表的同时，创建一个文件可以供数据库存储table 的相关record记录
*/
bool catalog::CreateTable_catalog(const Table &T)
{
	number++;
	Table newT;
	char c[20];
	_itoa(number, c, 10);
	//设置biggsetnum，用于insert的插入
	newT = T;
	newT.biggestnum = number*MAX_NUMBER_BLOCK+1;		//0号block表示错误，1号block开始计算

	Ta.push_back(newT);

	/*string filename(c);
	filename = filename + ".txt";
	//创建对应的文件用于后续数据的插入
	fstream f(filename.c_str(), fstream::in);
	f.close();
	*/

	return true;


}

//在内存中加入index的记录信息,number_index 记录的就是index中的文件偏移量
bool catalog::CreateIndex_catalog(const Index& I)
{
	Index temp_I;

	temp_I = I;
	number_index++;
	temp_I.fileoffset = number_index;
	temp_I.blockoffset = number_index*MAX_NUMBER_BLOCK+1;
	temp_I.BBN = temp_I.blockoffset;
	//temp_I.columnLength = I.columnLength;
	In.push_back(temp_I);
	return true;

}

//删除表 直接在内存中删除
bool catalog::DropTable_catalog(const Table &T)		//删除表
{

	TiXmlDocument cal;
	vector<Table>::iterator iter;
	vector<Index>::iterator iter1;

	for (iter = Ta.begin(); iter != Ta.end(); iter++){
		if ((*iter).name == T.name){
			iter=Ta.erase(iter);
			break;
		}
	}
	for (iter1 = In.begin(); iter1 != In.end(); ) {
		if ((*iter1).table_name == T.name) {
			iter1 = In.erase(iter1);
		}
		else {
			iter1++;
		}
	}
	if (iter != Ta.end()){
		return true;
	}
	else{
		return false;
	}

}

//删除index 直接在内存中删除
bool catalog::Dropindex_catalog(const Index &I)
{


	Table temp_T;
	int flag = 0;


	vector<Index>::iterator iter1;

	iter1 = In.begin();
	for (; iter1 != In.end(); iter1++){
		if ((*iter1).index_name == I.index_name){
			flag = 1;
			In.erase(iter1);
			break;

		}
	}

	if (flag == 1){
		return true;
	}
	else{
		return false;
	}


}

//返回这个表的最后一个blokc的存储编号（文件编号+block编号） 用于insert 操作
int catalog::get_Lblockindex(const Table &T)
{
	int result=-1;
	vector<Table>::iterator temp_iter; 
	
	for (temp_iter = Ta.begin(); temp_iter != Ta.end(); temp_iter++){
		if (T.name == (*temp_iter).name){
			result=(*temp_iter).biggestnum;
			break;
		}
	}

	return result;
}



//将这个表的block 编号更新 加1
bool catalog::add_Lblockindex(Table &T)
{
	vector<Table>::iterator temp_iter;

	for (temp_iter = Ta.begin(); temp_iter != Ta.end(); temp_iter++){
		if (T.name == (*temp_iter).name){
			(*temp_iter).biggestnum++;
		}
	}
	return false;

}

int catalog::get_Lblockindex(const Index &I)
{
	int result = -1;
	vector<Index>::iterator temp_iter;

	for (temp_iter = In.begin(); temp_iter != In.end(); temp_iter++){
		if (I.index_name== (*temp_iter).index_name){
			result = (*temp_iter).BBN;
			break;
		}
	}

	return result;
}



//将这个INDEX的block 编号更新 加1
bool catalog::add_Lblockindex(Index &I)
{
	vector<Index>::iterator temp_iter;

	for (temp_iter = In.begin(); temp_iter != In.end(); temp_iter++){
		if (I.index_name == (*temp_iter).index_name){
			(*temp_iter).BBN++;
			return true;
		}
	}
	return false;

}


/*光用传输一个attribute 无法确认*/
bool catalog::check_PK(const Table & T)
{
	vector<Table>::iterator temp_iter;
	vector<Attribute>::iterator temp_iter1;


	for (temp_iter = Ta.begin(); temp_iter != Ta.end(); temp_iter++){
		if (T.name == (*temp_iter).name){
			for (temp_iter1 = (*temp_iter).attributes.begin(); temp_iter1 != (*temp_iter).attributes.end(); temp_iter1++){
				if (T.attributes[0].name == (*temp_iter1).name){
					return (*temp_iter1).isPk;
				}
			}
		}
	}
	return false;
}

//true 存在这样的table， false 不存在 ；I中要有 index name 
bool catalog::check_Table(const Table &T)
{
	vector<Table>::iterator iter;

	for (iter = Ta.begin(); iter != Ta.end(); iter++){
		if (T.name == (*iter).name){
			return true;
		}
	}
	return false;
}
//true 存在这样的index， false 不存在 ；I中要有 index name 
bool catalog::check_index(const Index &I)
{

	vector<Index>::iterator iter1;


	for (iter1 = In.begin(); iter1 != In.end(); iter1++){
		if ((I.index_name == (*iter1).index_name)||(I.attribute_name==(*iter1).attribute_name&&I.table_name==(*iter1).table_name)){
			return true;
		}
	}
	return false;
}

//返回index所属的table的详细信息
Table catalog::gettable(const Index&I)
{
	vector<Table>::iterator iter;
	Table result;

	for (iter = Ta.begin(); iter != Ta.end(); iter++){
		if (I.table_name == (*iter).name){
			result = (*iter);
			return result;
		}
	}
}

//查看这个table中哪一个元素是index
vector<Index> catalog::check_index( Table &T)
{
	Index I;
	
	vector<Index> result;
	vector<Index>::iterator iter1;

	
	
	
	I.table_name = T.name;
	for (iter1 = In.begin(); iter1 != In.end(); iter1++){
		if ((T.name == (*iter1).table_name) ){
			I = (*iter1);
			I.fileoffset = I.blockoffset / 1000000;
			result.push_back(I);
		}
	}
	

	return result;
}


/*检测Table T是否有元素
注意传入参数T中只含一个attribute，就是需要检测的元素
*/

bool catalog::check_attribute(const Table&T)
{
	vector<Table>::iterator temp_iter;
	vector<Attribute>::iterator temp_iter1;

	for (temp_iter = Ta.begin(); temp_iter != Ta.end(); temp_iter++){
		if (T.name == (*temp_iter).name){
			for (temp_iter1 = (*temp_iter).attributes.begin(); temp_iter1 != (*temp_iter).attributes.end(); temp_iter1++){
				if (T.attributes[0].name == (*temp_iter1).name){
					return true;
				}
			}
		}
	}

	return false;
}

/*
输入Table object，只有名字
输出Table的详细信息
*/
Table catalog::get_table_info(const Table&T)
{
	vector<Table>::iterator iter;
	Table result;

	for (iter = Ta.begin(); iter != Ta.end(); iter++){
		if (T.name == (*iter).name){
			result = (*iter);
			return result;
		}
	}

}