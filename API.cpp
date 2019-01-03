#include<iostream>
#include<string>
#include<time.h>
#include"buffer.h"
#include"catalog.h"
#include"common.h"
#include"Interpreter.h"
#include"Recorder Manger.h"
#include"IndexManager.h"

using namespace std;


catalog cata;

Interpreter parsetree;
RecorderManager recorder;
IndexManager indexM;

buffer Buffer;


Index checkifindex(Conditiondata checkC, vector<Index> chechI, int &num) {
	int i, j;
	Index tempindex;
	for (i = 0; i < checkC.conds.size(); i++) {
		for (j = 0; j < chechI.size(); j++) {
			if (checkC.conds[i].column_name.compare(chechI[j].attribute_name) == 0)
				break;
		}
		if (j != chechI.size())
			break;
	}
	if (i == checkC.conds.size())
		return tempindex;
	num = i;
	return chechI[j];
}

Data DoSelect(Conditiondata &conditions){
	Index indexinfor;
	Data results;
	IndexColumn indexdata;
	IndexColumn tempinddata;
	vector<Index> whatindex;
	Row temprow;
	int i = 0;
	int num, breakflag = 0;
	whatindex.clear();
	whatindex = cata.check_index(parsetree.tableInfo);
	indexinfor.columnLength = 0;
	indexinfor = checkifindex(conditions, whatindex, num);//condition需要是引用
	temprow.isDelete = 0;
	if (indexinfor.type == INT)
		indexinfor.columnLength = 4;
	if (indexinfor.columnLength != 0) {
		switch (conditions.conds[num].op) {
		case Lt:
		case Gt:
		case LtLt:
			indexdata.rows.clear();
			indexdata = indexM.selectBetween(indexinfor, conditions.conds[num].minValue, conditions.conds[num].maxValue);
			results.rows.clear();
			if (indexdata.rows[0].key == ""){
				temprow.isDelete = 2;
				results.rows.push_back(temprow);
			}
			else
				results = recorder.SelectIndex(parsetree.tableInfo, indexdata, conditions);
			break;
		case Le:
		case Ge:
		case LtLe:
			indexdata.rows.clear();
			indexdata = indexM.selectBetween(indexinfor, conditions.conds[num].minValue, conditions.conds[num].maxValue);
			tempinddata = indexM.selectEqual(indexinfor, conditions.conds[num].eqValue);
			for (int i = 0; i < tempinddata.rows.size(); i++)
				indexdata.rows.push_back(tempinddata.rows[i]);
			results.rows.clear();
			if (indexdata.rows[0].key == ""){
				temprow.isDelete = 2;
				results.rows.push_back(temprow);
			}
			else
				results = recorder.SelectIndex(parsetree.tableInfo, indexdata, conditions);
			break;
		case LeLt:
			indexdata.rows.clear();
			indexdata = indexM.selectBetween(indexinfor, conditions.conds[num].minValue, conditions.conds[num].maxValue);
			tempinddata = indexM.selectEqual(indexinfor, conditions.conds[num].eqValue2);
			for (int i = 0; i < tempinddata.rows.size(); i++)
				indexdata.rows.push_back(tempinddata.rows[i]);
			results.rows.clear();
			if (indexdata.rows[0].key == ""){
				temprow.isDelete = 2;
				results.rows.push_back(temprow);
			}
			else
				results = recorder.SelectIndex(parsetree.tableInfo, indexdata, conditions);
			break;
		case LeLe:
			indexdata.rows.clear();
			indexdata = indexM.selectBetween(indexinfor, conditions.conds[num].minValue, conditions.conds[num].maxValue);
			tempinddata = indexM.selectEqual(indexinfor, conditions.conds[num].eqValue);
			for (int i = 0; i < tempinddata.rows.size(); i++)
				indexdata.rows.push_back(tempinddata.rows[i]);
			tempinddata.rows.clear();
			tempinddata = indexM.selectEqual(indexinfor, conditions.conds[num].eqValue);
			for (int i = 0; i < tempinddata.rows.size(); i++)
				indexdata.rows.push_back(tempinddata.rows[i]);
			results.rows.clear();
			if (indexdata.rows[0].key == ""){
				temprow.isDelete = 2;
				results.rows.push_back(temprow);
			}
			else
				results = recorder.SelectIndex(parsetree.tableInfo, indexdata, conditions);;
			break;
		case Eq:
			indexdata.rows.clear();
			indexdata = indexM.selectEqual(indexinfor, conditions.conds[0].eqValue);
			results.rows.clear();
			if (indexdata.rows[0].key == ""){
				temprow.isDelete = 2;
				results.rows.push_back(temprow);
			}
			else
				results = recorder.SelectIndex(parsetree.tableInfo, indexdata, conditions);
			break;
		case Ne:
			results.rows.clear();
			results = recorder.SelectNoIndex(parsetree.tableInfo, conditions);
			break;
		}
	}
	else {
		results.rows.clear();
		results = recorder.SelectNoIndex(parsetree.tableInfo, conditions);
	}
	return results;
}

void AddSeperator(char *command)
{
	unsigned len = strlen(command);
	command[len] = ' ';
	command[len + 1] = '\0';
}

short int IsComEnd(char *input)
{
	unsigned int next = strlen(input) - 1;
	char prev = ' ';
	while (next >= 0 && (prev == '\t' || prev == ' '))
	{
		prev = input[next];
		next--;
	}
	if (prev == ';')
	{
		input[next + 1] = '\0';
		return 1;
	}
	return 0;
}

void Print(const Table &tableInfo, const Data &results)
{
	if (results.rows[0].isDelete == 2)
		cout << "Not Found!" << endl;
	else{
		cout << tableInfo.attributes[0].name << "\t\t";
		for (int i = 1; i < tableInfo.attributes.size(); i++)
			cout << tableInfo.attributes[i].name << "\t";
		cout << endl;
		for (int i = 0; i < results.rows.size(); i++){
			for (int j = 0; j < results.rows[i].columns.size(); j++){
				cout << results.rows[i].columns[j] << "\t";
			}
			cout << endl;
		}
	}
}

int count1 = 0;

int main()
{
	Conditiondata conditions;
	Table tableinfor;
	Attribute tempattri;
	Index indexinfor;
	Row insertvalue;
	Row temprow;
	Data datas;
	Data results;
	string column;
	Table tableinf;
	IndexColumn indexdata;
	IndexColumn tempinddata;
	vector<Index> whatindex;
	int i = 0;
	int count = 0;
	int num, breakflag = 0;
	char command[COMLEN] = "";
	char input[INPUTLEN] = "";
	char word[WORDLEN] = "";
	short int ComEnd = 0;
	time_t t;
	tm *lt;
	t = time(NULL);
	lt = localtime(&t);
	cout << "****************************************************************" << endl;
	cout << "Welcome to MiniSQL!" << endl;
	cout << lt->tm_year + 1900 << "-" << lt->tm_mon + 1 << "-" << lt->tm_mday << " " << lt->tm_hour << ":" << lt->tm_min << ":" << lt->tm_sec << endl;
	cout << "****************************************************************" << endl;
	while (1){
		clock_t start1,end1;

		count1++;
		cout << "->";
		strcpy(command, "");
		ComEnd = 0;
		while (!ComEnd)
		{
			cin.getline(input, 200);
			if (IsComEnd(input))
				ComEnd = 1;
			strcat(command, input);
			AddSeperator(command);
		}
		parsetree.MakeInitial();
		parsetree.Parse(command);
		if (parsetree.error_info != UNKNOW){
			parsetree.error_info = UNKNOW;
			continue;
		}
		switch (parsetree.operation_type) {
		case SEL_NOWHERE:
			results = recorder.SelectNoIndex(parsetree.tableInfo, parsetree.conditions);
			Print(parsetree.tableInfo, results);
			break;
		case SEL_WHERE:
			start1 = clock();
			results.rows.clear();
			results = DoSelect(parsetree.conditions);
			Print(parsetree.tableInfo, results);
			end1 = clock();
			//cout << (double)(start1 - end1) / CLOCKS_PER_SEC<<endl;
			break;
		case CRETAB:
			cata.CreateTable_catalog(parsetree.tableInfo);
			cata.CreateIndex_catalog(parsetree.indexInfo);
			tableinf = parsetree.tableInfo;
			tableinf.name = tableinf.name.insert(0, "z", 1);
			cata.CreateTable_catalog(tableinf);
			break;
		case CREIND:
			cata.CreateIndex_catalog(parsetree.indexInfo);
			parsetree.tableInfo = cata.get_table_info(parsetree.tableInfo);
			indexdata = recorder.CreateIndexData(parsetree.tableInfo, parsetree.indexInfo);
			whatindex = cata.check_index(parsetree.tableInfo);
			for (i = 0; i < whatindex.size(); i++) {
				if (whatindex[i].index_name.compare(parsetree.indexInfo.index_name) == 0)
					break;
			}
			if (whatindex[i].type == INT)
				whatindex[i].columnLength = 4;
			indexM.createIndex(whatindex[i], indexdata);
			break;
		case DRPTAB:
			cata.DropTable_catalog(parsetree.tableInfo);
			tableinf = parsetree.tableInfo;
			tableinf.name = tableinf.name.insert(0, "z", 1);
			cata.DropTable_catalog(tableinf);
			break;
		case DRPIND:
			cata.Dropindex_catalog(parsetree.indexInfo);
			break;
		case DELETE:
			whatindex.clear();
			whatindex = cata.check_index(parsetree.tableInfo);
			indexinfor.columnLength = 0;
			indexinfor = checkifindex(parsetree.conditions, whatindex, num);//condition需要是引用
			temprow.isDelete = 0;
			if (indexinfor.type == INT)
				indexinfor.columnLength = 4;
			conditions = parsetree.conditions;
			if (indexinfor.columnLength != 0) {
				switch (conditions.conds[num].op) {
				case Lt:
				case Gt:
				case LtLt:
					indexdata.rows.clear();
					indexdata = indexM.selectBetween(indexinfor, conditions.conds[num].minValue, conditions.conds[num].maxValue);
					if (indexdata.rows[0].key == ""){
						temprow.isDelete = 2;
						results.rows.push_back(temprow);
					}
					else
						results = recorder.SelectIndex(parsetree.tableInfo, indexdata, conditions);
					break;
				case Le:
				case Ge:
				case LtLe:
					indexdata.rows.clear();
					indexdata = indexM.selectBetween(indexinfor, conditions.conds[num].minValue, conditions.conds[num].maxValue);
					tempinddata = indexM.selectEqual(indexinfor, conditions.conds[num].eqValue);
					for (int i = 0; i < tempinddata.rows.size(); i++)
						indexdata.rows.push_back(tempinddata.rows[i]);
					results.rows.clear();
					if (indexdata.rows[0].key == ""){
						temprow.isDelete = 2;
						results.rows.push_back(temprow);
					}
					else
						results = recorder.SelectIndex(parsetree.tableInfo, indexdata, conditions);
					break;
				case LeLt:
					indexdata.rows.clear();
					indexdata = indexM.selectBetween(indexinfor, conditions.conds[num].minValue, conditions.conds[num].maxValue);
					tempinddata = indexM.selectEqual(indexinfor, conditions.conds[num].eqValue2);
					for (int i = 0; i < tempinddata.rows.size(); i++)
						indexdata.rows.push_back(tempinddata.rows[i]);
					results.rows.clear();
					if (indexdata.rows[0].key == ""){
						temprow.isDelete = 2;
						results.rows.push_back(temprow);
					}
					else
						results = recorder.SelectIndex(parsetree.tableInfo, indexdata, conditions);
					break;
				case LeLe:
					indexdata.rows.clear();
					indexdata = indexM.selectBetween(indexinfor, conditions.conds[num].minValue, conditions.conds[num].maxValue);
					tempinddata = indexM.selectEqual(indexinfor, conditions.conds[num].eqValue);
					for (int i = 0; i < tempinddata.rows.size(); i++)
						indexdata.rows.push_back(tempinddata.rows[i]);
					tempinddata.rows.clear();
					tempinddata = indexM.selectEqual(indexinfor, conditions.conds[num].eqValue);
					for (int i = 0; i < tempinddata.rows.size(); i++)
						indexdata.rows.push_back(tempinddata.rows[i]);
					results.rows.clear();
					if (indexdata.rows[0].key == ""){
						temprow.isDelete = 2;
						results.rows.push_back(temprow);
					}
					else
						results = recorder.SelectIndex(parsetree.tableInfo, indexdata, conditions);
					break;
				case Eq:
					indexdata.rows.clear();
					indexdata = indexM.selectEqual(indexinfor, conditions.conds[0].eqValue);
					results.rows.clear();
					if (indexdata.rows[0].key == ""){
						temprow.isDelete = 2;
						results.rows.push_back(temprow);
					}
					else
						results = recorder.SelectIndex(parsetree.tableInfo, indexdata, conditions);
					break;
				case Ne:
					results.rows.clear();
					results = recorder.SelectNoIndex(parsetree.tableInfo, conditions);
					break;
				}
				if (conditions.conds[num].op != Ne) {
					results.rows.clear();
					results = recorder.DeleteIndex(parsetree.tableInfo, indexdata, conditions);
				}
			}
			if (indexinfor.columnLength == 0||conditions.conds[num].op == Ne) {
				results.rows.clear();
				results = recorder.DeleteNoIndex(parsetree.tableInfo, conditions);
			}
			indexdata.rows.clear();
			tempinddata.rows.clear();
			whatindex = cata.check_index(parsetree.tableInfo);
			for (int i = 0; i < whatindex.size(); i++) {
				if (whatindex[i].type == INT)
					whatindex[i].columnLength = 4;
				indexdata = recorder.GetIndexData(parsetree.tableInfo, whatindex[i], results);
				//indexM.deleteValue(whatindex[i], indexdata);
			}
			tableinf = parsetree.tableInfo;
			tableinf.name = tableinf.name.insert(0, "z", 1);
			recorder.InsertDelValue(tableinf, results);
			break;
		case INSERT:
			if (count >= 10000){
				for (int i = 0; i < parsetree.Uniqueconditions.conds.size(); i++){
					results.rows.clear();
					conditions.conds.clear();
					conditions.conds.push_back(parsetree.Uniqueconditions.conds[i]);
					results = DoSelect(conditions);
					if (results.rows[0].isDelete != 2){
						if (i == 0)
							cout << "Primary Key exists!" << endl;
						else
							cout << "Unique attribute error!" << endl;
						break;
					}
				}
			}
			if (count<10000 || results.rows[0].isDelete == 2){
				datas = recorder.InsertValue(parsetree.tableInfo, parsetree.value);
				whatindex = cata.check_index(parsetree.tableInfo);
				while (whatindex.size() > 0) {
					indexdata.rows.clear();
							indexdata = recorder.GetIndexData(parsetree.tableInfo, whatindex[whatindex.size() - 1], datas);
					if (whatindex[whatindex.size() - 1].type == INT)
						whatindex[whatindex.size() - 1].columnLength = 4;
					indexM.index_insertValue(whatindex[whatindex.size() - 1], indexdata);
					whatindex.pop_back();
				}
				count++;
			}
			break;
		case QUIT:
			breakflag = 1;
			break;
		}
		if (breakflag)
			break;
	}
	//getchar();
	return 0;
}