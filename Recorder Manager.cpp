#include"Recorder Manger.h"

bool ReFloat(string &src);
bool ReChar(string &src);

bool RecorderManager::checkDtable(const Table &table, Row &deltdata){
	Table temptable = table;
	int i, j, pos = 0;
	Row temprow;
	int tempnum, tempmul;
	bool flag = false;
	temptable.name.insert(0, "z");
	temptable = cata.get_table_info(temptable);
	pos = Buffer.malloc_block(temptable.biggestnum);
	for (i = 10; i < Buffer.b[pos].vaild_number;){
		if (Buffer.b[pos].block[i] == '1'){
			i = i + 11;
			continue;
		}
		i -= 10;
		tempmul = 1000000;
		tempnum = 0;
		for (j = 0; j < 7; j++){
			tempnum += Buffer.b[pos].block[i++] * tempmul;
			tempmul /= 10;
		}
		temprow.biggestnum = tempnum;
		tempmul = 100;
		tempnum = 0;
		for (j = 0; j < 3; j++){
			tempnum += Buffer.b[pos].block[i++] * tempmul;
			tempmul /= 10;
		}
		temprow.blockoffset = tempnum;
		Buffer.b[pos].block[i] = '1';
		Buffer.b[pos].isWritten = 1;
		flag = true;
		deltdata = temprow;
		break;
	}
	return flag;
}

Data RecorderManager::InsertValue(const Table &table, const Data &data){
	Table temptable = table;
	int i, j, k, pos, Pkposition, tempvalid_num;
	Data tempdata = data;
	Row  tempdeldata;
	temptable = cata.get_table_info(table);
	Pkposition = temptable.primaryindex;
	for (i = 0; i < tempdata.rows.size(); i++){
		if (checkDtable(temptable, tempdeldata)){
			pos = Buffer.malloc_block(tempdeldata.biggestnum);
			tempvalid_num = temptable.totalLength * (tempdeldata.blockoffset - 1) + 1;
			for (j = 0; j < tempdata.rows[i].columns.size(); j++){
				if (temptable.attributes[j].type == INT){
					tempdata.rows[i].columns[j].insert(0, INTLEN - tempdata.rows[i].columns[j].length(), '0');
				}
				for (k = 0; k < tempdata.rows[i].columns[j].length(); k++){
					Buffer.b[pos].block[++tempvalid_num] = tempdata.rows[i].columns[j][k];
				}
				if (temptable.attributes[j].type == INT)
					tempdata.rows[i].columns[j].erase(0, INTLEN - tempdata.rows[i].columns[j].length());
			}
			Buffer.b[pos].block[++tempvalid_num] = '0';
			Buffer.b[pos].isWritten = 1;
			tempdata.rows[i].biggestnum = tempdeldata.biggestnum;
			tempdata.rows[i].blockoffset = tempdeldata.blockoffset;
		}
		else{
			pos = Buffer.malloc_block(temptable.biggestnum);
			if (Buffer.b[pos].vaild_number == 0)Buffer.b[pos].vaild_number = -1;
			if (Buffer.b[pos].vaild_number + temptable.totalLength <= block_size){
				for (j = 0; j < tempdata.rows[i].columns.size(); j++){
					if (temptable.attributes[j].type == INT){
						tempdata.rows[i].columns[j].insert(0, INTLEN - tempdata.rows[i].columns[j].length(), '0');
					}
					for (k = 0; k < tempdata.rows[i].columns[j].length(); k++){
						Buffer.b[pos].block[++Buffer.b[pos].vaild_number] = tempdata.rows[i].columns[j][k];
					}
					if (temptable.attributes[j].type == INT)
						tempdata.rows[i].columns[j].erase(0, INTLEN - tempdata.rows[i].columns[j].length());
				}
				Buffer.b[pos].block[++Buffer.b[pos].vaild_number] = '0';
				Buffer.b[pos].isWritten = 1;
				tempdata.rows[i].biggestnum = temptable.biggestnum;
				tempdata.rows[i].blockoffset = (Buffer.b[pos].vaild_number-1) / temptable.totalLength;
			}
			else{
				cata.add_Lblockindex(temptable);
				temptable.biggestnum++;
				pos = Buffer.malloc_block(temptable.biggestnum);
				if (Buffer.b[pos].vaild_number == 0)Buffer.b[pos].vaild_number = -1;
				for (j = 0; j < tempdata.rows[i].columns.size(); j++){
					if (temptable.attributes[j].type == INT){
						tempdata.rows[i].columns[j].insert(0, INTLEN - tempdata.rows[i].columns[j].length(), '0');
					}
					for (k = 0; k < tempdata.rows[i].columns[j].length(); k++){
						Buffer.b[pos].block[++Buffer.b[pos].vaild_number] = tempdata.rows[i].columns[j][k];
					}
					if (temptable.attributes[j].type == INT)
						tempdata.rows[i].columns[j].erase(0, INTLEN - tempdata.rows[i].columns[j].length());
				}
				Buffer.b[pos].block[++Buffer.b[pos].vaild_number] = '0';
				Buffer.b[pos].isWritten = 1;
				tempdata.rows[i].biggestnum = temptable.biggestnum;
				tempdata.rows[i].blockoffset = (Buffer.b[pos].vaild_number - 1) / temptable.totalLength;
			}
		}
	}
	return tempdata;
}

Data RecorderManager::SelectNoIndex(const Table &table, Conditiondata conditions)
{
	Table temptable = table;
	int start;
	Data result;
	int column_num[32] = { 0 };
	int con_num[32] = { 0 };
	Row temprow;
	string tempcol;
	int i, j, k, l, m, n;
	int pos, flag, res;
	temptable = cata.get_table_info(temptable);
	start = temptable.biggestnum / 1000000 * 1000000 + 1;
	k = -1;
	for (i = 0; i < temptable.attriNum; i++){ //查找对应哪些属性
		for (j = 0; j < conditions.conds.size(); j++){
			if (!(temptable.attributes[i].name.compare(conditions.conds[j].column_name))){
				column_num[++k] = i;
				con_num[k] = j;
			}
		}
	}
	for (i = start; i <= temptable.biggestnum; i++){
		j = 0;
		pos = Buffer.malloc_block(i);
		for (; j < Buffer.b[pos].vaild_number;){
			temprow.columns.clear();
			for (k = 0; k < temptable.attriNum; k++){
				for (l = 0; l < temptable.attributes[k].length; l++){
					tempcol += Buffer.b[pos].block[j++];
				}
				n = 0;
				if (temptable.attributes[k].type == INT){
					do{
						if (tempcol[n] == '0')
							n++;
						else break;
					} while (1);
					tempcol.erase(0, n);
				}
				temprow.columns.push_back(tempcol);
				tempcol = "";
			}
			if (Buffer.b[pos].block[j++] == '1')
				continue;
			flag = 0;
			for (m = 0; m < conditions.conds.size(); m++){
				switch (conditions.conds[m].op){
				case Gt:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].minValue);
					if (res <= 0)
						flag = 1;
					break;
				case Lt:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].maxValue);
					if (res >= 0)
						flag = 1;
					break;
				case Ge:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].minValue);
					if (res < 0)
						flag = 1;
					break;
				case Le:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].maxValue);
					if (res > 0)
						flag = 1;
					break;
				case Eq:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].eqValue);
					if (res != 0)
						flag = 1;
					break;
				case Ne:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].eqValue);
					if (res == 0)
						flag = 1;
					break;
				case LtLt:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].minValue);
					if (res <= 0)
						flag = 1;
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].maxValue);
					if (res >= 0)
						flag = 1;
					break;
				case LtLe:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].minValue);
					if (res <= 0)
						flag = 1;
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].maxValue);
					if (res > 0)
						flag = 1;
					break;
				case LeLt:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].minValue);
					if (res < 0)
						flag = 1;
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].maxValue);
					if (res >= 0)
						flag = 1;
					break;
				case LeLe:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].minValue);
					if (res < 0)
						flag = 1;
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].maxValue);
					if (res > 0)
						flag = 1;
					break;
				default:
					break;
				}
				if (flag)
					break;
			}
			if (!flag){
				for (int i = 0; i < temptable.attriNum; i++){
					if (temptable.attributes[i].type == FLOAT)
						flag = ReFloat(temprow.columns[i]);
					if (temptable.attributes[i].type == CHAR)
						flag = ReChar(temprow.columns[i]);
				}
				result.rows.push_back(temprow);
			}
		}
	}
	if (result.rows.size() == 0){
		temprow.isDelete = 2;
		result.rows.push_back(temprow);
	}
	return result;
}

Data RecorderManager::SelectIndex(const Table &table, IndexColumn &indexdata, Conditiondata conditions)
{
	Table temptable;
	Data result;
	int column_num[32] = { 0 };
	int con_num[32] = { 0 };
	Row temprow;
	//IndexColumn tempindexdata = indexdata;
	string tempcol;
	int i, j, k, l, m, n;
	int pos, flag, res;
	temptable = table;
	temptable = cata.get_table_info(temptable);
	k = -1;
	for (i = 0; i < temptable.attriNum; i++){
		for (j = 0; j < conditions.conds.size(); j++){
			if (temptable.attributes[i].name.compare(conditions.conds[j].column_name) == 0){
				column_num[++k] = i;
				con_num[k] = j;
			}
		}
	}
	for (i = 0; i < indexdata.rows.size(); i++){
		pos = Buffer.malloc_block(indexdata.rows[i].blockoffset);
		j = indexdata.rows[i].fileoffset*temptable.totalLength;
		temprow.columns.clear();
		for (k = 0; k < temptable.attriNum; k++) {
			for (l = 0; l < temptable.attributes[k].length; l++) {
				tempcol += Buffer.b[pos].block[j++];
			}
			n = 0;
			if (temptable.attributes[k].type == INT) {
				do {
					if (tempcol[n] == '0')
						n++;
					else break;
				} while (1);
				tempcol.erase(0, n);
			}
			temprow.columns.push_back(tempcol);
			tempcol = "";
		}
		temprow.isDelete = 0;
		if (Buffer.b[pos].block[j++] == '1')
			continue;
		else j++;
		flag = 0;
		for (m = 0; m < conditions.conds.size(); m++) {
			switch (conditions.conds[m].op) {
			case Gt:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].minValue);
				if (res <= 0)
					flag = 1;
				break;
			case Lt:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].maxValue);
				if (res >= 0)
					flag = 1;
				break;
			case Ge:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].minValue);
				if (res < 0)
					flag = 1;
				break;
			case Le:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].maxValue);
				if (res > 0)
					flag = 1;
				break;
			case Eq:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].eqValue);
				if (res != 0)
					flag = 1;
				break;
			case Ne:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].eqValue);
				if (res == 0)
					flag = 1;
				break;
			case LtLt:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].minValue);
				if (res <= 0)
					flag = 1;
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].maxValue);
				if (res >= 0)
					flag = 1;
				break;
			case LtLe:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].minValue);
				if (res <= 0)
					flag = 1;
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].maxValue);
				if (res > 0)
					flag = 1;
				break;
			case LeLt:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].minValue);
				if (res < 0)
					flag = 1;
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].maxValue);
				if (res >= 0)
					flag = 1;
				break;
			case LeLe:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].minValue);
				if (res < 0)
					flag = 1;
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[con_num[m]].maxValue);
				if (res > 0)
					flag = 1;
				break;
			default:
				break;
			}
			if (flag)
				break;

		}
		if (!flag) {
			for (int i = 0; i < temptable.attriNum; i++) {
				if (temptable.attributes[i].type == FLOAT)
					flag = ReFloat(temprow.columns[i]);
				if (temptable.attributes[i].type == CHAR)
					flag = ReChar(temprow.columns[i]);
			}
			result.rows.push_back(temprow);
		}
	}
	if (result.rows.size() == 0){
		temprow.isDelete = 2;
		result.rows.push_back(temprow);
	}
	return result;
}

IndexColumn RecorderManager::CreateIndexData(const Table &table, const Index &index)
{
	IndexColumn tempindexcol;
	IndexRow tempindexrow;
	Table temptable;
	Index tempindex;
	string tempcol;
	temptable = table;
	tempindex = index;
	temptable = cata.get_table_info(temptable);
	int frontLength = 0, column_num = 0; //该属性前数据的长度，以及属性是第几个
	int i, j, l, n;
	int start, pos;
	for (i = 0; i < temptable.attriNum; i++){
		if (temptable.attributes[i].name.compare(tempindex.attribute_name)==0){
			column_num = i;
			break;
		}
		else
			frontLength += temptable.attributes[i].length;
	}
	start = temptable.biggestnum / 1000000 * 1000000 + 1;
	for (i = start; i <= temptable.biggestnum; i++){
		pos = Buffer.malloc_block(i);
		l = 0;
		for (j = frontLength + l*temptable.totalLength; j < Buffer.b[pos].vaild_number; j = frontLength + l*temptable.totalLength){
			if (Buffer.b[pos].block[(l + 1)*temptable.totalLength - 1] == '1')
				continue;
			while (j < l *temptable.totalLength + frontLength + temptable.attributes[column_num].length)
				tempcol += Buffer.b[pos].block[j++];
			n = 0;
			if (temptable.attributes[column_num].type == INT){
				do{
					if (tempcol[n] == '0')
						n++;
					else break;
				} while (1);
				tempcol.erase(0, n);
			}
			tempindexrow.key = tempcol;
			tempindexrow.blockoffset = i;
			tempindexrow.fileoffset = l;//block中记录从第0条开始
			l++;
			tempindexcol.rows.push_back(tempindexrow);
			tempindexrow.key = "";
			tempcol = "";
		}
	}
	return tempindexcol;
}

Data RecorderManager::DeleteNoIndex(const Table &table, Conditiondata conditions)
{
	Table temptable = table;
	int start;
	Data result;
	int column_num[32] = { 0 };
	Row temprow;
	int i, j, k, l, m, n;
	int pos, flag, res;
	IndexColumn dindexdata;
	IndexRow tempindexrow;
	string tempcol;
	temptable = cata.get_table_info(temptable);
	start = temptable.biggestnum / 1000000 * 1000000 + 1;
	k = -1;
	for (i = 0; i < temptable.attriNum; i++){
		for (j = 0; j < conditions.conds.size(); j++){
			if (temptable.attributes[i].name.compare(conditions.conds[j].column_name) == 0)
				column_num[++k] = i;
			break;
		}
	}
	for (i = start; i <= temptable.biggestnum; i++){
		j = 0;
		pos = Buffer.malloc_block(i);
		for (; j < Buffer.b[pos].vaild_number;){
			temprow.columns.clear();
			for (k = 0; k < temptable.attriNum; k++){
				for (l = 0; l < temptable.attributes[k].length; l++){
					tempcol += Buffer.b[pos].block[j++];
				}
				n = 0;
				if (temptable.attributes[k].type == INT){
					do{
						if (tempcol[n] == '0')
							n++;
						else break;
					} while (1);
					tempcol.erase(0, n);
				}
				temprow.columns.push_back(tempcol);
				tempcol = "";
			}
			if (Buffer.b[pos].block[j] == '1'){
				j++;
				continue;
			}
			temprow.biggestnum = i;
			temprow.blockoffset = j / temptable.totalLength;
			flag = 0;
			for (m = 0; m < conditions.conds.size(); m++){
				switch (conditions.conds[m].op){
				case Gt:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].minValue);
					if (res <= 0)
						flag = 1;
					break;
				case Lt:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].maxValue);
					if (res >= 0)
						flag = 1;
					break;
				case Ge:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].minValue);
					if (res < 0)
						flag = 1;
					break;
				case Le:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].maxValue);
					if (res > 0)
						flag = 1;
					break;
				case Eq:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].eqValue);
					if (res != 0)
						flag = 1;
					break;
				case Ne:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].eqValue);
					if (res == 0)
						flag = 1;
					break;
				case LtLt:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].minValue);
					if (res <= 0)
						flag = 1;
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].maxValue);
					if (res >= 0)
						flag = 1;
					break;
				case LtLe:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].minValue);
					if (res <= 0)
						flag = 1;
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].maxValue);
					if (res > 0)
						flag = 1;
					break;
				case LeLt:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].minValue);
					if (res < 0)
						flag = 1;
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].maxValue);
					if (res >= 0)
						flag = 1;
					break;
				case LeLe:
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].minValue);
					if (res < 0)
						flag = 1;
					res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].maxValue);
					if (res > 0)
						flag = 1;
					break;
				default:
					break;
				}
				if (flag)
					break;
			}
			if (!flag){
				Buffer.b[pos].block[j++] = '1';
				temprow.isDelete = 1;
				result.rows.push_back(temprow);
			}
			else j++;
		}
	}
	return result;
}

Data RecorderManager::DeleteIndex(const Table &table, const IndexColumn &indexdata, Conditiondata conditions)
{
	Table temptable;
	Data result;
	int column_num[32] = { 0 };
	Row temprow;
	IndexColumn tempindexdata;
	int i, j, k, l, m, n;
	int pos, flag, res = 0;
	string tempcol;
	temptable = table;
	tempindexdata = indexdata;
	temptable = cata.get_table_info(temptable);
	k = -1;
	for (i = 0; i < temptable.attriNum; i++){
		for (j = 0; j < conditions.conds.size(); j++){
			if (temptable.attributes[i].name.compare(conditions.conds[j].column_name) == 0)
				column_num[++k] = i;
			break;
		}
	}
	for (i = 0; i < tempindexdata.rows.size(); i++){
		pos = Buffer.malloc_block(tempindexdata.rows[i].blockoffset);
		j = tempindexdata.rows[i].fileoffset*temptable.totalLength;
		temprow.columns.clear();
		for (k = 0; k < temptable.attriNum; k++){
			for (l = 0; l < temptable.attributes[k].length; l++){
				tempcol += Buffer.b[pos].block[j++];
			}
			n = 0;
			if (temptable.attributes[k].type == INT){
				do{
					if (tempcol[n] == '0')
						n++;
					else break;
				} while (1);
				tempcol.erase(0, n);
			}
			temprow.columns.push_back(tempcol);
			tempcol = "";
		}
		temprow.biggestnum = tempindexdata.rows[i].blockoffset;
		temprow.blockoffset = j / temptable.totalLength;
		flag = 0;
		for (m = 0; m < conditions.conds.size(); m++){
			switch (conditions.conds[m].op){
			case Gt:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].minValue);
				if (res <= 0)
					flag = 1;
				break;
			case Lt:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].maxValue);
				if (res >= 0)
					flag = 1;
				break;
			case Ge:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].minValue);
				if (res < 0)
					flag = 1;
				break;
			case Le:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].maxValue);
				if (res > 0)
					flag = 1;
				break;
			case Eq:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].eqValue);
				if (res != 0)
					flag = 1;
				break;
			case Ne:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].eqValue);
				if (res == 0)
					flag = 1;
				break;
			case LtLt:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].minValue);
				if (res <= 0)
					flag = 1;
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].maxValue);
				if (res >= 0)
					flag = 1;
				break;
			case LtLe:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].minValue);
				if (res <= 0)
					flag = 1;
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].maxValue);
				if (res > 0)
					flag = 1;
				break;
			case LeLt:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].minValue);
				if (res < 0)
					flag = 1;
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].maxValue);
				if (res >= 0)
					flag = 1;
				break;
			case LeLe:
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].minValue);
				if (res < 0)
					flag = 1;
				res = calculate(temptable.attributes[column_num[m]].type, temprow.columns[column_num[m]], conditions.conds[m].maxValue);
				if (res > 0)
					flag = 1;
				break;
			default:
				break;
			}
			if (flag)
				break;
		}
		if (!flag){
			temprow.isDelete = 1;
			Buffer.b[pos].block[j++] = '1';
			result.rows.push_back(temprow);
		}
		else j++;
	}
	return result;
}

IndexColumn RecorderManager::GetIndexData(const Table &table, const Index &index, const Data &data)
{
	Table temptable;
	Index tempindex;
	IndexColumn dindexdata;
	IndexRow tempindexrow;
	int indexpos = 0;
	int i;
	temptable = cata.get_table_info(table);
	tempindex = index;
	for (i = 0; i < temptable.attriNum; i++){
		if (temptable.attributes[i].name.compare(tempindex.attribute_name) == 0){
			indexpos = i;
			break;
		}
	}
	for (i = 0; i < data.rows.size(); i++){
		tempindexrow.key = data.rows[i].columns[indexpos];
		tempindexrow.blockoffset = data.rows[i].biggestnum;
		tempindexrow.fileoffset = data.rows[i].blockoffset;
		dindexdata.rows.push_back(tempindexrow);
	}
	return dindexdata;
}

bool RecorderManager::InsertDelValue(const Table &table, const Data &data)//API完成新表的属性建立（biggestnum，offset，istaken）
{
	Table temptable = table;
	int i, j, k, pos;
	Data tempdata = data;
	char tempbig[8];
	char tempoff[4];
	bool flag = false;
	int templen;
	temptable = cata.get_table_info(table); 
	pos = Buffer.malloc_block(temptable.biggestnum);
	for (i = 0; i < tempdata.rows.size(); i++){
		_itoa(data.rows[i].biggestnum, tempbig, 10);
		_itoa(data.rows[i].blockoffset, tempoff, 10);
		if (strlen(tempoff) < 3){
			templen = strlen(tempoff);
			for (j = templen; j >= 0; j--)
				tempoff[j + 3 - templen]= tempoff[j];
			for (j = 0; j < 3 - templen; j++)
				tempoff[j] = '0';
		}
		if (!flag){
			for (k = 9; k <= Buffer.b[pos].vaild_number;){
				if (Buffer.b[pos].block[k] == '0'){
					k += 10;
					continue;
				}
				k -= 9;
				for (j = 0; j < 7; j++)
					Buffer.b[pos].block[k++] = tempbig[j];
				for (j = 0; j < 3; j++)
					Buffer.b[pos].block[k++] = tempoff[j];
				Buffer.b[pos].block[k] = '0';
				Buffer.b[pos].isWritten = 1;
				break;
			}
		}
		if (k == Buffer.b[pos].vaild_number)
			flag = true;
		if (Buffer.b[pos].vaild_number == 0){
			Buffer.b[pos].vaild_number = -1;
			flag = true;
		}
		if (flag){
			if (Buffer.b[pos].vaild_number + 10 <= block_size){
				for (j = 0; j < 7; j++)
					Buffer.b[pos].block[++Buffer.b[pos].vaild_number] = tempbig[j];
				for (j = 0; j < 3; j++)
					Buffer.b[pos].block[++Buffer.b[pos].vaild_number] = tempoff[j];
				Buffer.b[pos].block[++Buffer.b[pos].vaild_number] = '0';
				Buffer.b[pos].isWritten = 1;
			}
			else{
				cata.add_Lblockindex(temptable);
				pos = Buffer.malloc_block(temptable.biggestnum);
				for (j = 0; j < 7; j++)
					Buffer.b[pos].block[++Buffer.b[pos].vaild_number] = tempbig[j];
				for (j = 0; j < 3; j++)
					Buffer.b[pos].block[++Buffer.b[pos].vaild_number] = tempoff[j];
				Buffer.b[pos].block[++Buffer.b[pos].vaild_number] = '0';
				Buffer.b[pos].isWritten = 1;
			}
		}
	}
	return true;
}

int RecorderManager::calculate(int type, string s1, string s2)
{
	int res, num1, num2;
	num1 = num2 = 0;
	if (type == CHAR){
		ReChar(s1);
		ReChar(s2);
		res = s1.compare(s2);
	}
	else if (type == FLOAT){
		num1 = num2 = 0;
		do{
			if (s1[num1] == '0' && (s1[num1 + 1] != ',' || s1[num1 + 1] != '-'))
				num1++;
			else break;
		} while (1);
		s1.erase(0, num1);
		do{
			if (s2[num2] == '0' && (s2[num2 + 1] != ',' || s2[num2 + 1] != '-'))
				num2++;
			else break;
		} while (1);
		s2.erase(0, num2);
		res = s1.compare(s2);
	}
	else if (type = INT){
		res = s1.compare(s2);
	}
	return res;
}

bool ReFloat(string &src){
	int i, n, flag;
	string temp = src;
	n = flag = 0;
	for (i = 0; i < src.length(); i++){
		if (temp[i] == '0' && (temp[i + 1] != '.' || temp[i + 1] != '-'))
			n++;
		else
			break;
	}
	for (i = 0; i < src.length(); i++){
		if (temp[i] == '-' && (i != 0 && temp[i - 1] != '0'))
			flag = i;
	}
	if (flag == 0)
		temp.erase(0, n);
	else{
		temp[flag] = '.';
		temp.erase(0, n);
		temp.insert(0, "-");
	}
	src = temp;
	return true;
}

bool ReChar(string &src)
{
	string temp = src;
	int n = 0;
	do{
		if (temp[n] == 1)
			n++;
		else
			break;
	} while (1);
	temp.erase(0, n);
	src = temp;
	return true;
}