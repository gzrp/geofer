
# 秘钥管理

Geofer 使用DuckDB的秘密管理器来安全地存储和管理敏感信息，如API密钥和凭据。
Secret 由服务提供者键入，可以是临时的（在内存中），也可以是持久的（在磁盘上）。

支持的秘钥类型包括
- OpenAI
- Ollma
- Azure

创建临时秘钥, 默认名称 __default_openai
```sql
-- __default_openai
CREATE SECRET (TYPE OPENAI, API_KEY 'your-api-key');
-- __default_ollama
CREATE SECRET (TYPE OLLAMA, API_URL '127.0.0.1:11434');
-- __default_azure
CREATE SECRET (TYPE AZURE_LLM, API_KEY 'your-key-here', RESOURCE_NAME 'resource-name',API_VERSION 'api-version');
```

创建持久化秘钥
```sql
CREATE PERSISTENT SECRET (TYPE OPENAI, API_KEY 'your-api-key');
CREATE PERSISTENT SECRET (TYPE OLLAMA, API_URL '127.0.0.1:11434');
CREATE PERSISTENT SECRET (TYPE AZURE_LLM, API_KEY 'your-key-here', RESOURCE_NAME 'resource-name',API_VERSION 'api-version');
```

删除临时秘钥：名称模型 __default_<provider_name>
```sql
-- __default_openai, __default_ollama, or __default_azure
DROP TEMPORARY SECRET your_secret_name;
```

删除持久化秘钥
```sql
-- __default_openai, __default_ollama, or __default_azure
DROP PERSISTENT SECRET your_secret_name;
```

列举所有秘钥
```sql
FROM duckdb_secrets();
```