
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

  

## Run

compiler

    sh build.sh

run example

 1. insert
	

    curl -X POST -H "Content-Type: application/json" -d '{"test":test}' http://localhost:8080/insert

 2. search
 

    curl -X POST -H "Content-Type: application/json" -d '{"test":test}' http://localhost:8080/search
