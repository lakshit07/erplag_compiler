ERPLAG COMPILER
--------

This is a compiler for a toy language **ERPLAG** for the partial fulfilment of the course - *CS F363 Compiler Construction*.

#### Usage

To generate the executable **compiler** in the local folder.

```bash
make all
```
To demonstrate various stages of the compiler in a menu based format. The ERPLAG code file is *erplag_code.txt* and the assembly code will be generated in *assembly_code.asm*.

```bash
./compiler erplag_code.txt assembly_code.asm
```

To run the resulting assembly code generated in *assembly_code.asm*

```bash
nasm -f elf64 assembly_code.asm
gcc -o run code.o
./run
```

#### Contributors
* Anirudh Kumar Bansal
* Lakshit Bhutani

