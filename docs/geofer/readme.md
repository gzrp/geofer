


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




```sql
CREATE TABLE image_table (
    image_name TEXT,         -- 图片名
    image BLOB,              -- 图片二进制数据
    image_desc TEXT          -- 图片描述信息
);

INSERT INTO image_table
    SELECT
    'image1.jpg' AS image_name,
    content AS image,
    'image1 描述' AS image_desc
FROM read_blob('/home/zrp/workdir/image1.jpg');

```

```sql
SELECT image_analyze(image, image_desc)
FROM image_table
WHERE image_name = 'image1.jpg';


```


curl --location --request POST 'http://192.168.56.1:8080/api/v1/image_analyze' \
--form 'description="xxx"' \
--form 'file=@"/home/zrp/workdir/image1.jpg"'
