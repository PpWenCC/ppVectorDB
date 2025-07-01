
# ppVectorDB

  

dev env:
- c/c++ 14
- python 3.9.0
- openEuler 24.03
- blas
- Swig

  
  

third party lib:
- spdlog
- faiss
- cpp-httplib
- rapidjson
- hnswlib
- gtest(TODO)

  

## Run
当前无ST进行测试，手动执行可修改main函数中的dim值指定向量数据的维度

compiler

    sh build.sh


run example
```
    cd bin/
    ./ppVectorDB
```

 1. insert
	

    curl -X POST -H "Content-Type: application/json" -d '{"vector":[0.4], "id":4, "indexType":"FLAT"}' http://localhost:8080/insert

 2. search
 

    curl -X POST -H "Content-Type: application/json" -d '{"vector":[0.3], "k":3, "indexType":"FLAT"}' http://localhost:8080/search
