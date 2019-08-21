// Copyright 2017 Embecosm Limited <www.embecosm.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Simple Verilator model test bench
// Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>
// Contributor Graham Markall <graham.markall@embecosm.com>

#include "verilated.h"
#include "verilated_vcd_c.h"
#include "Vtop.h"
#include "Vtop__Syms.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <vector>

#include "cxxopts.hpp"
#include "scarv_cop_common.h"

using std::cout;
using std::cerr;
using std::endl;
using namespace std;

// Count of clock ticks

static vluint64_t  cpuTime = 0;

// Debug registers

const uint16_t DBG_CTRL    = 0x0000;  //!< Debug control
const uint16_t DBG_HIT     = 0x0004;  //!< Debug hit
const uint16_t DBG_IE      = 0x0008;  //!< Debug interrupt enable
const uint16_t DBG_CAUSE   = 0x000c;  //!< Debug cause (why entered debug)
const uint16_t DBG_GPR0    = 0x0400;  //!< General purpose register 0
const uint16_t DBG_GPR31   = 0x047c;  //!< General purpose register 41
const uint16_t DBG_NPC     = 0x2000;  //!< Next PC
const uint16_t DBG_PPC     = 0x2004;  //!< Prev PC

// Debug register flags

const uint32_t DBG_CTRL_HALT = 0x00010000;    //!< Halt core
const uint32_t DBG_CTRL_SSTE = 0x00000001;    //!< Single step core

static uint64_t mCycleCnt = 0;

Vtop *cpu;
VerilatedVcdC * tfp;
bool passed = true;

/* Test cases for subclass decoding */

typedef struct {
  uint32_t mask;
  uint32_t instr;
  uint32_t subclass;
  std::string signal;
  std::string mnemonic;
} decode_testcase;

std::vector<decode_testcase> decode_testcases = {
  { 0xfff8707f, 0x1000002b, SCARV_COP_SCLASS_XCR2GPR,       "dec_xcr2gpr",       "xc.xcr2gpr",       },
  { 0xfff0787f, 0x1100002b, SCARV_COP_SCLASS_GPR2XCR,       "dec_gpr2xcr",       "xc.gpr2xcr",       },
  { 0xfff87fff, 0x0800002b, SCARV_COP_SCLASS_RSEED,         "dec_rngseed",       "xc.rngseed",       },
  { 0xfffff87f, 0x0810082b, SCARV_COP_SCLASS_RSAMP,         "dec_rngsamp",       "xc.dec_rngsamp",   },
  { 0xfffff07f, 0x0820002b, SCARV_COP_SCLASS_RTEST,         "dec_rngtest",       "xc.rngtest",       },
  { 0xff08787f, 0x1800082b, SCARV_COP_SCLASS_CMOV_T,        "dec_cmov_t",        "xc.cmov_t",        },
  { 0xff08787f, 0x1800002b, SCARV_COP_SCLASS_CMOV_F,        "dec_cmov_f",        "xc.cmov_f",        },
  { 0x1f08787f, 0x0600002b, SCARV_COP_SCLASS_PADD,          "dec_padd",          "xc.padd",          },
  { 0x1f08787f, 0x0600082b, SCARV_COP_SCLASS_PSUB,          "dec_psub",          "xc.psub",          },
  { 0x1f08787f, 0x0e00002b, SCARV_COP_SCLASS_PMUL_L,        "dec_pmul_l",        "xc.pmul_l",        },
  { 0x1f08787f, 0x0e00082b, SCARV_COP_SCLASS_PMUL_H,        "dec_pmul_h",        "xc.pmul_h",        },
  { 0x1f08787f, 0x0e08002b, SCARV_COP_SCLASS_PCLMUL_L,      "dec_pclmul_l",      "xc.pclmul_l",      },
  { 0x1f08787f, 0x0e08082b, SCARV_COP_SCLASS_PCLMUL_H,      "dec_pclmul_h",      "xc.pclmul_h",      },
  { 0x1f08787f, 0x1600002b, SCARV_COP_SCLASS_PSLL,          "dec_psll",          "xc.psll",          },
  { 0x1f08787f, 0x1600082b, SCARV_COP_SCLASS_PSRL,          "dec_psrl",          "xc.psrl",          },
  { 0x1f08787f, 0x1608082b, SCARV_COP_SCLASS_PROT,          "dec_prot",          "xc.prot",          },
  { 0x1e08787f, 0x1e00002b, SCARV_COP_SCLASS_PSLL_I,        "dec_psll_i",        "xc.psll_i",        },
  { 0x1e08787f, 0x1e00082b, SCARV_COP_SCLASS_PSRL_I,        "dec_psrl_i",        "xc.psrl_i",        },
  { 0x1e08787f, 0x1e08082b, SCARV_COP_SCLASS_PROT_I,        "dec_prot_i",        "xc.prot_i",        },
  { 0x3e00707f, 0x0c00002b, SCARV_COP_SCLASS_SHA3_XY,       "dec_sha3_xy",       "xc.sha3_xy",       },
  { 0x3e00707f, 0x1400002b, SCARV_COP_SCLASS_SHA3_X1,       "dec_sha3_x1",       "xc.sha3_x1",       },
  { 0x3e00707f, 0x2400002b, SCARV_COP_SCLASS_SHA3_X2,       "dec_sha3_x2",       "xc.sha3_x2",       },
  { 0x3e00707f, 0x1c00002b, SCARV_COP_SCLASS_SHA3_X4,       "dec_sha3_x4",       "xc.sha3_x4",       },
  { 0x3e00707f, 0x3400002b, SCARV_COP_SCLASS_SHA3_YX,       "dec_sha3_yx",       "xc.sha3_yx",       },
  { 0xff08787f, 0x0400002b, SCARV_COP_SCLASS_AESSUB_ENC,    "dec_aessub_enc",    "xc.aessub_enc",    },
  { 0xff08787f, 0x0400082b, SCARV_COP_SCLASS_AESSUB_ENCROT, "dec_aessub_encrot", "xc.aessub_encrot", },
  { 0xff08787f, 0x0408002b, SCARV_COP_SCLASS_AESSUB_DEC,    "dec_aessub_dec",    "xc.aessub_dec",    },
  { 0xff08787f, 0x0408082b, SCARV_COP_SCLASS_AESSUB_DECROT, "dec_aessub_decrot", "xc.aessub_decrot", },
  { 0xff08787f, 0x0500002b, SCARV_COP_SCLASS_AESMIX_ENC,    "dec_aesmix_enc",    "xc.aesmix_enc",    },
  { 0xff08787f, 0x0508002b, SCARV_COP_SCLASS_AESMIX_DEC,    "dec_aesmix_dec",    "xc.aesmix_dec",    },
  { 0x3e00787f, 0x0200002b, SCARV_COP_SCLASS_LDR_B,         "dec_ldr_bu",        "xc.ldr_bu",        },
  { 0x3e00787f, 0x1200002b, SCARV_COP_SCLASS_LDR_H,         "dec_ldr_hu",        "xc.ldr_hu",        },
  { 0xfe00787f, 0x2200002b, SCARV_COP_SCLASS_LDR_W,         "dec_ldr_w",         "xc.ldr_w",         },
  { 0x3e00787f, 0x0200082b, SCARV_COP_SCLASS_STR_B,         "dec_str_b",         "xc.str_b",         },
  { 0x3e00787f, 0x1200082b, SCARV_COP_SCLASS_STR_H,         "dec_str_h",         "xc.str_h",         },
  { 0xfe00787f, 0x2200082b, SCARV_COP_SCLASS_STR_W,         "dec_str_w",         "xc.str_w",         },
  { 0xff00787f, 0x0a00082b, SCARV_COP_SCLASS_SCATTER_B,     "dec_scatter_b",     "xc.scatter_b",     },
  { 0xff00787f, 0x0a00002b, SCARV_COP_SCLASS_GATHER_B,      "dec_gather_b",      "xc.gather_b",      },
  { 0xff00787f, 0x0b00082b, SCARV_COP_SCLASS_SCATTER_H,     "dec_scatter_h",     "xc.scatter_h",     },
  { 0xff00787f, 0x0b00002b, SCARV_COP_SCLASS_GATHER_H,      "dec_gather_h",      "xc.gather_h",      },
  { 0x0008787f, 0x0000682b, SCARV_COP_SCLASS_BOP,           "dec_bop",           "xc.bop",           },
  { 0xf000707f, 0x0000702b, SCARV_COP_SCLASS_MEQU,          "dec_mequ",          "xc.mequ",          },
  { 0xf000707f, 0x1000702b, SCARV_COP_SCLASS_MLTE,          "dec_mlte",          "xc.mlte",          },
  { 0xf000707f, 0x2000702b, SCARV_COP_SCLASS_MGTE,          "dec_mgte",          "xc.mgte",          },
  { 0xf008787f, 0x3008702b, SCARV_COP_SCLASS_LUT,           "dec_lut",           "xc.lut",           },
  { 0xf0087c7f, 0x4000702b, SCARV_COP_SCLASS_MADD_3,        "dec_madd_3",        "xc.madd_3",        },
  { 0xf0087c7f, 0x6000702b, SCARV_COP_SCLASS_MSUB_3,        "dec_msub_3",        "xc.msub_3",        },
  { 0xff087c7f, 0x5000702b, SCARV_COP_SCLASS_MADD_2,        "dec_madd_2",        "xc.madd_2",        },
  { 0xff087c7f, 0x5100702b, SCARV_COP_SCLASS_MSUB_2,        "dec_msub_2",        "xc.msub_2",        },
  { 0xff087c7f, 0x5200702b, SCARV_COP_SCLASS_MACC_2,        "dec_macc_2",        "xc.macc_2",        },
  { 0xfff87c7f, 0x5f00702b, SCARV_COP_SCLASS_MACC_1,        "dec_macc_1",        "xc.macc_1",        },
  { 0xf0087c7f, 0x7000782b, SCARV_COP_SCLASS_MSLL,          "dec_msll",          "xc.msll",          },
  { 0xf0087c7f, 0x8000782b, SCARV_COP_SCLASS_MSRL,          "dec_msrl",          "xc.msrl",          },
  { 0xf0087c7f, 0x9000782b, SCARV_COP_SCLASS_MMUL_3,        "dec_mmul_3",        "xc.mmul_3",        },
  { 0xf0087c7f, 0xa000782b, SCARV_COP_SCLASS_MCLMUL_3,      "dec_mclmul_3",      "xc.mclmul_3",      },
  { 0xc0087c7f, 0xc000742b, SCARV_COP_SCLASS_MSLL_I,        "dec_msll_i",        "xc.msll_i",        },
  { 0xc0087c7f, 0x8000742b, SCARV_COP_SCLASS_MSRL_I,        "dec_msrl_i",        "xc.msrl_i",        },
  { 0x0000707f, 0x0000102b, SCARV_COP_SCLASS_LB_CR,         "dec_ld_bu",         "xc.ld_bu",         },
  { 0x0010707f, 0x0000202b, SCARV_COP_SCLASS_LH_CR,         "dec_ld_hu",         "xc.ld_hu",         },
  { 0x0010787f, 0x0000302b, SCARV_COP_SCLASS_LD_W,          "dec_ld_w",          "xc.ld_w",          },
  { 0x0010787f, 0x0010302b, SCARV_COP_SCLASS_LD_HIU,        "dec_ld_hiu",        "xc.ld_hiu",        },
  { 0x0010787f, 0x0010382b, SCARV_COP_SCLASS_LD_LIU,        "dec_ld_liu",        "xc.ld_liu",        },
  { 0x07f8787f, 0x0010202b, SCARV_COP_SCLASS_PERM_IBIT,     "dec_ipbit",         "xc.ipbit",         },
  { 0x07f8787f, 0x0010282b, SCARV_COP_SCLASS_PERM_BIT,      "dec_pbit",          "xc.pbit",          },
  { 0x00f8787f, 0x0030202b, SCARV_COP_SCLASS_PERM_BYTE,     "dec_pbyte",         "xc.pbyte",         },
  { 0x0038787f, 0x0030282b, SCARV_COP_SCLASS_INS,           "dec_ins",           "xc.ins",           },
  { 0x0038787f, 0x0038202b, SCARV_COP_SCLASS_BMV,           "dec_bmv",           "xc.bmv",           },
  { 0x0038787f, 0x0038282b, SCARV_COP_SCLASS_EXT,           "dec_ext",           "xc.ext",           },
  { 0x0000707f, 0x0000402b, SCARV_COP_SCLASS_ST_B,          "dec_st_b",          "xc.st_b",          },
  { 0x0100707f, 0x0000502b, SCARV_COP_SCLASS_ST_H,          "dec_st_h",          "xc.st_h",          },
  { 0x0100787f, 0x0000602b, SCARV_COP_SCLASS_ST_W,          "dec_st_w",          "xc.st_w",          },
};

/* Check that invariants hold. To be called on every clock cycle. */
void checkProperties(void) {
  auto idecode = cpu->top->riscv_core_i->id_stage_i->xcrypto_decoder;
  uint32_t encoded = idecode->read_encoded();
  uint32_t id_subclass = idecode->read_id_subclass();

  /* Check xc.init asserts id_cprs_init */
  if (encoded == 0x1110082B) {
    if (!idecode->read_id_cprs_init()) {
      std::cout << "id_cprs_init not asserted for xc.init." << std::endl;
      passed = false;
    }
  }

  /* Test subclass signals correctly asserted for each instruction */
  for (auto& t: decode_testcases) {
    if ((encoded & t.mask) == t.instr) {
      if (! (id_subclass & (1 << t.subclass))) {
        std::cout << t.signal << " not asserted for " << t.mnemonic << std::endl;
        passed = false;
      }
    }
  }
}

void writeInstr(uint32_t addr, uint32_t instr) {
  const auto &dp_ram = cpu->top->ram_i->dp_ram_i;
  //auto wbyte = cpu->top->ram_i->dp_ram_i->writeByte;
  dp_ram->writeByte(addr + 0x0, (instr >> 0 ) & 0xFF);
  dp_ram->writeByte(addr + 0x1, (instr >> 8 ) & 0xFF);
  dp_ram->writeByte(addr + 0x2, (instr >> 16) & 0xFF);
  dp_ram->writeByte(addr + 0x3, (instr >> 24) & 0xFF);
}

uint32_t readInstr(uint32_t addr) {
  const auto &dp_ram = cpu->top->ram_i->dp_ram_i;
  uint32_t instr = 0;
  for (int i = 0; i < 4; i++) { 
    instr |= (dp_ram->readByte(addr+i) << (8 * i));
  }
  return instr;
}

// Clock the CPU for a given number of cycles, dumping to the trace file at
// each clock edge.
void clockSpin(uint32_t cycles)
{
  for (uint32_t i = 0; i < cycles; i++)
  {
    cpu->clk_i = 0;
    cpu->eval ();
    cpuTime += 1;
    tfp->dump (cpuTime);
    cpu->clk_i = 1;
    cpu->eval ();
    cpuTime += 1;
    tfp->dump (cpuTime);
    mCycleCnt++;
    checkProperties();
  }
}

// Read/write a debug register.
void debugAccess(uint32_t addr, uint32_t& val, bool write_enable)
{
  cpu->debug_req_i   = 1;
  cpu->debug_addr_i  = addr;
  cpu->debug_we_i    = write_enable ? 1 : 0;

  if (write_enable)
  {
    cpu->debug_wdata_i = val;
  }

  // Access has been acknowledged when we get the grant signal asserted.
  do
    {
      clockSpin(1);
    }
  while (cpu->debug_gnt_o == 0);

  // Don't need to request once we get the grant.
  cpu->debug_req_i = 0;

  if (!write_enable)
  {
    // For reads, we need to read the data when we get rvalid_o.
    // This could be on the same cycle as the grant, or later.
    while (cpu->debug_rvalid_o == 0)
    {
      clockSpin(1);
    }
    val = cpu->debug_rdata_o;
  }
}

// Read a debug register
uint32_t debugRead(uint32_t addr)
{
  uint32_t val;
  debugAccess(addr, val, false);
  return val;
}

// Write a debug register
void debugWrite(uint32_t addr, uint32_t val)
{
  debugAccess(addr, val, true);
}

// Cycle the clock until the debug unit reports that the CPU is halted.
void waitForDebugStall()
{
  // Assume that stall could happen at any point - we don't need to wait a cycle
  // to check if it's stalled before reading
  while (!(debugRead(DBG_CTRL) & DBG_CTRL_HALT))
  {
    clockSpin(1);
  }
}

// Execution begins at 0x80, so that's where we write our code.
void loadProgram(string test_name)
{
  uint32_t addr = 0x80;

  const auto &dp_ram = cpu->top->ram_i->dp_ram_i;

  // open program.bin as a binary input stream
  ifstream program_stream (test_name, ios::binary);
  if (program_stream.fail()) {
    cerr << "Failed to open " << test_name << endl;
    exit(EXIT_FAILURE);
  }

  // read the contents of the program into buf
  std::vector<unsigned char> buf(std::istreambuf_iterator<char>(program_stream), {});

  // iterate over the bytes of the program we read into buf
  for (uint8_t byte : buf) {
    dp_ram->writeByte(addr++, byte);
  }
}

int
main (int    argc,
      char * argv[])
{
  string test_name;
  cxxopts::Options options("testbench", "RI5CY-Xcrypto testbench");
  options.add_options()
    ("test-name", "Path to test file to run", cxxopts::value<string>(), "<path>");
  options.positional_help("[test-name]");
  options.parse_positional({"test-name"});


  try {
    auto result = options.parse(argc, argv);

    if (result.count("test-name"))
      test_name = result["test-name"].as<string>();
    else {
      cerr << "Error: Test name is required." << endl << endl;
      cerr << options.help();
      return EXIT_FAILURE;
    }
  }
  catch (cxxopts::OptionException &e) {
    cerr << e.what() << endl;
    cerr << options.help();
    return EXIT_FAILURE;
  }

  // Instantiate the model
  cpu = new Vtop;

  // Open VCD
  Verilated::traceEverOn (true);
  tfp = new VerilatedVcdC;
  cpu->trace (tfp, 99);
  tfp->open ("model.vcd");

  // Fix some signals for now.
  cpu->irq_i          = 0;
  cpu->debug_req_i    = 0;
  cpu->fetch_enable_i = 0;

  // Cycle through reset
  cpu->rstn_i = 0;
  clockSpin(5);
  cpu->rstn_i = 1;

  // Put a few instructions in memory
  loadProgram(test_name);

  cout << "About to halt and set traps on exceptions" << endl;

  // Try to halt the CPU in the same way as in spi_debug_test.svh
  debugWrite(DBG_CTRL, debugRead(DBG_CTRL) | DBG_CTRL_HALT);

  // Let things run for a few cycles while the CPU waits in a halted state,
  // simply to check that doing so doesn't cause any errors.

  clockSpin(5);

  // Set traps on exceptions
  debugWrite(DBG_IE, 0xF);

  cout << "About to resume" << endl;

  uint32_t new_ctrl = debugRead(DBG_CTRL) & ~DBG_CTRL_HALT;
  debugWrite(DBG_CTRL, new_ctrl);

  cpu->fetch_enable_i = 1;

  cout << "Cycling clock to run for a few instructions" << endl;
  clockSpin(100);

  cout << "Halting" << endl;

  debugWrite(DBG_CTRL, debugRead(DBG_CTRL) | DBG_CTRL_HALT);
  waitForDebugStall();

  // Close VCD

  tfp->close ();

  // Tidy up

  delete tfp;
  delete cpu;

  return !passed;
}

//! Function to handle $time calls in the Verilog

double
sc_time_stamp ()
{
  return cpuTime;
}

// Local Variables:
// mode: C++
// c-file-style: "gnu"
// show-trailing-whitespace: t
// End:
