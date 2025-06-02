


llm_complete函数使用指定的模型生成文本补全，并提示动态数据生成。


使用方式

```sql
SELECT llm_complete(
    {'model_name': 'gpt-4'},
    {'prompt': 'Explain the purpose of GeoFer.'}
) AS flockmtl_purpose;
```

本例使用gpt-4模型的文本补全, prompt 为 xplain the purpose of GeoFer。该函数根据提供的提示返回每行的补全。


```sql
-- 对于 products 表中的每一行 product_name, 查到值后，将值代入 llm_complete 中
SELECT llm_complete(
    {'model_name': 'summarizer', 'secret_name': 'summarizer_secret'},
    {'prompt_name': 'description-generation', 'version': 1},
    {'product_name': product_name}
) AS product_description
FROM products;
```
对产品表中的 product_name 列的数据生成产品描述

```sql
WITH enhanced_products AS (
    SELECT
        product_id,
        product_name,
        llm_complete(
            {'model_name': 'reduce-model'},
            {'prompt_name': 'summarize-content', 'version': 2},
            {'product_name': product_name}
        ) AS enhanced_description
    FROM products
)
SELECT product_id, product_name, enhanced_description
FROM enhanced_products
WHERE LENGTH(enhanced_description) > 50;
```

它处理product_name列并生成汇总的描述。然后，查询过滤掉生成的描述短于50个字符的行。


llm_complete函数接受三种结构化输入：模型配置、提示配置和输入数据列。

模型配置：model_name 和 secret_name
```sql
{ 'model_name': 'gpt-4', 'secret_name': 'your_secret_name' }
```

提示配置
- 可以直接指定 prompt:  
```sql
{ 'prompt': 'Summarize the content of the article.' }
```

也可以使用具名prompt
```sql
{ 'prompt_name': 'summarize-content' }
{ 'prompt_name': 'summarize-content', 'version': 2 }
```

输入数据列：指定表中的列作为输入传递给模型。

```sql
{'product_name': product_name, 'product_description': product_description}
```