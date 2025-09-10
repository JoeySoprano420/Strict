---

# 1) Prereqs (install once)

## Windows (recommended: “Developer Command Prompt for VS”)

* **Visual Studio Build Tools 2022+** (C++ Desktop workload)
* **CMake**: `choco install cmake -y`
* **LLVM**: `choco install llvm -y`
* **NASM**: `choco install nasm -y`
* Make sure these are on your `PATH` (the Chocolatey installs do this automatically).

## Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake llvm clang nasm
```

## macOS (Homebrew)

```bash
brew install cmake llvm nasm
```

> **Note (Linux/macOS):** If CMake can’t find LLVM, set `CMAKE_PREFIX_PATH`:
>
> * Linux (typical): `export CMAKE_PREFIX_PATH=/usr/lib/llvm-$(llvm-config --version)`
> * macOS (Homebrew): `export CMAKE_PREFIX_PATH="$(brew --prefix llvm)"`
>
> You can also set `LLVM_DIR` directly to `.../lib/cmake/llvm`.

---

# 2) Create the repo structure

From any workspace folder:

```bash
mkdir -p strictc/src strictc/include strictc/examples strictc/tests/expected_outputs strictc/.github/workflows
```

---

# 3) Add each file with the content you already have

Create these files (exact paths):

```
strictc/CMakeLists.txt
strictc/src/lexer.cpp
strictc/src/parser.cpp
strictc/src/ast.cpp
strictc/src/codegen_llvm.cpp
strictc/src/dgm_translator.cpp
strictc/src/dgm_emitter.cpp
strictc/src/runtime.c
strictc/src/main.cpp
strictc/include/lexer.hpp
strictc/include/parser.hpp
strictc/include/ast.hpp
strictc/include/codegen.hpp
strictc/include/dgm.hpp
strictc/examples/hello.strict
strictc/examples/math.strict
strictc/examples/match.strict
strictc/examples/oop.strict
strictc/examples/generics.strict
strictc/tests/run_tests.sh
strictc/tests/expected_outputs/hello.txt
strictc/tests/expected_outputs/math.txt
strictc/tests/expected_outputs/match.txt
strictc/tests/expected_outputs/oop.txt
strictc/tests/expected_outputs/generics.txt
strictc/.github/workflows/build.yml
```

Paste the file contents from our previous messages into each. (Keep names/paths exactly the same.)

---

# 4) Apply two tiny fixes (so the pipeline is smooth)

These are minimal, mechanical edits to make everything build and run out-of-the-box across platforms.

### (A) Add `getModule()` implementation

Append this **one method** to the end of `strictc/src/codegen_llvm.cpp` (below `emitIR`):

```cpp
llvm::Module* ProgramAST::getModule() {
    return TheModule.get();
}
```

### (B) Make the NASM emitter’s epilogue OS-aware

In `strictc/src/dgm_emitter.cpp`, replace the hardcoded Linux `sys_exit` footer with a platform-aware one:

```cpp
// Exit / return to caller
#ifdef _WIN32
    out << "    ret\n"; // return to CRT on Windows
#elif __APPLE__
// macOS: use exit(0) via syscall
    out << "    mov rax, 0x2000001\n"; // sys_exit
    out << "    xor rdi, rdi\n";
    out << "    syscall\n";
#else
// Linux: use exit(0) via syscall
    out << "    mov rax, 60\n"; // sys_exit
    out << "    xor rdi, rdi\n";
    out << "    syscall\n";
#endif
```

> Why: Our assembly declares `global main`. On Windows, the CRT calls `main` and expects it to `ret` back; on Linux/macOS we can end with a syscall to `exit(0)`.

---

# 5) Optional (Windows): add a one-click builder

Create `strictc/install.bat`:

```bat
@echo off
setlocal
echo === Building Strict Compiler (strictc) ===
if not exist build mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
cd ..
echo === Compiling runtime.c to runtime.obj (for linking generated programs) ===
call "%VSINSTALLDIR%VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
cl /nologo /c src\runtime.c /Fo:src\runtime.obj
echo === Done ===
echo You can now run:
echo   build\Release\strictc.exe examples\hello.strict -o hello.exe
pause
```

> If `vcvars64.bat` isn’t found, open the “Developer Command Prompt for VS” first, then run `install.bat`.

---

# 6) Configure & build the compiler

## Windows

* Open **Developer Command Prompt for VS**.

```bat
cd path\to\strictc
install.bat
```

This builds `strictc.exe` and also produces `src\runtime.obj`, which the one-shot compile pipeline uses when linking your generated programs.

## Linux/macOS

```bash
cd strictc
cmake -S . -B build
cmake --build build --config Release
```

---

# 7) Compile & run an example

## Windows

```bat
cd strictc
build\Release\strictc.exe examples\hello.strict -o hello.exe
hello.exe
```

Sample interaction (enter a number such as `5` when prompted):

```
Enter a number:
5
Square is:
25
Loop iteration:
1
...
```

## Linux/macOS

```bash
cd strictc
./build/strictc examples/hello.strict -o hello
./hello
```

> On Linux/macOS, the toolchain will also produce `hello.ll`, `hello.dgm`, and `hello.s`.
> On Windows, it will do the same and call NASM + link automatically.

---

# 8) Running the test suite (Linux/macOS)

```bash
cd strictc/tests
chmod +x run_tests.sh
./run_tests.sh
```

> Note: The current examples are larger than the minimal backend actually compiles end-to-end (e.g., `Safe.*`, full `Match`/strings, OOP, and Generics were specified, but the backend we wrote is a minimal demonstrator). Start with `hello.strict` to validate your toolchain, then expand.

---

# 9) Create a ZIP for one-click sharing

From the directory **that contains** the `strictc/` folder:

* **Windows (PowerShell):**

  ```powershell
  Compress-Archive -Path strictc -DestinationPath strictc.zip
  ```
* **Linux/macOS:**

  ```bash
  zip -r strictc.zip strictc/
  ```

This produces `strictc.zip` ready to share or attach.

---

# 10) (Optional) Push to GitHub with CI

1. Create a GitHub repo named `strictc`.
2. Copy the `strictc/` contents into it and push.
3. The included workflow at `.github/workflows/build.yml` will:

   * Build on **Windows, Linux, macOS**
   * Run tests on Linux/macOS
   * Upload a Windows ZIP artifact automatically

---

## Troubleshooting quick hits

* **CMake can’t find LLVM**
  Set one of:

  ```bash
  # Linux
  export CMAKE_PREFIX_PATH=/usr/lib/llvm-$(llvm-config --version)
  # macOS
  export CMAKE_PREFIX_PATH="$(brew --prefix llvm)"
  # Or point directly:
  export LLVM_DIR=".../lib/cmake/llvm"
  ```

* **`link.exe` not found (Windows)**
  Open the **Developer Command Prompt for VS** or call `vcvars64.bat` first (the `install.bat` tries to handle this).

* **`strict_print`/`strict_input` unresolved (Windows)**
  Make sure you compiled `src\runtime.c` to `src\runtime.obj` once (the `install.bat` does that) **before** building your first program.

* **NASM errors at the very end of `.s`**
  Ensure you applied the OS-specific epilogue change in `dgm_emitter.cpp` (step 4B).

---

