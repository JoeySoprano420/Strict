#!/bin/bash
set -e

echo "=== Creating StrictC Compiler Repo ==="

mkdir -p strictc/src strictc/include strictc/examples strictc/tests/expected_outputs strictc/.github/workflows

# ---------- Root ----------
cat > strictc/CMakeLists.txt <<'EOF'
<PASTE CONTENTS OF CMakeLists.txt HERE>
EOF

# ---------- Source ----------
cat > strictc/src/lexer.cpp <<'EOF'
<PASTE CONTENTS OF src/lexer.cpp HERE>
EOF

cat > strictc/src/parser.cpp <<'EOF'
<PASTE CONTENTS OF src/parser.cpp HERE>
EOF

cat > strictc/src/ast.cpp <<'EOF'
<PASTE CONTENTS OF src/ast.cpp HERE>
EOF

cat > strictc/src/codegen_llvm.cpp <<'EOF'
<PASTE CONTENTS OF src/codegen_llvm.cpp HERE>
EOF

cat > strictc/src/dgm_translator.cpp <<'EOF'
<PASTE CONTENTS OF src/dgm_translator.cpp HERE>
EOF

cat > strictc/src/dgm_emitter.cpp <<'EOF'
<PASTE CONTENTS OF src/dgm_emitter.cpp HERE>
EOF

cat > strictc/src/runtime.c <<'EOF'
<PASTE CONTENTS OF src/runtime.c HERE>
EOF

cat > strictc/src/main.cpp <<'EOF'
<PASTE CONTENTS OF src/main.cpp HERE>
EOF

# ---------- Include ----------
cat > strictc/include/lexer.hpp <<'EOF'
<PASTE CONTENTS OF include/lexer.hpp HERE>
EOF

cat > strictc/include/parser.hpp <<'EOF'
<PASTE CONTENTS OF include/parser.hpp HERE>
EOF

cat > strictc/include/ast.hpp <<'EOF'
<PASTE CONTENTS OF include/ast.hpp HERE>
EOF

cat > strictc/include/codegen.hpp <<'EOF'
<PASTE CONTENTS OF include/codegen.hpp HERE>
EOF

cat > strictc/include/dgm.hpp <<'EOF'
<PASTE CONTENTS OF include/dgm.hpp HERE>
EOF

# ---------- Examples ----------
cat > strictc/examples/hello.strict <<'EOF'
<PASTE CONTENTS OF examples/hello.strict HERE>
EOF

cat > strictc/examples/math.strict <<'EOF'
<PASTE CONTENTS OF examples/math.strict HERE>
EOF

cat > strictc/examples/match.strict <<'EOF'
<PASTE CONTENTS OF examples/match.strict HERE>
EOF

cat > strictc/examples/oop.strict <<'EOF'
<PASTE CONTENTS OF examples/oop.strict HERE>
EOF

cat > strictc/examples/generics.strict <<'EOF'
<PASTE CONTENTS OF examples/generics.strict HERE>
EOF

# ---------- Tests ----------
cat > strictc/tests/run_tests.sh <<'EOF'
<PASTE CONTENTS OF tests/run_tests.sh HERE>
EOF
chmod +x strictc/tests/run_tests.sh

cat > strictc/tests/expected_outputs/hello.txt <<'EOF'
<PASTE CONTENTS OF tests/expected_outputs/hello.txt HERE>
EOF

cat > strictc/tests/expected_outputs/math.txt <<'EOF'
<PASTE CONTENTS OF tests/expected_outputs/math.txt HERE>
EOF

cat > strictc/tests/expected_outputs/match.txt <<'EOF'
<PASTE CONTENTS OF tests/expected_outputs/match.txt HERE>
EOF

cat > strictc/tests/expected_outputs/oop.txt <<'EOF'
<PASTE CONTENTS OF tests/expected_outputs/oop.txt HERE>
EOF

cat > strictc/tests/expected_outputs/generics.txt <<'EOF'
<PASTE CONTENTS OF tests/expected_outputs/generics.txt HERE>
EOF

# ---------- GitHub Workflow ----------
cat > strictc/.github/workflows/build.yml <<'EOF'
<PASTE CONTENTS OF .github/workflows/build.yml HERE>
EOF

echo "=== Repo created under ./strictc ==="
echo "Next steps:"
echo "  cd strictc"
echo "  cmake -S . -B build"
echo "  cmake --build build --config Release"
