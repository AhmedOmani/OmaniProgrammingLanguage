## OMANI Lang (C++ Compiler)
Omani is my very own programming language. Ever since I took my first steps in the field of computer science, I’ve been deeply curious about how compilers work. This curiosity has driven me to embark on a journey to build a small compiler for my language using C++.

Now, I’m taking the first steps toward understanding and implementing the core components of a compiler, starting with lexical analysis, syntax parsing, and eventually code generation. This project is my attempt to bring Omani Lang to life while demystifying the fascinating world of compilers.

## What Is OMANI Lang?
Omani Lang is a simple, yet expressive, programming language designed for learning purposes. 
Its syntax and structure are inspired by modern programming languages but aim to remain minimal to focus on compiler design.

## Building

Requires `nasm` and `ld` on a Linux operating system.

```bash
git clone https://github.com/orosmatthew/hydrogen-cpp
cd hydrogen-cpp
mkdir build
cmake -S . -B build
cmake --build build
```
