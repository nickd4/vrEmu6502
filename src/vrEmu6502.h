/*
 * Troy's 6502 Emulator
 *
 * Copyright (c) 2022 Troy Schrapel
 *
 * This code is licensed under the MIT license
 *
 * https://github.com/visrealm/vrEmu6502
 *
 */

#ifndef _VR_EMU_6502_H_
#define _VR_EMU_6502_H_

#include <stdlib.h> // for NULL

/* ------------------------------------------------------------------
 * LINKAGE MODES:
 * 
 * Default (nothing defined):    When your executable is using vrEmuTms9918 as a DLL
 * VR_6502_EMU_COMPILING_DLL:    When compiling vrEmuTms9918 as a DLL
 * VR_6502_EMU_STATIC:           When linking vrEmu6502 statically in your executable
 */

#if VR_6502_EMU_COMPILING_DLL
  #define VR_EMU_6502_DLLEXPORT __declspec(dllexport)
#elif VR_6502_EMU_STATIC
  #ifdef __cplusplus
    #define VR_EMU_6502_DLLEXPORT extern "C"
  #else
    #define VR_EMU_6502_DLLEXPORT extern
  #endif
#elif __EMSCRIPTEN__
  #include <emscripten.h>
  #define VR_EMU_6502_DLLEXPORT EMSCRIPTEN_KEEPALIVE
#else
  #define VR_EMU_6502_DLLEXPORT __declspec(dllimport)
#endif

#include <stdint.h>
#include <stdbool.h>  

/* ------------------------------------------------------------------
 * PRIVATE DATA STRUCTURE
 */
struct vrEmu6502_s;
typedef struct vrEmu6502_s VrEmu6502;

/* ------------------------------------------------------------------
 * CONSTANTS
 */
typedef enum
{
  CPU_6502,     /* NMOS 6502/6510 with documented opcodes only */
  CPU_6502U,    /* NMOS 6502/6510 with undocumented opcodes */
  CPU_65C02,    /* Standard CMOS 65C02 */
  CPU_W65C02,   /* Western Design Centre CMOS 65C02 */
  CPU_R65C02,   /* Rockwell CMOS 65C02 */
  CPU_6510 = CPU_6502U,
  CPU_8500 = CPU_6510,
  CPU_8502 = CPU_8500,
  CPU_7501 = CPU_6502,
  CPU_8501 = CPU_6502
} vrEmu6502Model;

typedef enum
{
  IntRequested,
  IntCleared,
  IntLow = IntRequested,
  IntHigh = IntCleared
} vrEmu6502Interrupt;

typedef enum
{
  BitC = 0,
  BitZ,
  BitI,
  BitD,
  BitB,
  BitU,
  BitV,
  BitN
} vrEmu6502FlagBit;

typedef enum
{
  FlagC = 0x01 << BitC,  /* carry */
  FlagZ = 0x01 << BitZ,  /* zero */
  FlagI = 0x01 << BitI,  /* interrupt */
  FlagD = 0x01 << BitD,  /* decimal */
  FlagB = 0x01 << BitB,  /* _brk */
  FlagU = 0x01 << BitU,  /* undefined */
  FlagV = 0x01 << BitV,  /* oVerflow */
  FlagN = 0x01 << BitN   /* negative */
} vrEmu6502Flag;


typedef enum
{
  AddrModeAbs,
  AddrModeAbsX,
  AddrModeAbsY,
  AddrModeAcc,
  AddrModeImm,
  AddrModeImp,
  AddrModeAbsInd,
  AddrModeAbsIndX,
  AddrModeIndX,
  AddrModeIndY,
  AddrModeRel,
  AddrModeZP,
  AddrModeZPI,
  AddrModeZPX,
  AddrModeZPY,
} vrEmu6502AddrMode;

/* ------------------------------------------------------------------
 * PUBLIC INTERFACE
 */

 /*
  * memory write function pointer
  */
typedef void(*vrEmu6502MemWrite)(uint16_t addr, uint8_t val);

/*
  * memory read function pointer
  * 
  * isDbg: some devices change their state when read 
  *        (eg. TMS9918 increments its address pointer)
  *        this flag will be false when the cpu is running
  *        however it can be true when querying the memory 
  *        for other purposes. devices should NOT change state
  *        when isDbg is true.
  *        
  */
typedef uint8_t(*vrEmu6502MemRead)(uint16_t addr/*, bool isDbg*/);


/*
 * create a new 6502
 */
VR_EMU_6502_DLLEXPORT VrEmu6502* vrEmu6502New(
                                    vrEmu6502Model model,
                                    vrEmu6502MemRead readFn,
                                    vrEmu6502MemWrite writeFn);

/* ------------------------------------------------------------------
 *
 * destroy a 6502
 */
VR_EMU_6502_DLLEXPORT void vrEmu6502Destroy(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * reset the 6502
 */
VR_EMU_6502_DLLEXPORT void vrEmu6502Reset(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * a single clock tick
 */
#if 1 // Nick
VR_EMU_6502_DLLEXPORT int vrEmu6502RunInstrs(VrEmu6502* vr6502, int n, int *cycles);
VR_EMU_6502_DLLEXPORT int vrEmu6502RunCycles(VrEmu6502* vr6502, int n, int *cycles);
VR_EMU_6502_DLLEXPORT void vrEmu6502Jam(VrEmu6502* vr6502);
VR_EMU_6502_DLLEXPORT void vrEmu6502Unjam(VrEmu6502* vr6502);
#else
VR_EMU_6502_DLLEXPORT void vrEmu6502Tick(VrEmu6502* vr6502);
#endif

/* ------------------------------------------------------------------
 *
 * returns a pointer to the interrupt signal.
 * externally, you can modify it to set/reset the interrupt signal
 */
VR_EMU_6502_DLLEXPORT vrEmu6502Interrupt *vrEmu6502Int(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * returns a pointer to the nmi signal.
 * externally, you can modify it to set/reset the interrupt signal
 */
VR_EMU_6502_DLLEXPORT vrEmu6502Interrupt *vrEmu6502Nmi(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * return the program counter
 */
VR_EMU_6502_DLLEXPORT uint16_t vrEmu6502GetPC(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * set the program counter
 */
VR_EMU_6502_DLLEXPORT void vrEmu6502SetPC(VrEmu6502* vr6502, uint16_t pc);

/* ------------------------------------------------------------------
 *
 * return the accumulator
 */
VR_EMU_6502_DLLEXPORT uint8_t vrEmu6502GetAcc(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * set the accumulator
 */
VR_EMU_6502_DLLEXPORT void vrEmu6502SetAcc(VrEmu6502* vr6502, uint8_t ac);

/* ------------------------------------------------------------------
 *
 * return the x index register
 */
VR_EMU_6502_DLLEXPORT uint8_t vrEmu6502GetX(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * set the x index register
 */
VR_EMU_6502_DLLEXPORT void vrEmu6502SetX(VrEmu6502* vr6502, uint8_t ix);

/* ------------------------------------------------------------------
 *
 * return the y index register
 */
VR_EMU_6502_DLLEXPORT uint8_t vrEmu6502GetY(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * set the y index register
 */
VR_EMU_6502_DLLEXPORT void vrEmu6502SetY(VrEmu6502* vr6502, uint8_t iy);

/* ------------------------------------------------------------------
 *
 * return the processor status register
 */
VR_EMU_6502_DLLEXPORT uint8_t vrEmu6502GetStatus(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * set the processor status register
 */
VR_EMU_6502_DLLEXPORT void vrEmu6502SetStatus(VrEmu6502* vr6502, uint8_t flags);

/* ------------------------------------------------------------------
 *
 * return the stack pointer register
 */
VR_EMU_6502_DLLEXPORT uint8_t vrEmu6502GetStackPointer(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * set the stack pointer register
 */
VR_EMU_6502_DLLEXPORT void vrEmu6502SetStackPointer(VrEmu6502* vr6502, uint8_t sp);

/* ------------------------------------------------------------------
 *
 * return the current opcode
 */
VR_EMU_6502_DLLEXPORT uint8_t vrEmu6502GetCurrentOpcode(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * return the current opcode address
 */
VR_EMU_6502_DLLEXPORT uint16_t vrEmu6502GetCurrentOpcodeAddr(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * return the next opcode
 */
VR_EMU_6502_DLLEXPORT uint8_t vrEmu6502GetNextOpcode(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * return the opcode cycle
 */
VR_EMU_6502_DLLEXPORT uint8_t vrEmu6502GetOpcodeCycle(VrEmu6502* vr6502);

/* ------------------------------------------------------------------
 *
 * return the opcode mnemonic string
 */
VR_EMU_6502_DLLEXPORT
const char* vrEmu6502OpcodeToMnemonicStr(VrEmu6502* vr6502, uint8_t opcode);

/* ------------------------------------------------------------------
 *
 * return the opcode address mode
 */
VR_EMU_6502_DLLEXPORT
vrEmu6502AddrMode vrEmu6502GetOpcodeAddrMode(VrEmu6502* vr6502, uint8_t opcode);

/* ------------------------------------------------------------------
 *
 * get disassembled instruction as a string. returns next instruction address
 */
VR_EMU_6502_DLLEXPORT
uint16_t vrEmu6502DisassembleInstruction(
  VrEmu6502* vr6502, uint16_t addr,
  int bufferSize, char *buffer,
  uint16_t *refAddr, const char* const labelMap[0x10000]);

/* ------------------------------------------------------------------
 * PRIVATE DATA STRUCTURE
 */

 /*
  * address mode function prototype
  */
typedef uint16_t(*vrEmu6502AddrModeFn)(VrEmu6502*);

 /*
  * instruction function prototype
  */
typedef void(*vrEmu6502InstructionFn)(VrEmu6502*, vrEmu6502AddrModeFn);

struct vrEmu6502Opcode
{
  const vrEmu6502InstructionFn  instruction;
  const vrEmu6502AddrModeFn     addrMode;
  const uint8_t                 cycles;
};
typedef struct vrEmu6502Opcode vrEmu6502Opcode;

struct vrEmu6502_s
{
  vrEmu6502Model model;
  
  vrEmu6502MemRead readFn;
  vrEmu6502MemWrite writeFn;
  
  vrEmu6502Interrupt intPin;
  vrEmu6502Interrupt nmiPin;

  uint8_t step;
  uint8_t inNmi;
  uint8_t currentOpcode;
  uint16_t currentOpcodeAddr;
  uint8_t wai;

  uint16_t pc;

  uint8_t  ac;
  uint8_t  ix;
  uint8_t  iy;
  uint8_t  sp;

  uint8_t  flags;

  uint16_t zpBase;
  uint16_t spBase;
  uint16_t tmpAddr;
  uint8_t jam;

  const vrEmu6502Opcode *opcodes;
  const char* mnemonicNames[256];
  vrEmu6502AddrMode addrModes[256];
};
 
/* ------------------------------------------------------------------
 *  ADDRESS MODES
 * ----------------------------------------------------------------*/

uint16_t ab(VrEmu6502* vr6502);
uint16_t abx(VrEmu6502* vr6502);
uint16_t aby(VrEmu6502* vr6502);
uint16_t axp(VrEmu6502* vr6502);
uint16_t ayp(VrEmu6502* vr6502);
uint16_t imm(VrEmu6502* vr6502);
uint16_t ind(VrEmu6502* vr6502);
uint16_t indx(VrEmu6502* vr6502);
uint16_t rel(VrEmu6502* vr6502);
uint16_t xin(VrEmu6502* vr6502);
uint16_t yin(VrEmu6502* vr6502);
uint16_t yip(VrEmu6502* vr6502);
uint16_t zp(VrEmu6502* vr6502);
uint16_t zpi(VrEmu6502* vr6502);
uint16_t zpx(VrEmu6502* vr6502);
uint16_t zpy(VrEmu6502* vr6502);

/*
 * accumulator mode (no address) - eg. ror
 */
#define acc NULL

/*
 * implied mode (no address) - eg. tax
 */
#define imp NULL

/* ------------------------------------------------------------------
 *  INSTRUCTIONS
 * ----------------------------------------------------------------*/

void adc(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void and(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void asl(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bra(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bcc(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bcs(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void beq(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bit(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bmi(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bne(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bpl(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void _brk(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bvc(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bvs(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void clc(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void cld(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void cli(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void clv(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void cmp(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void cpx(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void cpy(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void dec(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void dex(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void dey(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void eor(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void inc(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void inx(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void iny(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void jmp(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void jsr(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void lda(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void ldx(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void ldy(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void lsr(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void nop(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void ldd(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void ora(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void pha(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void php(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void phx(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void phy(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void pla(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void plp(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void plx(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void ply(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void rol(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void ror(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void rti(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void rts(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void sbc(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void sec(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void sed(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void sei(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void sta(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void stx(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void sty(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void stz(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void tax(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void tay(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void tsx(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void txa(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void txs(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void tya(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void trb(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void tsb(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void rmb(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr, int bitIndex);
void rmb0(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void rmb1(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void rmb2(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void rmb3(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void rmb4(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void rmb5(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void rmb6(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void rmb7(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void smb(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr, int bitIndex);
void smb0(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void smb1(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void smb2(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void smb3(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void smb4(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void smb5(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void smb6(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void smb7(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbr(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr, int bitIndex);
void bbr0(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbr1(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbr2(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbr3(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbr4(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbr5(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbr6(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbr7(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbs(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr, int bitIndex);
void bbs0(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbs1(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbs2(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbs3(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbs4(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbs5(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbs6(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void bbs7(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void stp(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);
void wai(VrEmu6502* vr6502, vrEmu6502AddrModeFn modeAddr);

#endif // _VR_EMU_6502_CORE_H_
