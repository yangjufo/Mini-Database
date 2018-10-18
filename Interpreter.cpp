#include<iostream>
#include<string>
#include<vector>
#include"Interpreter.h"
using namespace std;

bool GetChar(string &src, string &des, int column_length);

void Interpreter::inittable(){
	tableInfo.name = "";
	tableInfo.attriNum = tableInfo.totalLength = 0;
	tableInfo.attributes.clear();
}

void Interpreter::initindex(){
	indexInfo.index_name = "";
	indexInfo.table_name = "";
}

void Interpreter::initvalue(){
	if (value.rows.size() > 0)
		value.rows.clear();
}

void Interpreter::initcond(){
	if (conditions.conds.size() > 0)
		conditions.conds.clear();
	if (Uniqueconditions.conds.size() > 0)
		conditions.conds.clear();
}

bool Interpreter::GetWord(string &src, string &des)
{
	des.clear();
	unsigned int srcpos = 0, despos = 0;
	char tempchar = ' ';
	for (srcpos = 0; srcpos < src.length(); srcpos++){
		if (tempchar == ' ' || tempchar == '\t' || tempchar == 10 || tempchar == 13)
			tempchar = src[srcpos];
		else break;
	}
	if (srcpos == src.length() && (tempchar == ' ' || tempchar == '\t' || tempchar == 10 || tempchar == 13))
		return false;
	switch (tempchar){
	case ',':
	case '(':
	case ')':
	case '*':
	case '=':
	case '\'':
		des += tempchar;
		src.erase(0, srcpos);
		break;
	case '<':
		des += tempchar;
		tempchar = src[srcpos++];
		if (tempchar == '=' || tempchar == '>')
		{
			des += tempchar;
			src.erase(0, srcpos);
		}
		else
		{
			src.erase(0, srcpos - 1);
		}
		break;
	case '>':
		des += tempchar;
		tempchar = src[srcpos++];
		if (tempchar == '=')
		{
			des += tempchar;
			src.erase(0, srcpos);
		}
		else
		{
			src.erase(0, srcpos - 1);
		}
		break;
	default:
		do{
			des += tempchar;
			despos++;
			if (srcpos < src.length())
				tempchar = src[srcpos++];
			else{
				src.erase(0, srcpos);
				des[despos] = '\0';
				return true;
			}
		} while (tempchar != '*'&&tempchar != ','&&tempchar != '('&&tempchar != ')'
			&&tempchar != ' '&&tempchar != '\t'&&tempchar != '='&&tempchar != '>'
			&&tempchar != '<'&&tempchar != '\''&&tempchar != 10 && tempchar != 13);
		src.erase(0, srcpos - 1);
	}
	return true;
}

bool Interpreter::GetStr(string &src, string &des)
{
	unsigned int pos;
	des.clear();
	char tempchar;
	if (src[0] == '\''){
		des = "";
		return true;
	}
	else for (pos = 0; pos < src.length(); pos++){
		if (src[pos] != '\''){
			tempchar = src[pos];
			des += tempchar;
		}
		else{
			src.erase(0, pos);
			return true;
		}
	}
	return false;
}

bool Interpreter::Parse(string command)
{
	string word = "";
	string temp = command;
	bool flag;
	flag = GetWord(temp, word);
	if (!flag)
	{
		return false;
	}
	else if (strcmp(word.c_str(), "quit") == 0){
		flag = GetWord(temp, word);
		operation_type = QUIT;
		if (!flag)
			return true;
	}
	else if (strcmp(word.c_str(), "select") == 0)
		flag = GetSelect(temp);
	else if (strcmp(word.c_str(), "create") == 0)
		flag = GetCreate(temp);
	else if (strcmp(word.c_str(), "drop") == 0)
		flag = GetDrop(temp);
	else if (strcmp(word.c_str(), "insert") == 0)
		flag = GetInsert(temp);
	else if (strcmp(word.c_str(), "delete") == 0)
		flag = GetDelete(temp);
	if (flag)
		return true;
	else{
		switch (error_info){
		case SELERR:
		case CRETABERR:
		case CREINDERR:
		case DELERR:
		case INSERR:
		case DROPERR:
		case DROTABERR:
		case DROINDERR:
			cout << "Syntax error occurred！" << endl;
			break;
		case COLERR :
			cout << "Attribute dosen't exist!" << endl;
			break;
		case TABEXISTED:
			cout << "The table exists!" << endl;
			break;
		case CHARBOUND:
			cout << "The char type data is out of bound!" << endl;
			break;
		case NOPK:
			cout << "The table doesn't have primary key!" << endl;
			break;
		case NOUNIQUE:
			cout << "The table doesn't have unique attribute!" << endl;
			break;
		case INDEXISTED:
			cout << "The index exists!" << endl;
			break;
		case NOTABEXIST:
			cout << "No such table exists!" << endl;
			break;
		case NOINDEXIST:
			cout << "No such index exists!" << endl;
			break;
		case FLOATLONG:
			cout << "Too long for float value" << endl;
			break;
		default:
			cout << "Unkonwn error!" << endl;
			break;
		}
		return false;
	}
}

bool Interpreter::GetSelect(string &query)
{
	string word = "";
	string temp = query;
	string tempword;
	Condition tempcon;
	Table temptable;
	string tempop;
	string templeftword;
	string tempcolname;
	int i;
	Comparision op1 = Un;
	bool flag;
	operation_type = SELECT;
	try{
		flag = GetWord(temp, word);
		if (!flag)
			throw runtime_error("f");
		if (strcmp(word.c_str(), "*") != 0)
			throw runtime_error("f");
		flag = GetWord(temp, word);
		if (!flag)
			throw runtime_error("f");
		if (strcmp(word.c_str(), "from") != 0)
			throw runtime_error("f");
		flag = GetWord(temp, word);
		if (!flag)
			throw runtime_error("f");
		temptable.name = word;
		if (!cata.check_Table(temptable)){
			error_info = NOTABEXIST;
			return false;
		}
		tableInfo = cata.get_table_info(temptable);//在表存在的前提下获取表的信息
		flag = GetWord(temp, word);
		if (!flag)
		{
			operation_type = SEL_NOWHERE;
			return true;
		}
		operation_type = SEL_WHERE;
		if (strcmp(word.c_str(), "where") != 0)
			throw runtime_error("f");
		flag = GetWord(temp, word);
		if (!flag)
			throw runtime_error("f");
		if ((word[0] >= '1'&&word[0] <= '9') || word[0] == '-'){
			templeftword = word;
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			tempop = word;
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			tempcolname = word;
			for (i = 0; i < tableInfo.attriNum; i++){
				if (tempcolname.compare(tableInfo.attributes[i].name) == 0)
					break;
			}
			if (i == tableInfo.attriNum){
				error_info = COLERR;
				return false;
			}
			if (tableInfo.attributes[i].type == FLOAT)
				flag = GetFloat(templeftword, tempword, tableInfo.attributes[i].length);
			else
				tempword = templeftword;
			tempcon.column_name = tempcolname;
			if (tempop.compare("<=") == 0){
				op1 = Le;
				tempcon.minValue = tempword;
				tempcon.eqValue2 = tempword;
			}
			else if (tempop.compare("<") == 0){
				op1 = Lt;
				tempcon.minValue = tempword;
			}
			else
				throw runtime_error("f");
		}
		else{
			tempcolname = word;
			tempcon.column_name = word;
			for (i = 0; i < tableInfo.attriNum; i++){
				if (strcmp(tempcolname.c_str(), tableInfo.attributes[i].name.c_str()) == 0){
					break;
				}
			}
			if (i == tableInfo.attriNum){
				error_info = COLERR;
				return false;
			}
		}
		flag = GetWord(temp, word);
		if (!flag)
			throw runtime_error("f");
		if (strcmp(word.c_str(), "<") == 0)
			tempcon.op = Lt;
		else if (strcmp(word.c_str(), "<=") == 0)
			tempcon.op = Le;
		else if (strcmp(word.c_str(), ">") == 0)
			tempcon.op = Gt;
		else if (strcmp(word.c_str(), ">=") == 0)
			tempcon.op = Ge;
		else if (strcmp(word.c_str(), "=") == 0)
			tempcon.op = Eq;
		else if (strcmp(word.c_str(), "<>") == 0)
			tempcon.op = Ne;
		else throw runtime_error("f");
		if (op1 == Lt){
			switch (tempcon.op){
			case Lt:
				tempcon.op = LtLt;
				break;
			case Le:
				tempcon.op = LtLe;
				break;
			default:
				throw runtime_error("f");
			}
		}
		else if (op1 == Le){
			switch (tempcon.op){
			case Lt:
				tempcon.op = LeLt;
				break;
			case Le:
				tempcon.op = LeLe;
				break;
			default:
				throw runtime_error("f");
			}
		}
		flag = GetWord(temp, word);
		if (!flag)
			throw runtime_error("f");
		if (strcmp(word.c_str(), "\'") == 0)
		{
			flag = GetStr(temp, word);
			if (!flag)
				throw runtime_error("f");
			GetChar(word, tempword, tableInfo.attributes[i].length);
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			if (strcmp(word.c_str(), "\'") != 0)
				throw runtime_error("f");
		}
		else{
			if (tableInfo.attributes[i].type == FLOAT)
				flag = GetFloat(word, tempword, tableInfo.attributes[i].length);
			else
				tempword = word;
		}
		switch (tempcon.op){
		case Lt:
			tempcon.maxValue = tempword;
			tempcon.minValue = MIN;
			break;
		case Gt:
			tempcon.maxValue = MAX;
			tempcon.minValue = tempword;
			break;
		case Eq:
			tempcon.eqValue=tempword;
			break;
		case Ge :
			tempcon.maxValue = MAX;
			tempcon.minValue = tempword;
			tempcon.eqValue = tempword;
			break;
		case Le:
			tempcon.maxValue = tempword;
			tempcon.minValue = MIN;
			tempcon.eqValue = tempword;
			break;
		case Ne:
			tempcon.eqValue = tempword;
			break;
		case LtLt:
			tempcon.maxValue = tempword;
			break;
		case LtLe:
			tempcon.maxValue = tempword;
			tempcon.eqValue = tempword;
			break;
		case LeLt:
			tempcon.maxValue = tempword;
			break;
		case LeLe:
			tempcon.maxValue = tempword;
			tempcon.eqValue = tempword;
		default:
			throw runtime_error("f");
		}
		conditions.conds.push_back(tempcon);
		flag = GetWord(temp, word);
		if (!flag)
			return true;
		while (strcmp(word.c_str(), "and") == 0)
		{
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			if ((word[0] >= '1'&&word[0] <= '9') || word[0] == '-'){
				templeftword = word;
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				tempop = word;
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				tempcolname = word;
				for (i = 0; i < tableInfo.attriNum; i++){
					if (strcmp(tempcolname.c_str(), tableInfo.attributes[i].name.c_str()) == 0){
						break;
					}
				}
				if (i == tableInfo.attriNum){
					error_info = COLERR;
					return false;
				}
				if (tableInfo.attributes[i].type == FLOAT)
					flag = GetFloat(templeftword, tempword, tableInfo.attributes[i].length);
				else
					tempword = templeftword;
				tempcon.column_name = tempcolname;
				if (tempop.compare("<=") == 0){
					op1 = Le;
					tempcon.minValue = tempword;
					tempcon.eqValue2 = tempword;
				}
				else if (tempop.compare("<") == 0){
					op1 = Lt;
					tempcon.minValue = tempword;
				}
				else
					throw runtime_error("f");
			}
			else{
				tempcon.column_name = word;
				tempcolname = word;
				for (i = 0; i < tableInfo.attriNum; i++){
					if (strcmp(tempcolname.c_str(), tableInfo.attributes[i].name.c_str()) == 0){
						break;
					}
				}
				if (i == tableInfo.attriNum){
					error_info = COLERR;
					return false;
				}
			}
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			if (strcmp(word.c_str(), "<") == 0)
				tempcon.op = Lt;
			else if (strcmp(word.c_str(), "<=") == 0)
				tempcon.op = Le;
			else if (strcmp(word.c_str(), ">") == 0)
				tempcon.op = Gt;
			else if (strcmp(word.c_str(), ">=") == 0)
				tempcon.op = Ge;
			else if (strcmp(word.c_str(), "=") == 0)
				tempcon.op = Eq;
			else if (strcmp(word.c_str(), "<>") == 0)
				tempcon.op = Ne;
			else throw runtime_error("f");
			if (op1 == Lt){
				switch (tempcon.op){
				case Lt:
					tempcon.op = LtLt;
					break;
				case Le:
					tempcon.op = LtLe;
					break;
				default:
					throw runtime_error("f");
				}
			}
			else if (op1 == Le){
				switch (tempcon.op){
				case Lt:
					tempcon.op = LeLt;
					break;
				case Le:
					tempcon.op = LeLe;
					break;
				default:
					throw runtime_error("f");
				}
			}
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			if (strcmp(word.c_str(), "\'") == 0)
			{
				flag = GetStr(temp, word);
				if (!flag)
					throw runtime_error("f");
				GetChar(word, tempword, tableInfo.attributes[i].length);
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				if (strcmp(word.c_str(), "\'") != 0)
					throw runtime_error("f");
			}
			else{
				if (tableInfo.attributes[i].type == FLOAT)
					flag = GetFloat(word, tempword, tableInfo.attributes[i].length);
				else
					tempword = word;
			}
			switch (tempcon.op){
			case Lt:
				tempcon.maxValue = tempword;
				tempcon.minValue = MIN;
				break;
			case Gt:
				tempcon.maxValue = MAX;
				tempcon.minValue = tempword;
				break;
			case Eq:
				tempcon.eqValue = tempword;
				break;
			case Ge:
				tempcon.maxValue = MAX;
				tempcon.minValue = tempword;
				tempcon.eqValue = tempword;
				break;
			case Le:
				tempcon.maxValue = tempword;
				tempcon.minValue = MIN;
				tempcon.eqValue = tempword;
				break;
			case Ne:
				tempcon.eqValue = tempword;
				break;
			case LtLt:
				tempcon.maxValue = tempword;
				break;
			case LtLe:
				tempcon.maxValue = tempword;
				tempcon.eqValue = tempword;
				break;
			case LeLt:
				tempcon.maxValue = tempword;
				break;
			case LeLe:
				tempcon.maxValue = tempword;
				tempcon.eqValue = tempword;
			default:
				throw runtime_error("f");
			}
			conditions.conds.push_back(tempcon);
			flag = GetWord(temp, word);
			if (!flag)
			{
				operation_type = SEL_WHERE;
				return true;
			}
		}
	}
	catch (runtime_error &in){
		error_info = SELERR;
		return false;
	}
	error_info = SELERR;
	return false;
}

bool Interpreter::GetCreate(string &query)
{
	string word = "";
	string temp = query;
	Attribute tempcol;
	Condition tempcon;
	string temprow;
	int FindPK = 0, FindUnique = 0;
	bool flag;
	int i;
	flag = GetWord(temp, word);
	if (!flag)
		throw runtime_error("f");
	if (strcmp(word.c_str(), "table") == 0){
		operation_type = CRETAB;
		try{
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			tableInfo.name = word;
			if (cata.check_Table(tableInfo)){
				error_info = TABEXISTED;
				return false;
			}
			flag = GetWord(temp, word);
			if (!flag || (strcmp(word.c_str(), "(") != 0))
				throw runtime_error("f");
			flag = GetWord(temp, word);
			if (!flag || (strcmp(word.c_str(), "unique") == 0) || (strcmp(word.c_str(), "primary") == 0))
				throw runtime_error("f");
			tempcol.name = word;
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			if (strcmp(word.c_str(), "int") == 0)
			{
				tempcol.type = INT;
				tempcol.length = INTLEN;
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				if (strcmp(word.c_str(), "unique") == 0)
				{
					tempcol.isUnique = 1;
					flag = GetWord(temp, word);
					if (!flag)
						throw runtime_error("f");
				}
			}
			else if (strcmp(word.c_str(), "float") == 0)
			{
				tempcol.type = FLOAT;
				flag = GetWord(temp, word);
				if (!flag || strcmp(word.c_str(), "(") != 0)
					throw runtime_error("f");
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				tempcol.length = atoi(word.c_str());
				flag = GetWord(temp, word);
				if (!flag || strcmp(word.c_str(), ")") != 0)
					throw runtime_error("f");
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				if (strcmp(word.c_str(), "unique") == 0)
				{
					tempcol.isUnique = 1;
					flag = GetWord(temp, word);
					if (!flag)
						throw runtime_error("f");
				}
			}
			else if (strcmp(word.c_str(), "char") == 0)
			{
				tempcol.type = CHAR;
				flag = GetWord(temp, word);
				if (!flag || (strcmp(word.c_str(), "(") != 0))
					throw runtime_error("f");
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				tempcol.length = atoi(word.c_str());
				if (tempcol.length > 255 || tempcol.length < 1)
				{
					error_info = CHARBOUND;
					return false;
				}
				flag = GetWord(temp, word);
				if (!flag || (strcmp(word.c_str(), ")") != 0))
					throw runtime_error("f");
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				if (strcmp(word.c_str(), "unique") == 0)
				{
					tempcol.isUnique = 1;
					flag = GetWord(temp, word);
					if (!flag)
						throw runtime_error("f");
				}
			}
			else throw runtime_error("f");
			tableInfo.attributes.push_back(tempcol);
			tempcol.isUnique = tempcol.isPk = 0;
			while (strcmp(word.c_str(), ",") == 0)
			{
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				if (strcmp(word.c_str(), "primary") == 0)
				{
					flag = GetWord(temp, word);
					if (!flag || (strcmp(word.c_str(), "key")) != 0)
						throw runtime_error("f");
					flag = GetWord(temp, word);
					if (!flag || (strcmp(word.c_str(), "(") != 0))
						throw runtime_error("f");
					flag = GetWord(temp, word);
					if (!flag) runtime_error("f");
					for (int i = 0; i < tableInfo.attributes.size(); i++){
						if (strcmp(tableInfo.attributes[i].name.c_str(), word.c_str()) == 0){
							FindPK = 1;
							tableInfo.attributes[i].isPk = 1;
							tableInfo.attributes[i].isUnique = 1;
							tableInfo.primaryindex = i;
							indexInfo.index_name = tableInfo.attributes[i].name;
							indexInfo.index_name.insert(0, "pkind");
							indexInfo.attribute_name = tableInfo.attributes[i].name;
							indexInfo.columnLength = tableInfo.attributes[i].length;
							indexInfo.table_name = tableInfo.name;
							indexInfo.type = tableInfo.attributes[i].type;
						}
					}
					flag = GetWord(temp, word);
					if (!flag || (strcmp(word.c_str(), ")")) != 0)
						throw runtime_error("f");
					flag = GetWord(temp, word);
					if (!flag)
						throw runtime_error("f");
				}
				else if (strcmp(word.c_str(), "unique") == 0)
				{
					flag = GetWord(temp, word);
					if (!flag || (strcmp(word.c_str(), "(")) != 0)
						throw runtime_error("f");
					flag = GetWord(temp, word);
					if (!flag)
						throw runtime_error("f");
					for (int i = 0; i < tableInfo.attributes.size(); i++){
						if (strcmp(tableInfo.attributes[i].name.c_str(), word.c_str())){
							FindUnique = 1;
							tableInfo.attributes[i].isUnique = 1;
						}
					}
					if (!FindUnique)
					{
						error_info = NOUNIQUE;
						return false;
					}
					FindUnique = 0;
					flag = GetWord(temp, word);
					if (!flag || strcmp(word.c_str(), ")") != 0)
						throw runtime_error("f");
					flag = GetWord(temp, word);
					if (!flag)
						throw runtime_error("f");
				}
				else
				{
					tempcol.name = word;
					flag = GetWord(temp, word);
					if (!flag)
						throw runtime_error("f");
					if (strcmp(word.c_str(), "int") == 0)
					{
						tempcol.type = INT;
						tempcol.length = INTLEN;
						flag = GetWord(temp, word);
						if (!flag)
							throw runtime_error("f");
						if (strcmp(word.c_str(), "unique") == 0)
						{
							tempcol.isUnique = 1;
							flag = GetWord(temp, word);
							if (!flag)
								throw runtime_error("f");
						}
					}
					else if (strcmp(word.c_str(), "float") == 0)
					{
						tempcol.type = FLOAT;
						flag = GetWord(temp, word);
						if (!flag || strcmp(word.c_str(), "(") != 0)
							throw runtime_error("f");
						flag = GetWord(temp, word);
						if (!flag)
							throw runtime_error("f");
						tempcol.length = atoi(word.c_str());
						flag = GetWord(temp, word);
						if (!flag || strcmp(word.c_str(), ")") != 0)
							throw runtime_error("f");
						flag = GetWord(temp, word);
						if (!flag)
							throw runtime_error("f");
						if (strcmp(word.c_str(), "unique") == 0)
						{
							tempcol.isUnique = 1;
							flag = GetWord(temp, word);
							if (!flag)
								throw runtime_error("f");
						}
					}
					else if (strcmp(word.c_str(), "char") == 0){
						tempcol.type = CHAR;
						flag = GetWord(temp, word);
						if (!flag || (strcmp(word.c_str(), "(") != 0))
							throw runtime_error("f");
						flag = GetWord(temp, word);
						if (!flag)
							throw runtime_error("f");
						tempcol.length = atoi(word.c_str());
						if (tempcol.length > 255 || tempcol.length < 1){
							error_info = CHARBOUND;
							return false;
						}
						flag = GetWord(temp, word);
						if (!flag || (strcmp(word.c_str(), ")") != 0))
							throw runtime_error("f");
						flag = GetWord(temp, word);
						if (!flag)
							throw runtime_error("f");
						if (strcmp(word.c_str(), "unique") == 0)
						{
							tempcol.isUnique = 1;
							flag = GetWord(temp, word);
							if (!flag)
								throw runtime_error("f");
						}
					}
					else throw runtime_error("f");
					tableInfo.attributes.push_back(tempcol);
					tempcol.isPk = tempcol.isUnique = 0;
				}
			}
			if (strcmp(word.c_str(), ")") != 0)
				throw runtime_error("f");
			if (!FindPK)
			{
				error_info = NOPK;
				return false;
			}
			flag = GetWord(temp, word);
			if (!flag){
				tableInfo.attriNum = tableInfo.attributes.size();
				for (int i = 0; i < tableInfo.attributes.size(); i++)
					tableInfo.totalLength += tableInfo.attributes[i].length;
				tableInfo.totalLength++;
				return true;
			}
		}
		catch (runtime_error &in)
		{
			error_info = CRETABERR;
			return false;
		}
	}
	else if (strcmp(word.c_str(), "index") == 0)
	{
		try{
			operation_type = CREIND;
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			indexInfo.index_name = word;
			if (cata.check_index(indexInfo)){
				error_info = INDEXISTED;
				return false;
			}
			flag = GetWord(temp, word);
			if (!flag || (strcmp(word.c_str(), "on") != 0))
				throw runtime_error("f");
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			tableInfo.name = word;
			if (!cata.check_Table(tableInfo)){
				error_info = NOTABEXIST;
				return false;
			}
			tableInfo = cata.get_table_info(tableInfo);
			indexInfo.table_name = word;
			flag = GetWord(temp, word);
			if (!flag || strcmp(word.c_str(), "("))
				throw runtime_error("f");
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			for (i = 0; i < tableInfo.attributes.size(); i++) {
				if (strcmp(word.c_str(), tableInfo.attributes[i].name.c_str()) == 0){
					indexInfo.attribute_name = tableInfo.attributes[i].name;
					indexInfo.columnLength = tableInfo.attributes[i].length;
					indexInfo.type = tableInfo.attributes[i].type;
					break;
				}
			}
			if (i == tableInfo.attriNum){
				error_info = COLERR;
				return false;
			}
			flag = GetWord(temp, word);
			if (!flag || (strcmp(word.c_str(), ")")))
				throw runtime_error("f");
			flag = GetWord(temp, word);
			if (flag)
				throw runtime_error("f");
			return true;
		}
		catch (runtime_error &in)
		{
			error_info = CREINDERR;
			return false;
		}
	}
	return false;
}

bool Interpreter::GetDelete(string &query)
{
	string word = "";
	string temp = query;
	Attribute tempcol;
	Condition tempcon;
	string tempword;
	Table temptable;
	bool flag;
	int i;
	string tempop, templeftword, tempcolname;
	Comparision op1 = Un;
	operation_type = DELETE;
	try{
		flag = GetWord(temp, word);
		if (!flag)
			throw runtime_error("f");
		if (strcmp(word.c_str(), "from") == 0)
		{
			flag = GetWord(temp, word);
			if (!flag) throw runtime_error("f");
			temptable.name = word;
			if (!cata.check_Table(temptable)){
				error_info = NOTABEXIST;
				return false;
			}
			tableInfo = cata.get_table_info(temptable);
			flag = GetWord(temp, word);
			if (!flag) return true;
			if (strcmp(word.c_str(), "where") != 0)
				throw runtime_error("f");
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			if ((word[0] >= '1'&&word[0] <= '9') || word[0] == '-'){
				templeftword = word;
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				tempop = word;
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				tempcolname = word;
				for (i = 0; i < tableInfo.attriNum; i++){
					if (strcmp(tempcolname.c_str(), tableInfo.attributes[i].name.c_str()) == 0){
						break;
					}
				}
				if (i == tableInfo.attriNum){
					error_info = COLERR;
					return false;
				}
				if (tableInfo.attributes[i].type == FLOAT)
					flag = GetFloat(templeftword, tempword, tableInfo.attributes[i].length);
				else
					tempword = templeftword;
				tempcon.column_name = tempcolname;
				if (tempop.compare("<=") == 0){
					op1 = Le;
					tempcon.minValue = tempword;
					tempcon.eqValue2 = tempword;
				}
				else if (tempop.compare("<") == 0){
					op1 = Lt;
					tempcon.minValue = tempword;
				}
				else
					throw runtime_error("f");
			}
			else{
				tempcolname = word;
				for (i = 0; i < tableInfo.attriNum; i++){
					if (strcmp(tempcolname.c_str(), tableInfo.attributes[i].name.c_str()) == 0){
						break;
					}
				}
				if (i == tableInfo.attriNum){
					error_info = COLERR;
					return false;
				}
				tempcon.column_name = word;
			}
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			if (strcmp(word.c_str(), "<") == 0)
				tempcon.op = Lt;
			else if (strcmp(word.c_str(), "<=") == 0)
				tempcon.op = Le;
			else if (strcmp(word.c_str(), ">") == 0)
				tempcon.op = Gt;
			else if (strcmp(word.c_str(), ">=") == 0)
				tempcon.op = Ge;
			else if (strcmp(word.c_str(), "=") == 0)
				tempcon.op = Eq;
			else if (strcmp(word.c_str(), "<>") == 0)
				tempcon.op = Ne;
			else throw runtime_error("f");
			if (op1 == Lt){
				switch (tempcon.op){
				case Lt:
					tempcon.op = LtLt;
					break;
				case Le:
					tempcon.op = LtLe;
					break;
				default:
					throw runtime_error("f");
				}
			}
			else if (op1 == Le){
				switch (tempcon.op){
				case Lt:
					tempcon.op = LeLt;
					break;
				case Le:
					tempcon.op = LeLe;
					break;
				default:
					throw runtime_error("f");
				}
			}
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			if (strcmp(word.c_str(), "\'") == 0)
			{
				flag = GetStr(temp, word);
				if (!flag)
					throw runtime_error("f");
				GetChar(word, tempword, tableInfo.attributes[i].length);
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				if (strcmp(word.c_str(), "\'") != 0)
					throw runtime_error("f");
			}
			else{
				if (tableInfo.attributes[i].type == FLOAT)
					flag = GetFloat(word, tempword, tableInfo.attributes[i].length);
				else
					tempword = word;
			}
			switch (tempcon.op){
			case Lt:
				tempcon.maxValue = tempword;
				tempcon.minValue = MIN;
				break;
			case Gt:
				tempcon.maxValue = MAX;
				tempcon.minValue = tempword;
				break;
			case Eq:
				tempcon.eqValue = tempword;
				break;
			case Ge:
				tempcon.maxValue = MAX;
				tempcon.minValue = tempword;
				tempcon.eqValue = tempword;
				break;
			case Le:
				tempcon.maxValue = tempword;
				tempcon.minValue = MIN;
				tempcon.eqValue = tempword;
				break;
			case Ne:
				tempcon.eqValue = tempword;
				break;
			case LtLt:
				tempcon.maxValue = tempword;
				break;
			case LtLe:
				tempcon.maxValue = tempword;
				tempcon.eqValue = tempword;
				break;
			case LeLt:
				tempcon.maxValue = tempword;
				break;
			case LeLe:
				tempcon.maxValue = tempword;
				tempcon.eqValue = tempword;
			default:
				throw runtime_error("f");
			}
			conditions.conds.push_back(tempcon);
			flag = GetWord(temp, word);
			if (!flag)
				return true;
			while (strcmp(word.c_str(), "and") == 0)
			{
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				if ((word[0] >= '1'&&word[0] <= '9') || word[0] == '-'){
					templeftword = word;
					flag = GetWord(temp, word);
					if (!flag)
						throw runtime_error("f");
					tempop = word;
					flag = GetWord(temp, word);
					if (!flag)
						throw runtime_error("f");
					tempcolname = word;
					for (i = 0; i < tableInfo.attriNum; i++){
						if (strcmp(tempcolname.c_str(), tableInfo.attributes[i].name.c_str()) == 0){
							break;
						}
					}
					if (i == tableInfo.attriNum){
						error_info = COLERR;
						return false;
					}
					if (tableInfo.attributes[i].type == FLOAT)
						flag = GetFloat(templeftword, tempword, tableInfo.attributes[i].length);
					else
						tempword = templeftword;
					tempcon.column_name = tempcolname;
					if (tempop.compare("<=") == 0){
						op1 = Le;
						tempcon.minValue = tempword;
						tempcon.eqValue2 = tempword;
					}
					else if (tempop.compare("<") == 0){
						op1 = Lt;
						tempcon.minValue = tempword;
					}
					else
						throw runtime_error("f");
				}
				else{
					tempcolname = word;
					for (i = 0; i < tableInfo.attriNum; i++){
						if (strcmp(tempcolname.c_str(), tableInfo.attributes[i].name.c_str()) == 0){
							break;
						}
					}
					if (i == tableInfo.attriNum){
						error_info = COLERR;
						return false;
					}
					tempcon.column_name = word;
				}
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				if (strcmp(word.c_str(), "<") == 0)
					tempcon.op = Lt;
				else if (strcmp(word.c_str(), "<=") == 0)
					tempcon.op = Le;
				else if (strcmp(word.c_str(), ">") == 0)
					tempcon.op = Gt;
				else if (strcmp(word.c_str(), ">=") == 0)
					tempcon.op = Ge;
				else if (strcmp(word.c_str(), "=") == 0)
					tempcon.op = Eq;
				else if (strcmp(word.c_str(), "<>") == 0)
					tempcon.op = Ne;
				else throw runtime_error("f");
				if (op1 == Lt){
					switch (tempcon.op){
					case Lt:
						tempcon.op = LtLt;
						break;
					case Le:
						tempcon.op = LtLe;
						break;
					default:
						throw runtime_error("f");
					}
				}
				else if (op1 == Le){
					switch (tempcon.op){
					case Lt:
						tempcon.op = LeLt;
						break;
					case Le:
						tempcon.op = LeLe;
						break;
					default:
						throw runtime_error("f");
					}
				}
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				if (strcmp(word.c_str(), "\'") == 0)
				{
					flag = GetStr(temp, word);
					if (!flag)
						throw runtime_error("f");
					GetChar(word, tempword, tableInfo.attributes[i].length);
					flag = GetWord(temp, word);
					if (!flag)
						throw runtime_error("f");
					if (strcmp(word.c_str(), "\'") != 0)
						throw runtime_error("f");
				}
				else{
					if (tableInfo.attributes[i].type == FLOAT)
						flag = GetFloat(word, tempword, tableInfo.attributes[i].length);
					else
						tempword = word;
				}
				switch (tempcon.op){
				case Lt:
					tempcon.maxValue = tempword;
					tempcon.minValue = MIN;
					break;
				case Gt:
					tempcon.maxValue = MAX;
					tempcon.minValue = tempword;
					break;
				case Eq:
					tempcon.eqValue = tempword;
					break;
				case Ge:
					tempcon.maxValue = MAX;
					tempcon.minValue = tempword;
					tempcon.eqValue = tempword;
					break;
				case Le:
					tempcon.maxValue = tempword;
					tempcon.minValue = MIN;
					tempcon.eqValue = tempword;
					break;
				case Ne:
					tempcon.eqValue = tempword;
					break;
				case LtLt:
					tempcon.maxValue = tempword;
					break;
				case LtLe:
					tempcon.maxValue = tempword;
					tempcon.eqValue = tempword;
					break;
				case LeLt:
					tempcon.maxValue = tempword;
					break;
				case LeLe:
					tempcon.maxValue = tempword;
					tempcon.eqValue = tempword;
				default:
					throw runtime_error("f");
				}
				conditions.conds.push_back(tempcon);
				flag = GetWord(temp, word);
				if (!flag)
					return true;
			}
		}
	}
	catch (runtime_error &in){
		error_info = DELERR;
		return false;
	}
	error_info = DELERR;
	return false;
}

bool Interpreter::GetInsert(string &query)
{
	string word = "";
	string temp = query;
	Attribute tempcol;
	Condition tempcon;
	string temprow;
	Row tempdata;
	string tempword;
	bool flag;
	int i, AttriPos;
	operation_type = INSERT;
	try{
		AttriPos = 0;
		flag = GetWord(temp, word);
		if (!flag)
			throw runtime_error("f");
		if (strcmp(word.c_str(), "into") == 0)
		{
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			tableInfo.name = word;
			if (!cata.check_Table(tableInfo)){
				error_info = NOTABEXIST;
				return false;
			}
			tableInfo = cata.get_table_info(tableInfo);
			flag = GetWord(temp, word);
			if (!flag || (strcmp(word.c_str(), "values") != 0))
				throw runtime_error("f");
			flag = GetWord(temp, word);
			if (!flag || (strcmp(word.c_str(), "(") != 0))
				throw runtime_error("f");
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			if (strcmp(word.c_str(), "\'") == 0)
			{
				flag = GetStr(temp, word);
				if (!flag)
					throw runtime_error("f");
				GetChar(word, tempword, tableInfo.attributes[AttriPos].length);
				temprow = tempword;
				flag = GetWord(temp, word);
				if (!flag || (strcmp(word.c_str(), "\'") != 0))
					throw runtime_error("f");
			}
			else{
				if (tableInfo.attributes[AttriPos].type != FLOAT)
					temprow = word;
				else{
					GetFloat(word, tempword, tableInfo.attributes[AttriPos].length);
					temprow = tempword;
				}
			}
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			tempdata.columns.push_back(temprow);
			while (strcmp(word.c_str(), ",") == 0)
			{
				AttriPos++;
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
				if (strcmp(word.c_str(), "\'") == 0)
				{
					flag = GetStr(temp, word);
					if (!flag)
						throw runtime_error("f");
					GetChar(word, tempword, tableInfo.attributes[AttriPos].length);
					temprow = tempword;
					flag = GetWord(temp, word);
					if (!flag || (strcmp(word.c_str(), "\'") != 0))
						throw runtime_error("f");
				}
				else{
					if (tableInfo.attributes[AttriPos].type != FLOAT)
						temprow = word;
					else{
						GetFloat(word, tempword, tableInfo.attributes[AttriPos].length);
						temprow = tempword;
					}
				}
				tempdata.columns.push_back(temprow);
				flag = GetWord(temp, word);
				if (!flag)
					throw runtime_error("f");
			}
			if (strcmp(word.c_str(), ")") != 0)
				throw runtime_error("f");
			value.rows.push_back(tempdata);
			flag = GetWord(temp, word);
			Uniqueconditions.conds.clear();
			for (int i = 0; i < tableInfo.attributes.size(); i++){//把pk属性对应的值保存下来，API用select命令查找，若存在，则报错
				if (tableInfo.attributes[i].isPk){
					PKpos = i;
					tempcon.column_name = tableInfo.attributes[i].name;
					tempcon.op = Eq;
					tempcon.eqValue = value.rows[0].columns[i];
					Pkcondition = tempcon;
				}
				if (tableInfo.attributes[i].isUnique){//把unique属性对应的值保存下来，用select命令查找，若存在，则报错
					Uniquepos = i;
					tempcon.column_name = tableInfo.attributes[i].name;
					tempcon.op = Eq;
					tempcon.eqValue = value.rows[0].columns[i];
					Uniqueconditions.conds.push_back(tempcon);
				}
			}
			if (!flag)
				return true;
		}
	}
	catch (runtime_error &in){
		error_info = INSERR;
		return false;
	}
	return false;
}

bool Interpreter::GetDrop(string &query)
{
	string word = "";
	string temp = query;
	Attribute tempcol;
	Condition tempcon;
	string temprow;
	int FindPK = 0, FindUnique = 0;
	bool flag;
	flag = GetWord(temp, word);
	if (!flag){
		error_info = DROPERR;
		return false;
	}
	if (strcmp(word.c_str(), "table") == 0){
		operation_type = DRPTAB;
		try{
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			tableInfo.name = word;
			if (!cata.check_Table(tableInfo)){
				error_info = NOTABEXIST;
				return false;
			}
			tableInfo = cata.get_table_info(tableInfo);
			flag = GetWord(temp, word);
			if (!flag)
				return true;
		}
		catch (runtime_error &in){
			error_info = DROTABERR;
			return false;
		}
	}
	else if (strcmp(word.c_str(), "index") == 0){
		operation_type = DRPIND;
		try{
			flag = GetWord(temp, word);
			if (!flag)
				throw runtime_error("f");
			indexInfo.index_name = word;
			if (!cata.check_index(indexInfo)){
				error_info = NOINDEXIST;
				return false;
			}
			flag = GetWord(temp, word);
			if (!flag) return true;
		}
		catch (runtime_error &in){
			error_info = DROINDERR;
			return false;
		}
	}
	return false;
}

bool Interpreter::GetFloat(string &src, string &des, int column_length)
{
	int length, i;
	string word;
	word = src;
	length = word.length();
	for (i = 0; i < word.length(); i++){
		if (word[i] == '.')
			break;
	}
	if (word[0] == '-'&&i != word.length()) {
		word[i] = word[0];
		length--;
	}
	if (length > column_length){
		error_info = FLOATLONG;
		return false;
	}
	if (length < column_length){
		if (word[0] == '-' && i != word.length()) {
			word[0] = '0';
			word.insert(0, (column_length - 1 - length), '0');
		}
		else
			word.insert(0, (column_length - length), '0');
	}
	des = word;
}

bool GetChar(string &src, string &des, int column_length)
{
	string temp = src;
	temp.insert(0, column_length - temp.length(), 1);
	des = temp;
	return true;
}
