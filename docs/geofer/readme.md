


## 编译步骤





## 执行步骤

一、进入 duckdb
```bash
# 进入 duckdb
cd ./build/release
./duckdb
```

二、查看可用扩展
```sql
SELECT extension_name, installed, description
FROM duckdb_extensions();
```

三、基础命令

输出数据库
```sql
SHOW databases;
```

输出当前 schema 的表
```sql
SHOW tables;
```

输入所有的表
```sql
SHOW ALL tables
```


查询当前 schema
```sql
select current_schema();
```

设置当前 schema
```sql
SET schema=geofer_config;
```
