# Mini-Database
## Basic Information
This is a mini database project, developed with C++. It supports basic SQL instructions such as ```create table```, ```create index```, ```drop```, ```delete```, ```select```.

The architecture is shown in the following figure. The system consists of different components: API, Interpreter, Record Manager, Index Manager, Catalog Manager, Buffer Manager and DB Files. We use binary files to store data.

![alt image](https://raw.githubusercontent.com/yangjufo/Mini-Database/master/readme/architecture.png)

## Tool
We use Visual Studio 2015 to build the system.

## Components
### API
Provide the interface of performing SQL instructions. 
> 1. An input to Interpreter => build an inner data structure
> 2. Receive information from Catalog Manager => decide how to perform
> 3. Call Record Manager, Index Manager, Catalog Manager => perform
> 4. Return results to Interpreter

### Interpreter
1. Control the flow
>Start -> accept, process, show instructions -> quit
2. Check semantic and grammar

### Catalog Manager
Manage all model information; store information in XML files.
> 1. Definition of table
> 2. Definition of attribute
> 3. Definition of index

### Record Manager
Manage data files.
> 1. create/delete data files
> 2. insert/delete records
> 3. query records

### Index Manager
Implement B+ Tree Indexes.

### Buffer Manager
Manage the buffer area.
>1. Read/Write data blocks
> 2. Implement a replacement algorithm.

### DB Files
Store catalog, record and index.

## Demo
### Create and Insert
![alt image](https://raw.githubusercontent.com/yangjufo/Mini-Database/master/readme/create&insert.PNG)

### Delete and Drop
![alt image](https://raw.githubusercontent.com/yangjufo/Mini-Database/master/readme/delete&drop%20table.PNG)

### Select
![alt image](https://raw.githubusercontent.com/yangjufo/Mini-Database/master/readme/select.PNG)
