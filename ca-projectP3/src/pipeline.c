#include "processor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static void update_flags(Processor *p, uint8_t result, uint8_t val1, uint8_t val2, uint8_t op) {
  p->SREG = 0;
  if (result == 0) {
    p->SREG |= FLAG_Z;
  }
  if (result & 0x80) {
    p->SREG |= FLAG_N;
  }

  if (op == 0b0000 || op == 0b0001) {// add or subtract
    uint16_t temp;
    if (op == 0b0000){
        temp = (uint16_t)val1 + val2;
    } else{
        temp = (uint16_t)((int8_t)val1 - (int8_t)val2);
    }
      if (temp & 0x100){ 
        p->SREG |= FLAG_C;
      }
      uint8_t ovf = ((val1 ^ result) & (val2 ^ result)) >> 7;
      if (ovf) {
        p->SREG |= FLAG_V;
      }
      if (((p->SREG & FLAG_N) >> 1) ^ (p->SREG & FLAG_V)){
       p->SREG |= FLAG_S;
       }
  }
}
// carry add 
// ovf add & sub
// negative  ADD, SUB, MUL, ANDI, EOR, SAL, and SAR
//sign ADD and SUB instruction.
//zero ADD, SUB, MUL, ANDI, EOR, SAL, and SAR 

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
 
    if (E.opcode == 3 || E.opcode == 4 || E.opcode == 5 || E.opcode == 10 || E.opcode == 11 ||E.opcode == 8 ||E.opcode == 9) {
        E.imm = (int16_t)((int8_t)(instruction & 0x3F));
        E.rt=0;
    } else {
        E.imm=0;
        E.rt = (int16_t)((int8_t)(instruction & 0x3F));
    }
    E.valueRS = p->Register[E.rs];
    E.valueRT = p->Register[E.rt];
    E.valid   = true;
    p->ID_EX = E;
    p->IF_ID.valid = false;
}

void execute(Processor *p) {
  if (!p->ID_EX.valid) {
    return;
  }

  uint8_t opcode = p->ID_EX.opcode;
  uint8_t rs = p->ID_EX.rs;
  uint8_t rt = p->ID_EX.rt;
  int8_t immediate = p->ID_EX.imm;
  uint8_t val1 = p->ID_EX.valueRS;
  uint8_t val2 = (opcode <= 0b0010 || opcode == 0b0110 || opcode == 0b0111) ? p->ID_EX.valueRT : immediate;// law rtype rt else imm
  uint8_t result = 0;

  bool flag = false;

  switch (opcode) {
      case 0b0000: result = val1 + val2; break;               // ADD R1 R2
      case 0b0001: result = val1 - val2; break;               // SUB R1 R2
      case 0b0010: result = val1 * val2; break;               // MUL R1 R2
      case 0b0011: result = immediate; break;                 // MOVI R1 IMM
      case 0b0101: result = val1 & val2; break;               // ANDI R1 IMM
      case 0b0110: result = val1 ^ val2; break;               // EOR R1  R2
      case 0b1000: result = val1 << val2; break;              // SAL R1 R2
      case 0b1001: result = ((int8_t)val1) >> val2; break;    // SAR R1 R2
      case 0b1010: result = mem_read_data(p, immediate); break; // LDR R1 IMM

      case 0b1011:  // STR R1 IMM
          mem_write_data(p, immediate, p->Register[rs]);
          flag = true;
          break;

      case 0b0100:  // BEQZ R1 IMM
          if (p->Register[rs] == 0) {
              p->PC = p->ID_EX.pc + 1 + immediate;
              p->IF_ID.valid = false;
              p->ID_EX.valid = false;
              return;
          }
          flag = true;
          break;

      case 0b0111:  // BR R1 R2
          p->PC = ((uint16_t)p->Register[rs] << 8) | p->Register[rt];
          p->IF_ID.valid = false;
          p->ID_EX.valid = false;
          return;

      default:
          flag = true;
          break;
  }

  if (!flag) {
      if (rs != 0) {
          p->Register[rs] = result;
      }
      update_flags(p, result, val1, val2, opcode);
  }// 34an mayekteb4 f R0

  p->ID_EX.valid = false;
}


void process_cycle(Processor *p) {
   
    p->EX_instr = p->ID_EX.instr;
    p->EX_pc    = p->ID_EX.pc;
    p->EX_valid = p->ID_EX.valid;
    execute(p);
    decode(p);
    if (p->PC < 1024) {
        fetch(p);
    } 
}

void print_registers(const Processor *p) {
    printf("Registers:\n");
    int m = 0;
    for (int i = 0; i < 64; i++) {
        printf("R%02d: 0x%02X  ", i, p->Register[i]);
        if (p->Register[i] && i != 0) {
            m = 1;
        }
        if ((i & 7) == 7) {
            printf("\n");
        }
    }
    if (!m) {
        printf("(all zero except R0)\n");
    }
    printf("SREG: [%c%c%c%c%c]\n",
           (p->SREG & FLAG_C) ? 'C' : '-',
           (p->SREG & FLAG_V) ? 'V' : '-',
           (p->SREG & FLAG_N) ? 'N' : '-',
           (p->SREG & FLAG_S) ? 'S' : '-',
           (p->SREG & FLAG_Z) ? 'Z' : '-');
}

void print_pipeline(const Processor *p, int cycle) {
    static char if_buffer[64], id_buffer[128], ex_buffer[128];
    printf("Clock Cycle %d\n", cycle);
   
    if (p->IF_ID.valid)
        sprintf(if_buffer, "Instruction %d (PC=%d)", p->IF_ID.pc + 1, p->IF_ID.pc);
    else
        sprintf(if_buffer, "-");
 
    if (p->ID_EX.valid){
        if ( p->ID_EX.opcode == 3 ||  p->ID_EX.opcode == 4 ||  p->ID_EX.opcode == 5 ||  p->ID_EX.opcode == 10 ||  p->ID_EX.opcode == 11 || p->ID_EX.opcode == 8 || p->ID_EX.opcode == 9) {
          sprintf(id_buffer, "Instruction %d (opcode=%d, rs=R%d=%d, , imm=%d)", 
            p->ID_EX.pc + 1, p->ID_EX.opcode, p->ID_EX.rs, p->ID_EX.valueRS, p->ID_EX.imm);
    } else {
         sprintf(id_buffer, "Instruction %d (opcode=%d, rs=R%d=%d, rt=R%d=%d)", 
            p->ID_EX.pc + 1, p->ID_EX.opcode, p->ID_EX.rs, p->ID_EX.valueRS, p->ID_EX.rt, p->ID_EX.valueRT);
        
            }
        }
    else
        sprintf(id_buffer, "-");
  
    if (p->EX_valid)
        sprintf(ex_buffer, "Instruction %d (PC=%d)", p->EX_pc + 1, p->EX_pc);
    else
        sprintf(ex_buffer, "-");
    printf("| %-30s | %-60s | %-30s |\n", if_buffer, id_buffer, ex_buffer);
}

