# Double Big Harvard Processor Simulator

A complete C-based simulator for a fictional "Double Big Harvard" processor architecture. This simulator models a Harvard memory system with separate instruction and data memories, implements a 3-stage pipeline, and supports 12 core instructions with proper flag management.

## Table of Contents

- [Motivation](#motivation)
- [Build Status](#build-status)
- [Code Style](#code-style)
- [Tech/Framework Used](#techframework-used)
- [Features](#features)
- [Code Examples](#code-examples)
- [Installation](#installation)
- [Usage](#usage)
- [Architecture](#architecture)
- [Instruction Set](#instruction-set)
- [Pipeline](#pipeline)
- [Status Flags](#status-flags)
- [Project Structure](#project-structure)
- [Contribute](#contribute)
- [Credits](#credits)

## Motivation

This project was developed as an educational tool to understand and simulate processor architecture concepts. The simulator demonstrates:

- **Harvard Architecture**: Separate instruction and data memory spaces, which is fundamental in many embedded systems and microcontrollers
- **Pipeline Processing**: Implementation of a 3-stage pipeline (Fetch, Decode, Execute) to understand instruction-level parallelism and pipeline hazards
- **Flag Management**: Proper handling of processor status flags (Carry, Overflow, Negative, Sign, Zero) that are essential for conditional execution and arithmetic operations
- **Instruction Set Architecture (ISA)**: Design and implementation of a complete instruction set including arithmetic, logical, shift, data movement, and control flow instructions

This simulator serves as a practical learning tool for computer architecture courses, allowing students to see how instructions are fetched, decoded, and executed in a pipelined processor, and how status flags are updated based on instruction results.

## Build Status

The project is functional and compiles successfully with GCC using C11 standard. However, there are some known issues and limitations:

- **Hardcoded File Path**: The program file path in `main.c` (line 8) is currently hardcoded with an absolute path. This needs to be updated to use a relative path or command-line argument for better portability.
- **Makefile Incomplete**: The Makefile does not include `memory.c` in the compilation, which may cause linking errors. Manual compilation requires explicitly including all source files.
- **Limited Error Handling**: Some error cases (e.g., invalid register numbers, out-of-bounds memory access) are not fully validated.
- **No Pipeline Hazard Detection**: The simulator does not detect or handle data hazards, control hazards, or structural hazards that would occur in a real pipeline.
- **Register R0 Protection**: While R0 is protected from writes, there's no explicit validation or warning when attempting to write to it.
- **Memory Bounds**: Memory access functions perform basic bounds checking but could provide more detailed error messages.

These issues do not prevent the simulator from functioning correctly for its intended educational purposes, but improvements could enhance robustness and usability.

## Code Style

The project follows consistent C programming conventions:

### Naming Conventions
- **Functions**: Use lowercase with underscores (snake_case)
  - Examples: `proc_init()`, `mem_load_program()`, `print_registers()`
- **Variables**: Use lowercase with underscores (snake_case)
  - Examples: `opcode`, `valueRS`, `instr_mem`
- **Constants/Macros**: Use uppercase with underscores
  - Examples: `FLAG_C`, `FLAG_V`, `PROCESSOR_H`
- **Types**: Use PascalCase for typedef structures
  - Examples: `Processor`, `IF_ID_Reg`, `ID_EX_Reg`

### Code Organization
- **Header Guards**: All header files use `#ifndef` guards to prevent multiple inclusions
- **Function Prefixes**: Functions are prefixed by module:
  - `proc_*` for processor initialization
  - `mem_*` for memory operations
  - `print_*` for display/output functions
- **Static Functions**: Internal helper functions (like `update_flags()`) are marked as `static`
- **Indentation**: Uses 4 spaces for indentation
- **Comments**: Inline comments explain complex logic, especially for flag calculations and instruction encoding

### Standards Compliance
- **C Standard**: C11 (`-std=c11`)
- **Compiler Warnings**: Project uses `-Wall -Wextra` flags for comprehensive warning detection
- **Type Safety**: Uses standard integer types (`uint8_t`, `uint16_t`, `int8_t`, etc.) from `<stdint.h>`

## Tech/Framework Used

This project is implemented using:

- **Programming Language**: C (C11 standard)
- **Compiler**: GCC (GNU Compiler Collection)
- **Build System**: Make (GNU Make)
- **Standard Libraries**:
  - `<stdio.h>`: File I/O and formatted output
  - `<stdlib.h>`: Memory allocation and exit functions
  - `<string.h>`: String manipulation functions (`memset`, `strcmp`)
  - `<stdint.h>`: Standard integer types
  - `<stdbool.h>`: Boolean type support

### Development Tools
- **Text Editor/IDE**: Any C-compatible editor (VS Code, Vim, etc.)
- **Debugger**: GDB (GNU Debugger) compatible
- **Platform**: Cross-platform (Windows, Linux, macOS) - tested on Windows

No external frameworks or libraries are required beyond the standard C library, making the project lightweight and easily portable.

## Features

The simulator implements the following features:

### Core Processor Features
- **Harvard Memory Architecture**: Separate 1024×16-bit instruction memory and 2048×8-bit data memory
- **64 General-Purpose Registers**: 8-bit registers (R0-R63) with R0 hardcoded to zero
- **Status Register (SREG)**: 5 flags (Carry, Overflow, Negative, Sign, Zero)
- **Program Counter**: 16-bit PC tracking instruction execution

### Instruction Set (12 Instructions)
- **Arithmetic**: ADD, SUB, MUL
- **Data Movement**: MOVI (move immediate), LDR (load from memory), STR (store to memory)
- **Logical**: ANDI (AND immediate), EOR (exclusive OR)
- **Shift**: SAL (shift arithmetic left), SAR (shift arithmetic right)
- **Control Flow**: BEQZ (branch if equal to zero), BR (branch register)

### Pipeline Features
- **3-Stage Pipeline**: Fetch, Decode, Execute stages
- **Pipeline Registers**: IF/ID and ID/EX pipeline registers
- **Branch Handling**: Pipeline flush on branch instructions
- **Cycle Counting**: Tracks execution cycles with detailed pipeline state output

### Simulation Features
- **Program Loading**: Loads assembly programs from text files
- **Cycle-by-Cycle Output**: Displays pipeline state at each clock cycle
- **Register Display**: Shows all register values and status flags
- **Memory Display**: Prints instruction and data memory contents
- **Instruction Formatting**: Supports comments (; or #) and empty lines in program files

### Educational Features
- **Detailed Output**: Comprehensive logging of instruction execution
- **Flag Visualization**: Status flags displayed as [C-V-N-S-Z] format
- **Pipeline Visualization**: Shows which instruction is in each pipeline stage
- **Memory Inspection**: View both instruction and data memory states

## Code Examples

### Example 1: Processor Initialization
```c
void proc_init(Processor *p) {
    memset(p, 0, sizeof(Processor));
    p->PC = 0; 
    p->IF_ID.valid = false;
    p->ID_EX.valid = false;
}
```
This function initializes the processor structure, zeroing all memory and setting initial values for the program counter and pipeline registers.

### Example 2: Flag Update Logic
```c
static void update_flags(Processor *p, uint8_t result, uint8_t val1, uint8_t val2, uint8_t op) {
  p->SREG = 0;
  if (result == 0) {
    p->SREG |= FLAG_Z;
  }
  if (result & 0x80) {
    p->SREG |= FLAG_N;
  }

  if (op == 0b0000 || op == 0b0001) { // add or subtract
    uint16_t temp;
    if (op == 0b0000) {
        temp = (uint16_t)val1 + val2;
    } else {
        temp = (uint16_t)((int8_t)val1 - (int8_t)val2);
    }
    if (temp & 0x100) { 
      p->SREG |= FLAG_C;
    }
    uint8_t ovf = ((val1 ^ result) & (val2 ^ result)) >> 7;
    if (ovf) {
      p->SREG |= FLAG_V;
    }
    if (((p->SREG & FLAG_N) >> 1) ^ (p->SREG & FLAG_V)) {
      p->SREG |= FLAG_S;
    }
  }
}
```
This function updates processor flags based on instruction results, handling carry, overflow, negative, sign, and zero flags for arithmetic operations.

### Example 3: Instruction Fetch Stage
```c
void fetch(Processor *p) {
  if (p->PC < 1024) {
      uint16_t instruction = p->instr_mem[p->PC];
      if (instruction == 0) {
          p->PC = 1024;
          return;
      }
      p->IF_ID.instr = instruction;
      p->IF_ID.pc    = p->PC;
      p->IF_ID.valid = true;
      p->PC++;
  }
}
```
The fetch stage reads instructions from instruction memory and loads them into the IF/ID pipeline register.

### Example 4: Instruction Decode Stage
```c
void decode(Processor *p) {
    if (!p->IF_ID.valid) {
        return;
    }
    uint16_t instruction = p->IF_ID.instr;
    ID_EX_Reg E = {0};
    E.instr   = instruction;
    E.pc      = p->IF_ID.pc;
    E.opcode  = (instruction >> 12) & 0x0F;
    E.rs      = (instruction >> 6) & 0x3F;
 
    if (E.opcode == 3 || E.opcode == 4 || E.opcode == 5 || E.opcode == 10 || 
        E.opcode == 11 || E.opcode == 8 || E.opcode == 9) {
        E.imm = (int16_t)((int8_t)(instruction & 0x3F));
        E.rt = 0;
    } else {
        E.imm = 0;
        E.rt = (int16_t)((int8_t)(instruction & 0x3F));
    }
    E.valueRS = p->Register[E.rs];
    E.valueRT = p->Register[E.rt];
    E.valid   = true;
    p->ID_EX = E;
    p->IF_ID.valid = false;
}
```
The decode stage extracts opcode, register numbers, and immediate values from the instruction, then reads register values and stores them in the ID/EX pipeline register.

### Example 5: Program Loading from File
```c
void mem_load_program(Processor *p, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    char line[128];
    uint16_t addr = 0; 

    while (fgets(line, sizeof(line), file) && addr < 0x0400) {
        if (line[0] == '\n' || line[0] == ';' || line[0] == '#') continue;

        char op[16];
        int r1 = 0, r2 = 0, value = 0;
        uint16_t opcode = 0, rs = 0, rt = 0, imm = 0, instruction = 0;
        
        if (sscanf(line, "%s R%d R%d", op, &r1, &r2) == 3) {
            rs = (uint8_t)r1;
            rt = (uint8_t)r2;
            
            if      (strcmp(op, "ADD") == 0) opcode = 0;
            else if (strcmp(op, "SUB") == 0) opcode = 1;
            else if (strcmp(op, "MUL") == 0) opcode = 2;
            else if (strcmp(op, "EOR") == 0) opcode = 6;
            else if (strcmp(op, "BR")  == 0) opcode = 7;
            
            instruction = (opcode << 12) | ((rs & 0x3F) << 6) | (rt & 0x3F);
            p->instr_mem[addr++] = instruction;
        }
        // ... additional parsing for immediate instructions
    }
    fclose(file);
}
```
This function parses assembly instructions from a text file and encodes them into 16-bit binary instructions stored in instruction memory.

### Example 6: Execute Stage - Instruction Execution
```c
void execute(Processor *p) {
  if (!p->ID_EX.valid) {
    return;
  }

  uint8_t opcode = p->ID_EX.opcode;
  uint8_t rs = p->ID_EX.rs;
  uint8_t rt = p->ID_EX.rt;
  int8_t immediate = p->ID_EX.imm;
  uint8_t val1 = p->ID_EX.valueRS;
  uint8_t val2 = (opcode <= 0b0010 || opcode == 0b0110 || opcode == 0b0111) 
                  ? p->ID_EX.valueRT : immediate;
  uint8_t result = 0;

  switch (opcode) {
      case 0b0000: result = val1 + val2; break;               // ADD
      case 0b0001: result = val1 - val2; break;               // SUB
      case 0b0010: result = val1 * val2; break;               // MUL
      case 0b0011: result = immediate; break;                 // MOVI
      case 0b0101: result = val1 & val2; break;              // ANDI
      case 0b0110: result = val1 ^ val2; break;              // EOR
      case 0b1000: result = val1 << val2; break;             // SAL
      case 0b1001: result = ((int8_t)val1) >> val2; break;   // SAR
      case 0b1010: result = mem_read_data(p, immediate); break; // LDR
      case 0b1011: mem_write_data(p, immediate, p->Register[rs]); break; // STR
      // ... branch instructions
  }

  if (rs != 0) {
      p->Register[rs] = result;
  }
  update_flags(p, result, val1, val2, opcode);
  p->ID_EX.valid = false;
}
```
The execute stage performs the actual operation based on the opcode, updates registers (except R0), and updates status flags.

## Installation

### Prerequisites

To build and run this simulator, you need:

1. **GCC Compiler**: GNU Compiler Collection (version 4.8 or later recommended)
   - Windows: Install via MinGW-w64 or use WSL
   - Linux: Install via package manager: `sudo apt-get install gcc` (Ubuntu/Debian) or `sudo yum install gcc` (RHEL/CentOS)
   - macOS: Install via Xcode Command Line Tools: `xcode-select --install`

2. **Make** (optional, for using Makefile):
   - Windows: Included with MinGW or use `nmake` (Visual Studio)
   - Linux: Usually pre-installed, or `sudo apt-get install make`
   - macOS: Included with Xcode Command Line Tools

3. **Text Editor**: Any text editor for creating/editing program files

### Installation Steps

1. **Clone or download the repository**:
   ```bash
   git clone <repository-url>
   cd Double-Big-Harvard-Processor-Simulator
   ```

2. **Navigate to the project directory**:
   ```bash
   cd ca-projectP3
   ```

3. **Build the project**:
   
   **Option A: Using Makefile** (recommended)
   ```bash
   make
   ```
   Note: You may need to manually compile with all source files if the Makefile is incomplete:
   ```bash
   gcc -Wall -Wextra -std=c11 -o sim src/main.c src/processor.c src/pipeline.c src/memory.c
   ```
   
   **Option B: Manual compilation**
   ```bash
   cd src
   gcc -Wall -Wextra -std=c11 -o sim main.c processor.c pipeline.c memory.c
   ```

4. **Verify installation**:
   The executable `sim` (or `sim.exe` on Windows) should be created in the `src` directory.

### Clean Build Artifacts

To remove compiled files:
```bash
cd ca-projectP3
make clean
# or manually:
rm -f sim *.o src/sim src/sim.exe
```

## Usage

1. **Prepare your program**: Create a text file with assembly instructions (see format below)

2. **Update the program path**: Edit `main.c` line 8 to point to your program file:
   ```c
   mem_load_program(&cpu, "path/to/your/program.txt");
   ```

3. **Run the simulator**:
   ```bash
   ./sim
   # or on Windows:
   sim.exe
   ```

### Program File Format

Programs are written as plain text with one instruction per line:

```
MOVI R1 5
ADD R2 R1
SUB R3 R2
STR R3 100
LDR R4 100
```

**Rules:**
- One instruction per line
- Comments start with `;` or `#`
- Empty lines are ignored
- Register numbers: R0 through R63
- Immediate values: signed 6-bit integers (-32 to 31)

## Architecture

### Memory System
- **Instruction Memory**: 1024 × 16-bit words (addresses 0x0000 - 0x03FF)
- **Data Memory**: 2048 × 8-bit bytes (addresses 0x0000 - 0x07FF)
- **Harvard Architecture**: Separate instruction and data memory spaces

### Register File
- **64 General-Purpose Registers**: R0 through R63 (8-bit each)
  - R0 is hardcoded to always contain 0 (cannot be written)
- **Status Register (SREG)**: 8-bit register containing processor flags
- **Program Counter (PC)**: 16-bit register tracking current instruction address

### Instruction Format
All instructions are 16 bits with the following format:
```
[15:12] - Opcode (4 bits)
[11:6]  - Source register (rs) - 6 bits
[5:0]   - Destination register (rt) or Immediate value - 6 bits
```

## Instruction Set

The simulator supports 12 instructions:

### Arithmetic Instructions

| Instruction | Opcode | Format | Description |
|------------|--------|--------|-------------|
| **ADD** | 0x0 | `ADD Rs Rt` | Add: `Rs = Rs + Rt` |
| **SUB** | 0x1 | `SUB Rs Rt` | Subtract: `Rs = Rs - Rt` |
| **MUL** | 0x2 | `MUL Rs Rt` | Multiply: `Rs = Rs * Rt` |

### Data Movement Instructions

| Instruction | Opcode | Format | Description |
|------------|--------|--------|-------------|
| **MOVI** | 0x3 | `MOVI Rs Imm` | Move immediate: `Rs = Imm` |
| **LDR** | 0xA | `LDR Rs Imm` | Load from memory: `Rs = DataMem[Imm]` |
| **STR** | 0xB | `STR Rs Imm` | Store to memory: `DataMem[Imm] = Rs` |

### Logical Instructions

| Instruction | Opcode | Format | Description |
|------------|--------|--------|-------------|
| **ANDI** | 0x5 | `ANDI Rs Imm` | Bitwise AND immediate: `Rs = Rs & Imm` |
| **EOR** | 0x6 | `EOR Rs Rt` | Exclusive OR: `Rs = Rs ^ Rt` |

### Shift Instructions

| Instruction | Opcode | Format | Description |
|------------|--------|--------|-------------|
| **SAL** | 0x8 | `SAL Rs Imm` | Shift arithmetic left: `Rs = Rs << Imm` |
| **SAR** | 0x9 | `SAR Rs Imm` | Shift arithmetic right: `Rs = Rs >> Imm` (sign-extended) |

### Control Flow Instructions

| Instruction | Opcode | Format | Description |
|------------|--------|--------|-------------|
| **BEQZ** | 0x4 | `BEQZ Rs Imm` | Branch if equal to zero: `if (Rs == 0) PC = PC + 1 + Imm` |
| **BR** | 0x7 | `BR Rs Rt` | Branch register: `PC = (Rs << 8) \| Rt` |

## Pipeline

The processor implements a **3-stage pipeline**:

1. **Fetch (IF)**: Reads instruction from instruction memory at PC
2. **Decode (ID)**: Decodes instruction, reads register values, extracts immediate
3. **Execute (EX)**: Performs operation, updates registers/flags, handles memory access

### Pipeline Registers
- **IF/ID Register**: Stores fetched instruction and PC
- **ID/EX Register**: Stores decoded instruction, opcode, register values, and immediate

### Pipeline Behavior
- Instructions flow through stages sequentially
- Branch instructions (BEQZ, BR) cause pipeline flush (invalidates IF/ID and ID/EX)
- The pipeline executes in reverse order (EX → ID → IF) to maintain correct dependencies

## Status Flags

The Status Register (SREG) contains 5 flags:

| Flag | Bit | Name | Description |
|------|-----|------|-------------|
| **C** | 3 | Carry | Set when arithmetic operation produces carry/borrow |
| **V** | 2 | Overflow | Set when signed arithmetic overflow occurs |
| **N** | 1 | Negative | Set when result MSB is 1 (result is negative) |
| **S** | 0 | Sign | Set when N XOR V (signed result interpretation) |
| **Z** | 4 | Zero | Set when result equals zero |

### Flag Updates
- **C**: Updated by ADD and SUB instructions
- **V**: Updated by ADD and SUB instructions
- **N**: Updated by ADD, SUB, MUL, ANDI, EOR, SAL, SAR
- **S**: Updated by ADD and SUB instructions
- **Z**: Updated by ADD, SUB, MUL, ANDI, EOR, SAL, SAR

## Project Structure

```
Double-Big-Harvard-Processor-Simulator/
├── README.md                 # This file
├── ca-projectP3/
│   ├── Makefile             # Build configuration
│   ├── README.md            # Project-specific README (empty)
│   ├── docs/
│   │   └── design.md        # Design documentation (empty)
│   └── src/
│       ├── main.c           # Entry point and simulation loop
│       ├── processor.h      # Header file with data structures
│       ├── processor.c      # Processor initialization
│       ├── pipeline.c       # Pipeline stages and execution
│       ├── memory.c         # Memory management and program loading
│       ├── utils.c          # Utility functions (currently empty)
│       ├── program.txt      # Sample program
│       └── sim.exe          # Compiled executable (generated)
```

## Contribute

Contributions to improve this simulator are welcome! Here are areas where contributions would be valuable:

### Known Areas for Improvement

1. **Command-Line Arguments**: Replace hardcoded file path with command-line argument parsing
2. **Pipeline Hazard Detection**: Implement detection and handling of data hazards, control hazards, and structural hazards
3. **Enhanced Error Handling**: Add comprehensive validation for register numbers, memory addresses, and instruction formats
4. **Better Makefile**: Fix the Makefile to properly include all source files
5. **Additional Instructions**: Extend the instruction set with more operations (e.g., OR, NOT, conditional branches)
6. **Pipeline Visualization**: Improve the pipeline state display with better formatting
7. **Debugging Features**: Add breakpoint support, step-by-step execution, and register/memory watchpoints
8. **Documentation**: Expand inline code documentation and add more example programs
9. **Code Refactoring**: Improve code organization, reduce code duplication, and enhance modularity
10. **Cross-Platform Compatibility**: Ensure consistent behavior across Windows, Linux, and macOS

### How to Contribute

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/improvement-name`)
3. Make your changes following the existing code style
4. Test your changes thoroughly
5. Commit your changes with clear commit messages
6. Push to the branch (`git push origin feature/improvement-name`)
7. Open a Pull Request with a detailed description of your changes

### Contribution Guidelines

- Follow the existing code style and naming conventions
- Add comments for complex logic
- Test your changes with multiple program examples
- Update documentation if you add new features
- Ensure code compiles without warnings using `-Wall -Wextra`

## Credits

This project was developed as part of the GUC computer architecture systems course. 

### Team Members
- Youssef Khaled
- Salma Ahmed
- Nourhan Ehab

