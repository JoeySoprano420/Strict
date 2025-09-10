import os
import urllib.request

# Base GitHub raw link
BASE = "https://raw.githubusercontent.com/JoeySoprano420/Strict/main/"

# Files we want to pull
FILES = [
    "CMakeLists.txt",
    "src/lexer.cpp",
    "src/parser.cpp",
    "src/ast.cpp",
    "src/codegen_llvm.cpp",
    "src/dgm_translator.cpp",
    "src/dgm_emitter.cpp",
    "src/runtime.c",
    "src/main.cpp",
    "include/lexer.hpp",
    "include/parser.hpp",
    "include/ast.hpp",
    "include/codegen.hpp",
    "include/dgm.hpp",
    "examples/hello.strict",
    "examples/math.strict",
    "examples/match.strict",
    "examples/oop.strict",
    "examples/generics.strict",
    "tests/run_tests.sh",
    "tests/expected_outputs/hello.txt",
    "tests/expected_outputs/math.txt",
    "tests/expected_outputs/match.txt",
    "tests/expected_outputs/oop.txt",
    "tests/expected_outputs/generics.txt",
    ".github/workflows/build.yml",
]

print("=== Creating StrictC Compiler Repo ===")

for path in FILES:
    url = BASE + path
    dest = os.path.join("strictc", path)
    os.makedirs(os.path.dirname(dest), exist_ok=True)
    print(f"Downloading {url} -> {dest}")
    try:
        urllib.request.urlretrieve(url, dest)
    except Exception as e:
        print(f"  !! Failed to fetch {url}: {e}")

# Make test script executable on Unix
try:
    os.chmod("strictc/tests/run_tests.sh", 0o755)
except:
    pass

print("=== Repo created under ./strictc ===")
print("Next steps:")
print("  cd strictc")
print("  cmake -S . -B build")
print("  cmake --build build --config Release")
