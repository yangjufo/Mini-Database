#include "catalog.h"
#include"tinyxml.h"
#include<fstream>

//#include"tinyxml.cpp"
/*����tinyxml�ļ���洢��Ϣ
��tinyxml�ļ��洢�ڵ�ǰĿ¼��calatog�ļ���
ֻ��һ�Ÿ�xml�ļ�
��ʽΪ
MYcalatog---(number��ʾ�м���table number_index)
<table name>
Tableinfo��name,attributenum��PI����һ����ֵ����ʾ�ڼ���������primary key����biggest_blocknum,TotalLength,fi��
attributename��name,type(�ֶ�����),length, IsUnique(�Ƿ�Ψһ),IsPK��
....
Index()
name( indexname,attributename (��Ӧ���ǵڼ���Ԫ��),BBN��biggest block number���� columnLength,blockoffset(���ڵ�Ķ�Ӧ�ڼ����ļ���block),type)
.....
<>......
*/

/*
table��biggest_block���������    x_000000 ��6Ϊ��block��ƫ��������һλ���ļ���ƫ����
*/

//��ʼ��calatog����catalog.xml�ļ��е����ݶ����ڴ�����
catalog::catalog()
{
	string file(".\\catalog\\catalog.xml");

	string name;		//XML ��index ��name�� ��ʾ�ڼ���index
	TiXmlDocument cal;
	cal.LoadFile("catalog.xml");
	//���ݿ��һ�����У�û�����ɹ�xml�ļ�����Ҫ����ļ�
	if (false == cal.LoadFile(file.c_str())){
		TiXmlDocument cal;

		number = 0;
		number_index = 0;
		//����
		TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
		cal.LinkEndChild(decl);

		//���ڵ�
		TiXmlElement * root = new TiXmlElement("MyCalatog");
		cal.LinkEndChild(root);
		root->SetAttribute("number", 0);
		//�ļ�����
		//cal.SaveFile(file.c_str());
	}


	//���ݿ��Ѿ������й��ˣ�xml�ļ��Ѿ������ˣ���xml�ļ������ڴ�
	else{
		TiXmlHandle docHandle(&cal);
		number = atoi(docHandle.FirstChild("MyCalatog").ToElement()->Attribute("number"));//�õ�number
		number_index = atoi(docHandle.FirstChild("MyCalatog").ToElement()->Attribute("number_index"));
		//�õ�table��һ�����
		TiXmlElement *tab = docHandle.FirstChild("MyCalatog").FirstChild().ToElement();

		//TiXmlElement *ind = tab->FirstChildElement("Index");	//�ҵ�index������
		//Ϊʲô������ind = tab->FirstChild("Index")->ToElement()
		//����tab���ֵܽ�㣬ע��tab�ķ��ؿ��ܲ���0
		for (; tab != 0; tab = tab->NextSiblingElement()){
			//	TiXmlElement *ind = tab->FirstChildElement("Index");
			TiXmlElement *tabinfo = tab->FirstChildElement("Tableinfo");
			TiXmlElement *ind = tabinfo->NextSiblingElement();
			Table T;						//Ҫ�й��캯������Ȼ��ʼ����Ϊ0 ��
			T.name = tabinfo->Attribute("name");
			T.attriNum = atoi(tabinfo->Attribute("attributenum"));
			T.primaryindex = atoi(tabinfo->Attribute("PI"));
			T.biggestnum = atoi(tabinfo->Attribute("biggest_blocknum"));
			T.totalLength = atoi(tabinfo->Attribute("TotalLength"));
			TiXmlElement *attribute = tabinfo->FirstChildElement();

			//����������е�attribute��㣬����Ϣ�洢
			for (; attribute != 0; attribute = attribute->NextSiblingElement()){
				Attribute a;
				a.name = attribute->Attribute("name");
				a.type = atoi(attribute->Attribute("type"));
				a.length = atoi(attribute->Attribute("length"));
				a.isUnique = atoi(attribute->Attribute("IsUnique"));
				a.isPk = atoi(attribute->Attribute("IsPK"));
				T.attributes.push_back(a);
			}
			Ta.push_back(T);				//�������
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
���ڴ��е�index��tableд��disk��
*/
catalog::~catalog()
{
	string file(".\\catalog\\catalog.xml");
	string name;		//XML ��index ��name�� ��ʾ�ڼ���index
	TiXmlDocument cal;

	vector<Table>::iterator iter1;
	vector<Index>::iterator iter2;
	vector<Attribute>::iterator iter3;

	//����
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	cal.LinkEndChild(decl);

	//���ڵ�
	TiXmlElement * root = new TiXmlElement("MyCalatog");
	cal.LinkEndChild(root);
	root->SetAttribute("number", number);
	root->SetAttribute("number_index", number_index);


	//��table ����xml�ļ���
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

		//����Ԫ�� attriute node
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
		//��������������index node
		for (iter2 = In.begin(); iter2<In.end();){
			//�ҵ��������
			if ((*iter2).table_name == (*iter1).name){
				TiXmlElement *index_node = new TiXmlElement((*iter2).index_name.c_str());
				tab_node->LinkEndChild(index_node);
				index_node->SetAttribute("indexname", (*iter2).index_name.c_str());
				/*Index()




				name(indexname, attributename(��Ӧ���ǵڼ���Ԫ��),BBN, columnLength, blockoffset(���ڵ�Ķ�Ӧ�ڼ����ļ���block), type)
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
	//�ļ�����
	cal.SaveFile(file.c_str());

}

/*���������ļ�¼��ֱ�����ڴ����޸ģ�Ӳ����ֻ���˳����ݿ��ʱ�����޸�
ע�� ����һ�����ͬʱ������һ���ļ����Թ����ݿ�洢table �����record��¼
*/
bool catalog::CreateTable_catalog(const Table &T)
{
	number++;
	Table newT;
	char c[20];
	_itoa(number, c, 10);
	//����biggsetnum������insert�Ĳ���
	newT = T;
	newT.biggestnum = number*MAX_NUMBER_BLOCK+1;		//0��block��ʾ����1��block��ʼ����

	Ta.push_back(newT);

	/*string filename(c);
	filename = filename + ".txt";
	//������Ӧ���ļ����ں������ݵĲ���
	fstream f(filename.c_str(), fstream::in);
	f.close();
	*/

	return true;


}

//���ڴ��м���index�ļ�¼��Ϣ,number_index ��¼�ľ���index�е��ļ�ƫ����
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

//ɾ���� ֱ�����ڴ���ɾ��
bool catalog::DropTable_catalog(const Table &T)		//ɾ����
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

//ɾ��index ֱ�����ڴ���ɾ��
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

//�������������һ��blokc�Ĵ洢��ţ��ļ����+block��ţ� ����insert ����
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



//��������block ��Ÿ��� ��1
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



//�����INDEX��block ��Ÿ��� ��1
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


/*���ô���һ��attribute �޷�ȷ��*/
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

//true ����������table�� false ������ ��I��Ҫ�� index name 
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
//true ����������index�� false ������ ��I��Ҫ�� index name 
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

//����index������table����ϸ��Ϣ
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

//�鿴���table����һ��Ԫ����index
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


/*���Table T�Ƿ���Ԫ��
ע�⴫�����T��ֻ��һ��attribute��������Ҫ����Ԫ��
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
����Table object��ֻ������
���Table����ϸ��Ϣ
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