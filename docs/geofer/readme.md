


## 编译步骤





## 执行步骤

一、进入 duckdb
```bash
# 进入 duckdb
cd ./build/release
./duckdb geofer.db
```

二、查看可用扩展
```sql
SELECT extension_name, installed, description
FROM duckdb_extensions(),
```

三、基础命令

输出数据库
```sql
SHOW databases,
```

输出当前 schema 的表
```sql
SHOW tables,
```

输入所有的表
```sql
SHOW ALL tables
```


查询当前 schema
```sql
select current_schema(),
```

设置当前 schema
```sql
SET schema=geofer_config,
```

四、图片推理

准备数据
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
INSERT INTO image_table
SELECT
    'image2.jpg' AS image_name,
    content AS image,
    'image2 描述' AS image_desc
FROM read_blob('/home/zrp/workdir/image1.jpg');
```

spatial 经纬度扩展
```sql
INSTALL spatial;    
LOAD spatial;
CREATE TABLE geo_table (
   id TEXT PRIMARY KEY,
   name TEXT,
   address TEXT,
   location GEOMETRY,
   pcode INTEGER,
   adcode INTEGER,
   pname TEXT,
   cityname TEXT,
   type TEXT,
   typecode TEXT,
   adname TEXT,
   citycode INTEGER
);
```

导入数据
```sql
.read init.sql
.read geo_hk.sql
.read geo_bk.sql
```


SQL 验证
```sql
WITH geo_a AS ( 
    SELECT * FROM geo_table WHERE name LIKE '%好客连锁%'
), geo_b AS ( 
    SELECT * FROM geo_table WHERE name LIKE '%银行%'
)
SELECT 
    a.name AS a_name, 
    a.address AS a_address, 
    b.name AS b_name, 
    b.address AS b_address, 
    ROUND((st_distance(a.location, b.location) / 0.0111) * 1000) AS distance 
FROM geo_a AS a 
JOIN geo_b AS b ON 1=1
WHERE 1=1 AND ST_X(a.location) <= ST_X(b.location)
ORDER BY distance
LIMIT 5;
```

```sql
.mode duckbox    -- 默认模式
.mode column
.mode csv       -- csv 模式
.mode json     -- json 模式  内容完整
.mode line     --  一行 一条数据， 内容完整，推荐使用

SELECT image_name AS image, image_analyze(image, image_desc) AS result
FROM image_table
WHERE image_name = 'image1.jpg';

SELECT image_name AS image, image_analyze(image, image_desc) AS result
FROM image_table
WHERE image_name = 'image1.jpg' or image_name = 'image2.jpg';
```


```sql
COPY (
    SELECT image_analyze(image, image_desc) AS result
    FROM image_table
    WHERE image_name = 'image1.jpg'
) TO 'output.json' (FORMAT JSON);
```


curl --location --request POST 'http://192.168.56.1:8080/api/v1/image_analyze' \
--form 'description='xxx'' \
--form 'file=@'/home/zrp/workdir/image1.jpg''
