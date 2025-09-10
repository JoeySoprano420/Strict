strictc/
 ├─ CMakeLists.txt
 ├─ src/
 │   ├─ lexer.cpp
 │   ├─ parser.cpp
 │   ├─ ast.cpp
 │   ├─ codegen_llvm.cpp
 │   ├─ dgm_translator.cpp
 │   ├─ dgm_emitter.cpp
 │   ├─ runtime.c
 │   └─ main.cpp
 ├─ include/
 │   ├─ lexer.hpp
 │   ├─ parser.hpp
 │   ├─ ast.hpp
 │   ├─ codegen.hpp
 │   ├─ dgm.hpp
 ├─ examples/
 │   ├─ hello.strict
 │   ├─ math.strict
 │   ├─ match.strict
 │   ├─ oop.strict
 │   └─ generics.strict
 ├─ tests/
 │   ├─ run_tests.sh
 │   └─ expected_outputs/
 │       ├─ hello.txt
 │       ├─ math.txt
 │       ├─ match.txt
 │       ├─ oop.txt
 │       └─ generics.txt
 └─ .github/
     └─ workflows/
         └─ build.yml
