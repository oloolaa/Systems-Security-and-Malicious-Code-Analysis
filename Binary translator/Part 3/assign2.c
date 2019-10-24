
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <assert.h>

#define ASSERT              assert
#define NOT_REACHED()       assert(0)
#define NOT_IMPLEMENTED()   assert(0)
#define DPRINT              printf

#define BIT(_v, _b)         (((_v) >> (_b)) & 0x1)
#define BITS(_v, _l, _h)    (((uint32_t)(_v) << (31-(_h))) >> ((_l)+31-(_h)))

/*********************************************************************
 *
 *  ia32DecodeTable
 *
 *   The decode table is provided to save you typing. The actual table is
 *   defined at the bottom of the file due to its size. Each opcode has an
 *   entry in the table. 2 byte opcodes are encoded as 0x1XX. For example the
 *   opcode 0x0f 0x83 would be encoded as 0x183. See Figure 2.12b in the text
 *   for further explaination.
 *
 *********************************************************************/

#define IA32_DATA            0       // default
#define IA32_notimpl         1
#define IA32_PREFIX          2
#define IA32_2BYTE           3
#define IA32_CFLOW           4
#define IA32_DECODE_TYPE(_d) ((_d) & 0x000f)

/*
 *  The following defines add extra information on top of the decode type.
 */

#define IA32_MODRM           0x0010
#define IA32_IMM8            0x0020   // REL8 also
#define IA32_IMM32           0x0040   // REL32 also

#define IA32_RET             0x0100
#define IA32_JCC             0x0200
#define IA32_JMP             0x0400
#define IA32_CALL            0x0800

extern unsigned ia32DecodeTable[]; /* see below */

/*********************************************************************
 *
 *  IA32Instr
 *
 *   Decoded information about a single ia32 instruction.
 *
 *********************************************************************/

typedef struct {
   uint16_t  opcode;
   uint8_t   len;
   unsigned  modRM;
   uint32_t  imm;
} IA32Instr;

/*********************************************************************
 *
 *  IA32Instr
 *
 *   Program registers saved on a callout. These must be restored
 *   when we return to the program.
 *
 *     pc - this is the return address of the callout
 *     retPC - this is only valid if the callout replaced a RET 
 *             instruction. This will be the return PC the ret
 *             will jump to.
 *
 *********************************************************************/

typedef struct {
   uint32_t   eflags;
   uint32_t   edi;
   uint32_t   esi;
   uint32_t   ebp;
   uint32_t   esp;
   uint32_t   ebx;
   uint32_t   edx;
   uint32_t   ecx;
   uint32_t   eax;
   void      *pc;
   void      *retPC;
} IA32SaveRegs;

/* addresses of asm callout glue code */

extern void* jccCallout;
extern void* jmpCallout;
extern void* callCallout;
extern void* retCallout;
void *callTarget;


/*********************************************************************
 *
 *  ia32Decode
 *
 *   Decode an IA32 instruction.
 *
 *********************************************************************/

void 
ia32Decode(uint8_t *ptr, IA32Instr *instr)
{
   // Assume opcode is 1-byte
   instr->opcode = *ptr;
   instr->len = 1;

   // Encode if it's in fact 2-byte
   if (instr->opcode == 0x0f) {
      ptr++;
      instr->opcode = 0x100 | *(ptr);
      instr->len++;
   }

   unsigned decode = ia32DecodeTable[instr->opcode];

   // Check immediate
   if ((decode & 0xf0) >= IA32_IMM32) {
      instr->len += 4;
      decode -= IA32_IMM32;
   }
   else if ((decode & 0xf0) >= IA32_IMM8) {
      instr->len += 1;
      decode -= IA32_IMM8;
   }

   // Check MODRM
   if((decode & 0xf0) >= IA32_MODRM){
      instr->len += 1;
      decode -= IA32_MODRM;
      
      ptr++;
      unsigned char modrm = ((unsigned char) *(ptr));

      // Check SIB bytes
      if((modrm & 0x07 == 0x04) &&    // *****100
      ((modrm & 0xc0) == 0x00 ||   // 00******
      (modrm & 0xc0) == 0x40 ||   // 01******
      (modrm & 0xc0) == 0x80 )){  // 10******
         instr->len += 1;
      }       

      // Check displacement bytes
      if((modrm & 0xc7) == 0x05 ||     // 00***101
      (modrm & 0xc0) == 0x80){      // 10******
         instr->len += 4;
      }
      if((modrm & 0xc0) == 0x40){      // 01*******
         instr->len += 1;
      }
   }
}


/*********************************************************************
 *
 *  callout handlers
 *
 *   These get called by asm glue routines.
 *
 *********************************************************************/

void
handleCallCallout(IA32SaveRegs regs)
{
   NOT_IMPLEMENTED();
}

void
handleRetCallout(IA32SaveRegs regs)
{
   NOT_IMPLEMENTED();
}


/*********************************************************************
 *
 *  StartProfiling, StopProfiling
 *
 *   Profiling hooks. This is your place to inspect and modify the profiled
 *   function.
 *
 *********************************************************************/

void
StartProfiling(void *func)
{
   /* inspect/modify func here */
   unsigned char *p = (unsigned char *)func;
   IA32Instr *instr = malloc(sizeof(IA32Instr));

   for (int i = 0; i < 24; i++) {
      instr->len = 0;
      instr->opcode = 0;

      unsigned char *addr = p;
      ia32Decode(p, instr);

      p = p + instr->len;
      printf("address %p, opcode: %02x, len: %d\n", addr, instr->opcode, instr->len);
   }
}


/*********************************************************************
 *
 *  fib, main
 *
 *   This is the user program to be profiled.
 *
 *********************************************************************/

int fib(int i)
{
   if (i <= 1) {
      return 1;
   }

   return fib(i-1) + fib(i-2);
}

int main(int argc, char *argv[])
{
   int value;
   char *end;

   if (argc != 2) {
      fprintf(stderr, "usage: %s <value>\n", argv[0]);
      exit(1);
   }

   value = strtol(argv[1], &end, 10);

   if (((errno == ERANGE) 
        && ((value == LONG_MAX) || (value == LONG_MIN)))
       || ((errno != 0) && (value == 0))) {
      perror("strtol");
      exit(1);
   }

   if (end == argv[1]) {
      fprintf(stderr, "error: %s is not an integer\n", argv[1]);
      exit(1);
   }

   if (*end != '\0') {
      fprintf(stderr, "error: junk at end of parameter: %s\n", end);
      exit(1);
   }

   StartProfiling(fib);

   value = fib(value);
   
   printf("%d\n", value);
   exit(0);
}

unsigned ia32DecodeTable[] = {
   IA32_MODRM,                  IA32_MODRM,         // 00
   IA32_MODRM,                  IA32_MODRM,   
   IA32_IMM8,                   IA32_IMM32,   
   0,                           0,
   IA32_MODRM,                  IA32_MODRM,
   IA32_MODRM,                  IA32_MODRM,   
   IA32_IMM8,                   IA32_IMM32,   
   0,                           IA32_2BYTE,
   IA32_MODRM,                  IA32_MODRM,         // 10
   IA32_MODRM,                  IA32_MODRM,   
   IA32_IMM8,                   IA32_IMM32,   
   0,                           0,
   IA32_MODRM,                  IA32_MODRM,
   IA32_MODRM,                  IA32_MODRM,   
   IA32_IMM8,                   IA32_IMM32,   
   0,                           0,
   IA32_MODRM,                  IA32_MODRM,         // 20
   IA32_MODRM,                  IA32_MODRM,   
   IA32_IMM8,                   IA32_IMM32,   
   IA32_PREFIX,                 0,
   IA32_MODRM,                  IA32_MODRM,
   IA32_MODRM,                  IA32_MODRM,   
   IA32_IMM8,                   IA32_IMM32,   
   0,                           0,
   IA32_MODRM,                  IA32_MODRM,         // 30
   IA32_MODRM,                  IA32_MODRM,   
   IA32_IMM8,                   IA32_IMM32,   
   IA32_PREFIX,                 0,
   IA32_MODRM,                  IA32_MODRM,
   IA32_MODRM,                  IA32_MODRM,   
   IA32_IMM8,                   IA32_IMM32,   
   0,                           0,
   0,                           0,                  // 40
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,                  // 50
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,                  // 60
   IA32_notimpl,                IA32_notimpl, 
   IA32_PREFIX,                 IA32_PREFIX,
   IA32_PREFIX,                 IA32_PREFIX,
   IA32_IMM32,                  IA32_MODRM | IA32_IMM32,
   IA32_IMM8,                   IA32_MODRM | IA32_IMM8,
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_CFLOW | IA32_JCC | IA32_IMM8,  IA32_CFLOW | IA32_JCC | IA32_IMM8, // 70
   IA32_CFLOW | IA32_JCC | IA32_IMM8,  IA32_CFLOW | IA32_JCC | IA32_IMM8,
   IA32_CFLOW | IA32_JCC | IA32_IMM8,  IA32_CFLOW | IA32_JCC | IA32_IMM8,
   IA32_CFLOW | IA32_JCC | IA32_IMM8,  IA32_CFLOW | IA32_JCC | IA32_IMM8,
   IA32_CFLOW | IA32_JCC | IA32_IMM8,  IA32_CFLOW | IA32_JCC | IA32_IMM8,
   IA32_CFLOW | IA32_JCC | IA32_IMM8,  IA32_CFLOW | IA32_JCC | IA32_IMM8,
   IA32_CFLOW | IA32_JCC | IA32_IMM8,  IA32_CFLOW | IA32_JCC | IA32_IMM8,
   IA32_CFLOW | IA32_JCC | IA32_IMM8,  IA32_CFLOW | IA32_JCC | IA32_IMM8,
   IA32_MODRM | IA32_IMM8,      IA32_MODRM | IA32_IMM32,      // 80
   IA32_MODRM | IA32_IMM8,      IA32_MODRM | IA32_IMM8,
   IA32_MODRM,                  IA32_MODRM,
   IA32_MODRM,                  IA32_MODRM,
   IA32_MODRM,                  IA32_MODRM,
   IA32_MODRM,                  IA32_MODRM,
   IA32_MODRM,                  IA32_MODRM,
   IA32_MODRM,                  IA32_MODRM,
   0,                           0,                  // 90
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,
   0,                           0,
   IA32_notimpl,                IA32_notimpl,       // a0
   IA32_notimpl,                IA32_notimpl, 
   0,                           0,
   0,                           0,
   IA32_IMM8,                   IA32_IMM32,
   0,                           0,
   0,                           0,
   0,                           0,
   IA32_IMM8,                   IA32_IMM8,         // b0
   IA32_IMM8,                   IA32_IMM8,    
   IA32_IMM8,                   IA32_IMM8,    
   IA32_IMM8,                   IA32_IMM8,    
   IA32_IMM32,                  IA32_IMM32,    
   IA32_IMM32,                  IA32_IMM32,    
   IA32_IMM32,                  IA32_IMM32,    
   IA32_IMM32,                  IA32_IMM32,    
   IA32_MODRM | IA32_IMM8,      IA32_MODRM | IA32_IMM8,  // c0
   IA32_notimpl,                IA32_CFLOW | IA32_RET, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_MODRM | IA32_IMM8,      IA32_MODRM | IA32_IMM32,
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // d0
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // e0
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_CFLOW | IA32_CALL | IA32_IMM32,      IA32_notimpl, 
   IA32_notimpl,                IA32_CFLOW | IA32_JMP | IA32_IMM8, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // f0
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // 100
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // 110
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // 120
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // 130
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // 140
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // 150
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // 160
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // 170
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_CFLOW | IA32_JCC | IA32_IMM32, IA32_CFLOW | IA32_JCC | IA32_IMM32,// 180
   IA32_CFLOW | IA32_JCC | IA32_IMM32, IA32_CFLOW | IA32_JCC | IA32_IMM32,
   IA32_CFLOW | IA32_JCC | IA32_IMM32, IA32_CFLOW | IA32_JCC | IA32_IMM32,
   IA32_CFLOW | IA32_JCC | IA32_IMM32, IA32_CFLOW | IA32_JCC | IA32_IMM32,
   IA32_CFLOW | IA32_JCC | IA32_IMM32, IA32_CFLOW | IA32_JCC | IA32_IMM32,
   IA32_CFLOW | IA32_JCC | IA32_IMM32, IA32_CFLOW | IA32_JCC | IA32_IMM32,
   IA32_CFLOW | IA32_JCC | IA32_IMM32, IA32_CFLOW | IA32_JCC | IA32_IMM32,
   IA32_CFLOW | IA32_JCC | IA32_IMM32, IA32_CFLOW | IA32_JCC | IA32_IMM32,
   IA32_notimpl,                IA32_notimpl,       // 190
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // 1a0
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // 1b0
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_MODRM,                  IA32_MODRM,
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_MODRM,                  IA32_MODRM,
   IA32_notimpl,                IA32_notimpl,       // 1c0
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // 1d0
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // 1e0
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl,       // 1f0
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
   IA32_notimpl,                IA32_notimpl, 
};


