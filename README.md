# CVM++ (Custom Virtual Machine in C++)

CVM++ is a minimal end-to-end programming language pipeline built in C++. It demonstrates how a simple language is processed through all major stages:

**Source Code → Tokens → AST → Bytecode → Execution (VM)**

This project is ideal for learning:
- Compilers (Lexer, Parser, AST)
- Bytecode generation
- Virtual Machines
- Language design basics

---

## 🚀 Features

- Simple custom language with:
  - Variable declaration (`let`)
  - Basic arithmetic operations ('+')
  - Output statements ('print')
- Full pipeline:
  - Lexer → Parser → Compiler → Virtual Machine
- REPL mode (interactive)
- Script execution from file

---

## 📁 Project Structure

```
CVM_PlusPlus/
├── CMakeLists.txt
├── src/
│   └── main.cpp
├── include/
│   ├── Common.h
│   ├── Lexer.h
│   ├── Parser.h
│   ├── Compiler.h
│   └── VM.h
└── tests/
    └── script.cvm
```

---

## ⚙️ Build Instructions

### 1. Install CMake
Make sure you have CMake ≥ 3.10 installed.

### 2. Build the project

```bash
mkdir build
cd build
cmake ..
make
```

This will generate the executable:

```
cvm
```

---

## ▶️ How to Run

### 🔹 Run with a script file

```bash
./cvm ../tests/script.cvm
```

### 🔹 Run in REPL mode

```bash
./cvm
```

Example:

```
cvm> let x = 5;
cvm> print x + 10;
15
```

---

## 🧪 Example Script

File: `tests/script.cvm`

```c
let x = 10;
let y = 20;
print x + y;
let z = x + y + 5;
print z;
```

### Output

```
--- VM Execution Output ---
30
35
```

---

## 🧠 How It Works (Pipeline)

### 1. Lexer (`Lexer.h`)
- Converts raw text into tokens

Example:
```
let x = 10;
```

Becomes:
```
TOK_LET TOK_IDENT TOK_ASSIGN TOK_INT TOK_SEMI
```

---

### 2. Parser (`Parser.h`)
- Builds an Abstract Syntax Tree (AST)
- Handles expressions like:

```
x + y + 5
```

---

### 3. Compiler (`Compiler.h`)
- Converts AST → Bytecode
- Uses a stack-based instruction model

Example bytecode:
```
PUSH 10
STORE x
LOAD x
PUSH 20
ADD
PRINT
```

---

### 4. Virtual Machine (`VM.h`)
- Executes bytecode instructions
- Uses:
  - Stack for computation
  - Array for variables

---

## 🧾 Supported Language Syntax

### Variables
```c
let x = 10;
```

### Arithmetic
```c
x + y + 5
```

### Print
```c
print x + y;
```

---

## 🔧 Opcodes

| Opcode    | Description                     |
|----------|--------------------------------|
| OP_PUSH  | Push value to stack            |
| OP_ADD   | Add top two values             |
| OP_STORE | Store value in variable        |
| OP_LOAD  | Load variable to stack         |
| OP_PRINT | Print top value                |
| OP_HALT  | Stop execution                 |

---

## 📌 Limitations (v1)

- Only integer support
- Only `+` operator
- No error handling
- No parentheses
- Max 256 variables

---

## 🚀 Future Improvements

- Add subtraction, multiplication, division
- Add parentheses and precedence
- Better error handling
- Support for strings
- Control flow (if, loops)
- Functions

---

## 💡 Learning Goal

This project is designed to help you understand:
- How programming languages work internally
- How compilers and interpreters are built
- How bytecode VMs execute instructions

---

## 🤝 Contribution

Feel free to fork and extend:
- Add new syntax
- Improve VM performance
- Expand compiler capabilities

---

## 📜 License

This project is open-source and free to use for educational purposes.
