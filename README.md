
# Two Pass Assembler

Implements a two-pass assembler for a custom assembly language in ANSI C.


## **Architecture Overview**

 ### 0️⃣ Pre-Assembler
 * Expands macros in the source code
 * Replaces comment lines with empty lines
 * Outputs a `.am` file containing the preprocessed code

 ### 1️⃣ First Pass 
 * Performs syntax and semantic analysis
 * Builds the symbol table
 * Calculates addresses for symbols
 * Identifies and collects all data and code segments
 * Outputs error messages for syntax errors

 ### 2️⃣ Second Pass Phase
 * Resolves symbols and references
 * Generates machine code in a special encoding
 * Creates output files:
    * `.ob:` Object file containing the machine code
    * `.ent:` Entry symbols file
    * `.ext:` External symbols file

---


## 🏃‍♂️ **Run Examples**
run examples screenShots are available 
[here](runExamples/screenShots.pdf)

---

## 🛠️ **Project Structure**

The project is organized into several modules, each handling different aspects of the assembler.

### Core Assembler Components
These files handle the main logic of the assembler:
- **`assembler.c`** – Main entry point for the assembler program
- **`preAssembler.c/h`** – Macro expansion and preprocessing
- **`firstPass.c/h`** – Symbol table construction and initial code analysis
- **`firstAnalysis.c/h`** – Syntax and semantic analysis during the first pass
- **`secondPass.c/h`** – Symbol resolution and machine code generation

### General Data Structures
Reusable data structures used throughout the project:
- **`linkedList.c/h`** – Generic linked list implementation
- **`vector.c/h`** – Dynamic array implementation

### Data Structures
Data structures specific to assembler processing:
- **`fileStructures/`** – Contains various data structure implementations
- **`symbolTable.c/h`** – Manages the symbol table
- **`codeSeg.c/h`** – Handles the code segment
- **`dataSeg.c/h`** – Manages the data segment
- **`entryLines.c/h`** – Tracks entry lines
- **`externUsages.c/h`** – Tracks external symbol usage

### Utilities
General-purpose functions used across the assembler:
- **`setting.c/h`** – settings and definitions for the assembler program
- **`errors.c/h`** – Error reporting system
- **`machineWord.c/h`** – Machine word formatting utilities

### Analysis Tools
- **`text.c/h`** – Text processing utilities
- **`analysis.c/h`** – Common analysis functions
- **`fsm.c/h`** – Finite State Machine for syntax analysis

---

## 👨‍💻 **Usage**
 Require a C compiler (e.g., GCC) and a make utility 

 ** build and run**
 ```bash
  make
  ./assembler file1 file2 ...
 ```
you may want use `make clear` to delete .o file

---


## 💬 **Assembly languages**
### Registers
* `r0`, `r1`, ... , `r7`- 8 user registers
* `PSW` - Program Status Word register, stores the result of comparison operators.
* `PC` - Program Counter, holds the address of the next instruction to execute.

### Strings
 ```
 myStr: .SRTING "this is a string named myStr"
 ```
Defines a string `myStr` with the text `"this is a string named str"`.

### Integers
 ```
 .DEFINE myInt = 17
 ```
Defines a constant `myInt` with the value `17`.

### Arrays
 ```
 myArr: .DATA -1,  2 , myInt , +40 , -78
 ```
Defines an array `myArr` with the elements `-1`, `2`, `myInt` (which evaluates to 17), `+40`, and `-78`.

### operators

| opcode | operator | syntax          | meaning                                        |
| :----- | :--------|:----------------|:-----------------------------------------------|
| 0      | **mov**  | `mov A r1`      | r1 = A                                         | 
| 1      | **cmb**  | `cmp A r1`      | PSW = 1 if A==r1 else 0                        | 
| 2      | **add**  | `add A r1`      | r1 = r1 + A                                    | 
| 3      | **sub**  | `sub A r1`      | r1 = r1 - A                                    | 
| 4      | **lea**  | `lea HELLO r1`  | load memory address: r1 = &HELLO               | 
| 5      | **not**  | `not r2`        | r2 = !r2 (bitwise)                             | 
| 6      | **clr**  | `clr r2`        | r2 = 0                                         | 
| 7      | **inc**  | `inc A`         | r2++                                           | 
| 8      | **dec**  | `dec A`         | r2--                                           | 
| 9      | **jmp**  | `jmp LINE`      | move the program counter: pc = LINE            |  
| 10     | **bne**  | `bne LINE`      | jump to line if PSW in not equal to 0          |  
| 11     | **red**  | `red r2`        | read a character from stdin and store it in r2 | 
| 12     | **prn**  | `prn r2`        | print the value if r2 to stdout                | 
| 13     | **jsr**  | `jsr FUNC`      | jump to &FUNC                                  | 
| 14     | **rts**  | `rts`           | return from function                           | 
| 15     | **hlt**  | `hlt`           | halt program execution                         | 

**Addressing Modes:**
There are 4 addressing modes used for operands:
* 00 - **Immediate** *(imm)*: an integer or define value preceded by a `#` symbol  (e.g `#8`, `#x`)
* 01 - **Direct** *(sym)*: a memory address via a label - (e.g `label`)
* 10 - **Indexed** *(ind)*: a memory address with an offset - (e.g `arr[1]`)
* 11 - **Register** *(reg)*: a register - (e.g `r5`)


**Instruction Operand Table:**
 ```
 /*  --------------------------------------------------------------------
  |      operator      |      scr operand      |    target operand     |
  |--------------------|-----------------------|-----------------------|
  |   addressing mode  | imm | sym | ind | reg | imm | sym | ind | reg |
  |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  |    mov, add, sub   |  X  |  X  |  X  |  X  |     |  X  |  X  |  X  |
  |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  |        cmp         |  X  |  X  |  X  |  X  |  X  |  X  |  X  |  X  |
  |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  |        lea         |     |  X  |  X  |     |     |  X  |  X  |  X  |
  |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  | not, clr, inc, dec |                       |     |  X  |  X  |  X  |
  |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  |    jmp, bne, jsr   |                       |     |  X  |     |  X  |
  |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  |        red         |                       |     |  X  |  X  |  X  |
  |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  |        prn         |                       |  X  |  X  |  X  |  X  |
  |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  |     rts, hlt       |                       |                       |
  ----------------------------------------------------------------------
 ```
An instructional sentence may start with a label.

### Macros

A macro is a block of code that is defined with the `mcr` keyword and ends with the `endmcr` keyword. Example:
 ```
 mcr myMacro 
    inc r2
    mov A r1
 endmcr    
 ```
To use the macro, simply type the macro name `myMacro` on a separate line.

### Comment
Lines starting with `;` are comments.
 ```
 ; This is a comment
 ```
Comments must occupy the entire line.

### Extern symbol
An extern symbol is imported from another file:
  ```
 .extern EXT  
 ```

### Entry symbol
To use an extern symbol, the file defining the label must declare it as an entry symbol:
  ```
 .entry ENT  
 ```
**note:**  forward declaration resolution is supported, allowing symbols to be

---

## 👁️‍🗨️ **Machine Code**

### machine code file - `file.ob`
Machine code is encrypted in a 4-character base:

| regular 4 base | 0   | 1   | 2   | 3  |
|:---------------|:----|:----|:----|:---|
| **Encryption** | *   | #   | %   | !  |

Memory size is 4096 words, with each word being 14 bits.
The object file contains the translation of the assembly code into machine code, with the code segment preceding the data segment.


#### **code segment**

Each instruction is encoded into 1-5 words depending on the addressing modes of its operands. 

The structure of the first word is:

| 0 1   | 2 3         | 4 5          | 6 7 8 9 | 10 11 12 13 |
| :---- | :---------- | :----------- |:------- | :---------- |
| A,R,E | src add-mod | dest add-mod | opcode  | unused      |

**ARE:**
* **A (Absolute): `00`** for instructions that do not use labels and have fixed memory addresses.
* **E (External): `01`** for instructions that use extern labels. The loader resolves these addresses.
* **R (Relocatable): `11`** for instructions using internal labels, where the address is relative to the program's starting point.


**operand translation**:
* **Immediate:** add word with the integer in the whole 14 bits
* **symbol:** add word with the symbol address, where the first 2 bit are for ARE
* **Indexed:** add 2 word: first for the symbol address with ARE, and second for the index
* **Register:** add word in which: if the register is src operand, its number will be at bits 2-4. If the register is target operand, its number will be at bits 5-7. The remaining bits are zeroed. note that if both opernds are register, it'll add just one word.


#### **data segment**

Explain via example:
```
myData .data 1 2 3 
```
**Result:** 3 words in memory, one for each number.

```
myStr .str "hello"
```
**Result:** 5 words with the ASCII  codes of 'h' 'e' 'l' 'l' 'o' followed by one word for the null terminator (`0`)


### entry - `file.ent`
Each line in the entry file contains the name of a label defined as an entry symbol and its value as defined in the symbol table.

### extern - `file.ext`
Each line in the extern file contains the name of an external symbol and the address in the machine code (the line number) where an operand uses this label.

