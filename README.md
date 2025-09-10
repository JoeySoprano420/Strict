# Strict

---

## 🛠 Extend `.github/workflows/build.yml`

Add a **deploy job** after `release`:

```yaml
  pages:
    needs: release
    runs-on: ubuntu-latest
    permissions:
      contents: write
      pages: write
      id-token: write

    steps:
      - uses: actions/checkout@v4

      - name: Setup Pages
        uses: actions/configure-pages@v4

      - name: Build Site
        run: |
          mkdir -p site
          cp index.html site/
          # (Optional: copy CSS, logos, JS if split into assets/)
          echo "get.hypershade.org" > site/CNAME

      - name: Upload artifact
        uses: actions/upload-pages-artifact@v3
        with:
          path: site

      - name: Deploy to GitHub Pages
        uses: actions/deploy-pages@v4
```

---

## 📦 What This Does

1. **Trigger:** Runs after every tagged release (`vX.Y.Z`).
2. **Build Site:** Copies `index.html` (and assets) into a `site/` directory.
3. **Publish:** Deploys to GitHub Pages (`gh-pages` branch) automatically.
4. **Custom Domain:** If you point DNS → GitHub Pages, the `CNAME` file ensures it’s served at `get.hypershade.org`.

---

## ⚡ Benefits

* **Always up-to-date:** Landing page auto-refreshes with each release.
* **One source of truth:** GitHub API fetch inside `index.html` ensures buttons + version always match the newest release.
* **No manual deploys:** CI/CD handles publishing automatically.

---

## 📦 Final User Experience

* **Developers:**

  ```bash
  curl -fsSL https://get.hypershade.org/install.sh | bash
  winget install Hypershade.Hypershade
  brew install ORG/hypershade/hypershade
  choco install hypershade
  snap install hypershade --classic
  flatpak install flathub org.hypershade.hscc
  ```

* **New users:**

  * Visit **get.hypershade.org**
  * Site auto-detects OS + shows correct snippet
  * OS buttons link to **latest release artifacts**
  * Version + release notes pulled directly from GitHub API

---


---

# 📄 Strict Whitepaper

### *The Disciplined Language for Clear Logic and Fearless Execution*

---

## Abstract

**Strict** is a modern programming language designed to unify **readable pseudocode clarity** with **machine-level efficiency and safety**. Its syntax enforces discipline and explicit structure while its compiler pipeline targets LLVM IR, DGM (Dodecagram), and NASM x64 assembly, ensuring safety and performance across platforms. This whitepaper presents the design motivations, architecture, language features, safety model, and ecosystem of Strict, arguing its suitability for real-world adoption in mission-critical domains.

---

## 1. Introduction

Programming languages often compromise between **readability** and **performance**. Strict rejects this compromise. It offers a **disciplined, end-terminated syntax** designed for clarity, while providing a **full optimizing compiler** that outputs **native machine code** without hidden runtime overhead.

Strict enforces **structural correctness, guarded arithmetic, and safe memory semantics**. It maps directly to a **144-opcode DGM model**, ensuring transparent compilation to LLVM IR and NASM x64 assembly.

This paper introduces:

* **Design Philosophy** (discipline, readability, safety, performance)
* **Language Features** (syntax, safety arithmetic, match, OOP, generics, concurrency)
* **Compiler Architecture** (AST → LLVM IR → DGM → NASM)
* **Ecosystem** (package manager, tooling, CI)
* **Adoption Contexts** (education, systems programming, mission-critical industries)

---

## 2. Design Philosophy

Strict is built on four pillars:

1. **Discipline First**

   * Blocks always close with `End`.
   * Explicit keywords (`Let`, `Func`, `Class`, `Match`).
   * No implicit coercion or silent fallthrough.

2. **Human-Readable, Machine-Ready**

   * Syntax mirrors pseudocode and structured English.
   * Direct translation to LLVM IR and NASM ensures efficiency.

3. **Fearless Execution**

   * Guarded safe arithmetic (`Safe.Add`, `Safe.Div`).
   * Exhaustive `Match` checking with compiler diagnostics.
   * Checked arrays and lists.

4. **Performance Without Compromise**

   * One-to-one DGM → IR → NASM mapping.
   * Zero-cost abstractions for OOP, generics, concurrency.
   * Fully linkable with C/C++ libraries.

---

## 3. Language Features

### 3.1 Syntax and Blocks

* Pseudocode clarity with structural keywords.
* Universal `End` terminator ensures unambiguous scoping.

```strict
Func Square(n)
    Return n * n
End
```

### 3.2 Safety Arithmetic

* `Safe.Add`, `Safe.Sub`, `Safe.Mul`, `Safe.Div` prevent overflow and invalid ops.

### 3.3 Match Expressions

* Multi-pattern branching with ranges, relational guards, and wildcard.

```strict
Match x
    Case 0: Print "None"
    Case 1..5: Print "Few"
    Case 6..100: Print "Many"
    Case <0: Print "Negative"
    Case *: Print "Other"
End
```

### 3.4 Object-Oriented Model

* Classes, inheritance, encapsulation, polymorphism.

### 3.5 Generics & Templates

* Generic classes (`Box<T>`) and compile-time templates (`Max<T>`).

### 3.6 Concurrency

* `Future`, `Parallel`, and `Sync` for deterministic parallelism.

---

## 4. Compiler Architecture

Strict’s compiler implements a **three-stage pipeline** (see Figure 1):

1. **Front-End**: Lexer → Parser → AST
2. **Middle-End**: AST → LLVM IR → DGM Translation
3. **Back-End**: DGM → NASM → Executable

### Figure 1. Strict Compilation Pipeline

```
.strict → AST → LLVM IR (.ll) → DGM (.dgm) → NASM (.s) → Executable
```

### 4.1 DGM Mapping

The **144-entry DGM opcode set** ensures consistency between high-level semantics and low-level assembly.

* `00–4B` Core LLVM instructions
* `50–7B` Safety & arithmetic extensions
* `80–9B` Data structures
* `A0–BB` CIAM extensions

### 4.2 Safety Guards

* Overflow checks compiled into DGM safe ops.
* Memory bounds enforced at array/list access.
* Unreachable states trigger `language.assert` (DGM `7B`).

---

## 5. Ecosystem

### 5.1 Tooling

* **strictc**: Core compiler CLI
* **SPM**: Strict Package Manager
* **IDE Plugins**: Syntax highlighting, debugging, IR/DGM inspection
* **Test Framework**: Golden output diffs, integrated CI

### 5.2 Continuous Integration

* GitHub Actions pipeline: builds on Windows/Linux/macOS, runs regression tests, publishes executables.

### 5.3 Interop

* C ABI compatible.
* FFI allows seamless linking with C/C++ shared libraries.

---

## 6. Adoption Scenarios

### 6.1 Education

* Strict’s pseudocode-like syntax makes it ideal for teaching programming.
* Students can transition from structured pseudocode to native executables seamlessly.

### 6.2 Mission-Critical Systems

* Finance, aerospace, embedded, and defense demand **safe low-level code**.
* Strict’s safety model reduces risk of overflow, null deref, or unchecked state.

### 6.3 General-Purpose Development

* Full-featured OOP, generics, concurrency, and tooling make Strict competitive with mainstream general-purpose languages.

---

## 7. Example End-to-End Program

```strict
Class Account
    Private Let balance = 0
    Func Deposit(amount)
        balance = Safe.Add balance, amount
    End
    Func GetBalance()
        Return balance
    End
End

Func Main()
    Let acc = New Account()
    Call acc.Deposit(200)
    Print "Balance:"
    Print Call acc.GetBalance()
End
```

---

## 8. Tagline and Mission

> **Strict — The disciplined language for clear logic and fearless execution.**

Strict’s mission is to prove that **discipline, safety, and performance are not opposites** but natural allies. By blending structured readability with uncompromising low-level compilation, Strict empowers developers to write software that is **clear, correct, and fast enough for any domain**.

---

## References

1. LLVM Project. *LLVM Language Reference Manual.*
2. NASM Team. *Netwide Assembler Documentation.*
3. [Strict Programming Language Repository](https://github.com/JoeySoprano420/Strict)

---


Here’s a crisp, no-nonsense overview that answers the “who/what/why/when/where/how” of **Strict** as a mature, real-world language.

---

# Who will use Strict?

* **Systems & infra engineers** who want native performance and deterministic builds.
* **Safety-critical developers** (finance, aerospace, automotive, med-tech) who need explicit control, audits, and guardrails.
* **Backend & platform teams** building high-reliability services and CLIs with minimal runtime overhead.
* **Educators & students** who want pseudocode-clear syntax that still compiles to fast native code.
* **Security & compliance teams** that favor explicit flow, assertions, and verifiable outputs.
* **Data & quant engineers** needing low-latency pipelines with guarded arithmetic.

---

# What will it be used for?

* **High-reliability services**: control planes, schedulers, queues, config/secret managers, audit pipelines.
* **Low-latency components**: gateways, proxies, telemetry shippers, data-plane filters.
* **Batch/ETL & analytics kernels** with tight loops and predictable CPU behavior.
* **CLI tools** that start fast, ship as a single native binary, and have few external deps.
* **Embedded/edge utilities** where footprint, determinism, and safety checks matter.
* **Financial/risk engines** that benefit from `Safe.*` arithmetic and explicit flow.
* **Education**: teaching algorithmic thinking with `If/Then/End`, `Match`, `For`, `Func`.

---

# Industries & sectors likely to gravitate to it

* **Finance/FinTech**, **Aerospace/Automotive**, **Industrial IoT**, **Telecom/Networking**, **Defense**, **Healthcare IT**, **Gov/Regulated** environments, **Cloud & DevOps platforms**, and **Academia**.

---

# Real-world things you can build

* **APIs & microservices** with strict request validation and low overhead.
* **Stream processors** (log enrichment, metric aggregation, alerting).
* **CNC/PLC control helpers** and **edge device supervisors**.
* **Trading venue adapters** and **risk calculators** using `Safe.Div`, `Safe.Add`.
* **Command-line suites** (packagers, linters, generators) with instant startup.
* **High-performance libraries** exposed via C ABI to other ecosystems.

---

# Learning curve (expected)

* **On-ramp**: very fast. The `Let/If/Then/End/Match/Func` style reads like structured pseudocode.
* **Intermediate**: understanding the **safety ops** and **pattern matching** idioms.
* **Advanced**: OOP/Generics/Templates/Concurrency and DGM/LLVM mapping. Still simpler than mastering C++ templates or Rust’s borrow checker, but more “systems-aware” than Python/JS.

---

# Interoperability

* **What languages interoperate?** Any language that can call a **C ABI** function. That includes **C, C++, Rust, Zig, Go (cgo), Python (ctypes/cffi), Java/Kotlin (JNI), C# (P/Invoke)**, and more.
* **How does interop work?**

  * Expose Strict functions as **`extern "C"`-style signatures** during codegen.
  * Link with **system linkers**; ship as shared libs (`.so/.dylib/.dll`) or static libs.
  * Call out to C/C++ libs with straightforward FFI (no hidden runtime).
* **How interoperable?** High. Strict compiles to **plain native code** with a minimal runtime and a stable C boundary.

---

# Purposes & use cases (including edge cases)

* **Primary**: deterministic services, native CLIs, low-latency components, safety-checked compute kernels.
* **Edge**:

  * **Air-gapped builds** (auditable IR/DGM/ASM artifacts).
  * **Regulated pipelines** (prove safety checks exist; archive `.ll` and `.dgm`).
  * **Constrained environments** (fast cold-start, tiny RSS, no heavy VM).
  * **“Last-mile” acceleration**: rewrite hot paths called from higher-level stacks.

---

# What Strict can do **right now**

* **Compile to native** via **LLVM IR → NASM x64**, producing a single executable.
* **Core language**: `Let`, expressions, `If/Else`, `For/While`, `Func/Return`, `Match`, `Print`, `Input`.
* **Safety ops**: `Safe.Add/Sub/Mul/Div/Shift` (guarded arithmetic).
* **Data structures**: Lists/Arrays with bounds helpers in the runtime.
* **OOP**: Classes, fields, methods, inheritance, encapsulation, basic polymorphism.
* **Generics & templates**: Parametric types and compile-time expansion.
* **Concurrency**: `Future`, `Parallel`, `Sync` (deterministic joins).
* **Artifacts**: Emit **`.ll`**, **`.dgm`**, **`.s`** alongside the final binary for auditability.

---

# When is Strict preferred over others?

* When you want **readable, explicit control flow** *and* **native performance**.
* When **overflow/guarded math** and **exhaustive decision logic** must be first-class.
* When you need **instant startup** and **small, dependency-light** deliverables.
* When **auditability** matters (keep IR/ASM trail for compliance).
* When integrating at the **C ABI boundary** with existing systems.

---

# Where Strict shines

* **Low-latency services** (proxies, API edges, order routers).
* **Compute kernels** with tight loops, where safety and speed both matter.
* **Security-sensitive utilities** with minimal attack surface and explicit checks.
* **Educational settings**: teach algorithms and systems thinking with real compilation.

---

# Where it can out-perform others

* **Startup time & footprint** vs. VM/bytecode languages (JVM/.NET/Node).
* **CPU-bound hot paths** vs. dynamic languages (Python/Ruby/JS).
* **Auditability & determinism** vs. languages with large opaque runtimes.
* **Safety by default** vs. C/C++ when arithmetic errors and unchecked logic are risks.

---

# Where it shows the most potential

* Becoming the **“pseudocode you can ship”** language for regulated, high-reliability software.
* A **teaching-to-production bridge**: students learn structured logic that compiles into real, fast binaries.

---

# What’s next (evolution)

* **Richer pattern matching** (guards, destructuring for user types).
* **Ownership & region analysis** for stronger memory safety without GC.
* **First-class async/await** on top of `Future/Parallel`.
* **WASM target** and **arm64** backends alongside x64.
* **Package manager (SPM)** with reproducible lockfiles and supply-chain attestations.
* **Formal spec** + **verifier** for the DGM mapping (machine-checkable semantics).

---

# Where is Strict most needed?

* **Finance & risk**, **aerospace/automotive control**, **industrial automation**, **healthcare IT**, **telecom core**, **government**—anywhere that requires **explainable, auditable, fast** software.

---

# Performance notes

* **How fast to load / start up?** Native-binary fast (no VM warm-up). Typical CLI/services start in **milliseconds**.
* **Throughput/latency?** Competitive with hand-tuned C/C++ for CPU-bound sections; predictable GC-free latency profile.

---

# Security & safety

* **Why secure?** Minimal runtime surface, explicit control flow, no dynamic JIT, no reflection by default.
* **How safe?** Guarded arithmetic (`Safe.*`), bounds helpers, exhaustive `Match`, `Assert`, `Defer`, compile-time diagnostics, and auditable **IR/DGM/ASM** outputs.

---

# Comparisons (thumbnail)

* **vs C**: far clearer control flow + safe ops + match; similar performance.
* **vs C++**: simpler mental model; fewer foot-guns; no gigantic runtime; easier audit trail.
* **vs Rust**: friendlier on-ramp and simpler syntax; less rigorous ownership at present (future work may add region/ownership analysis).
* **vs Go**: lower overhead and faster startup; more explicit control; fewer batteries included (by design).
* **vs Python/JS**: vastly faster and lower latency; compiled; but fewer dynamic conveniences (deliberate trade-off).

---

# Why choose Strict?

* **Clarity by design** (human-readable, audit-friendly).
* **Safety as a feature** (not a library).
* **Performance without VM baggage**.
* **Interop that just works** (C ABI, link everything).
* **Deterministic artifacts** you can ship, sign, and explain.

---

# Why was Strict created?

To **collapse the gap between pseudocode clarity and production performance**. Teams were either writing unreadable low-level code or readable high-level code with heavy runtime costs and fuzzy guarantees. Strict’s thesis: you can have **clear logic + fearless execution** in one language—no compromise.

---


