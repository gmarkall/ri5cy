// Simple Verilator model test bench

// Copyright (C) 2017  Embecosm Limited <www.embecosm.com>

// Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.

// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.

// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#include "verilated.h"
#include "verilated_vcd_c.h"
#include "Vtop.h"
#include "Vtop_top.h"
#include "Vtop_ram__A10.h"
#include "Vtop_dp_ram__A10.h"

#include <iostream>
#include <cstdint>
#include <cstdlib>

using std::cerr;
using std::endl;


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

  // FIXME: Changed to uint32_t to get to compile - constants too large. Check
  // the constants are correct.
  const uint32_t DBG_CTRL_HALT = 0x00010000;    //!< Halt core
  const uint32_t DBG_CTRL_SSTE = 0x00000001;    //!< Single step core

  // GDB register numbers

  const int REG_R0  = 0;                //!< GDB R0 regnum
  const int REG_R31 = 31;               //!< GDB R31 regnum

static uint64_t mCycleCnt = 0;


bool stepSingle (Vtop *mCpu)
{
// uint32_t pc;

  mCpu->debug_req_i   = 1;
  mCpu->debug_addr_i  = DBG_IE;
  mCpu->debug_we_i    = 0;

  // Write has succeeded when we get the grant signal asserted.
  do
    {
      mCpu->clk_i = 0;
      mCpu->eval ();
      mCpu->clk_i = 1;
      mCpu->eval ();
      mCycleCnt++;
      std::cout << "DBG_IE read cycle count " << mCycleCnt << std::endl;
    }
  while (mCpu->debug_gnt_o == 0);

  std::cout << "DBG_IE is " << std::hex << mCpu->debug_rdata_o << std::dec << std::endl;

 mCpu->rstn_i        = 1;

 // Write HALT + SSTE into the debug register

  mCpu->debug_req_i   = 1;
  mCpu->debug_addr_i  = DBG_CTRL;
  mCpu->debug_we_i    = 1;
  mCpu->debug_wdata_i = DBG_CTRL_SSTE; // & DBG_CTRL_HALT;

  // Write has succeeded when we get the grant signal asserted.
  do
    {
      mCpu->clk_i = 0;
      mCpu->eval ();
      mCpu->clk_i = 1;
      mCpu->eval ();
      mCycleCnt++;
      std::cout << "2 cycle count " << mCycleCnt << std::endl;
    }
  while (mCpu->debug_gnt_o == 0);

// Read debug hit

  mCpu->debug_req_i   = 1;
  mCpu->debug_addr_i  = DBG_HIT;
  mCpu->debug_we_i    = 0;

  // Write has succeeded when we get the grant signal asserted.
  do
    {
      mCpu->clk_i = 0;
      mCpu->eval ();
      mCpu->clk_i = 1;
      mCpu->eval ();
      mCycleCnt++;
      std::cout << "3 cycle count " << mCycleCnt << std::endl;
      std::cout << "DBG_HIT reg " << std::hex << mCpu->debug_rdata_o << std::dec << std::endl;
    }
  while (mCpu->debug_gnt_o == 0 && !(mCpu->debug_rdata_o & 1));


  // Clearing debug hit

  mCpu->debug_req_i   = 1;
  mCpu->debug_addr_i  = DBG_HIT;
  mCpu->debug_we_i    = 1;
  mCpu->debug_wdata_i = 0;

  // Write has succeeded when we get the grant signal asserted.
  do
    {
      mCpu->clk_i = 0;
      mCpu->eval ();
      mCpu->clk_i = 1;
      mCpu->eval ();
      mCycleCnt++;
      std::cout << "db_hit clear cycle count " << mCycleCnt << std::endl;
    }
  while (mCpu->debug_gnt_o == 0);

  return true;
}



int
main (int    argc,
      char * argv[])
{
  Vtop * cpu = new Vtop;
  VerilatedVcdC * tfp;

  // Open VCD

  Verilated::traceEverOn (true);
  tfp = new VerilatedVcdC;
  cpu->trace (tfp, 99);
  tfp->open ("model.vcd");

  // Fix some signals for now.

  //cpu->irq_i          = 0;
  cpu->debug_req_i    = 0;
  cpu->fetch_enable_i = 1;

  // Cycle through reset

  cpu->rstn_i = 0;

  for (int i = 0; i < 100; i++)
    {
      cpu->clk_i = 0;
      cpu->eval ();
      cpuTime += 5;
      tfp->dump (cpuTime);

      cpu->clk_i = 1;
      cpu->eval ();
      cpuTime += 5;
      tfp->dump (cpuTime);
    }

  // Write some program code into memory:
  //
  // ; Store a word to memory first:
  // li a5, 64
  // li a4, 102
  // sw a4, 0(a5)
  //
  // ; Then do something a bit like _exit(0)
  // li a1, 0
  // li a2, 0
  // li a3, 0
  // li a7, 93
  // ecall
  //
  // Execution begins at 0x80, so that's where we write our code.

  cpu->top->ram_i->dp_ram_i->writeByte (0x80, 0x93);
  cpu->top->ram_i->dp_ram_i->writeByte (0x81, 0x07);
  cpu->top->ram_i->dp_ram_i->writeByte (0x82, 0x00);
  cpu->top->ram_i->dp_ram_i->writeByte (0x83, 0x04);

  cpu->top->ram_i->dp_ram_i->writeByte (0x84, 0x13);
  cpu->top->ram_i->dp_ram_i->writeByte (0x85, 0x07);
  cpu->top->ram_i->dp_ram_i->writeByte (0x86, 0x60);
  cpu->top->ram_i->dp_ram_i->writeByte (0x87, 0x06);

  cpu->top->ram_i->dp_ram_i->writeByte (0x88, 0x23);
  cpu->top->ram_i->dp_ram_i->writeByte (0x89, 0xa0);
  cpu->top->ram_i->dp_ram_i->writeByte (0x8a, 0xe7);
  cpu->top->ram_i->dp_ram_i->writeByte (0x8b, 0x00);

  cpu->top->ram_i->dp_ram_i->writeByte (0x8c, 0x93);
  cpu->top->ram_i->dp_ram_i->writeByte (0x8d, 0x05);
  cpu->top->ram_i->dp_ram_i->writeByte (0x8e, 0x00);
  cpu->top->ram_i->dp_ram_i->writeByte (0x8f, 0x00);

  cpu->top->ram_i->dp_ram_i->writeByte (0x90, 0x13);
  cpu->top->ram_i->dp_ram_i->writeByte (0x91, 0x06);
  cpu->top->ram_i->dp_ram_i->writeByte (0x92, 0x00);
  cpu->top->ram_i->dp_ram_i->writeByte (0x93, 0x00);

  cpu->top->ram_i->dp_ram_i->writeByte (0x94, 0x93);
  cpu->top->ram_i->dp_ram_i->writeByte (0x95, 0x06);
  cpu->top->ram_i->dp_ram_i->writeByte (0x96, 0x00);
  cpu->top->ram_i->dp_ram_i->writeByte (0x97, 0x00);

  cpu->top->ram_i->dp_ram_i->writeByte (0x98, 0x93);
  cpu->top->ram_i->dp_ram_i->writeByte (0x99, 0x08);
  cpu->top->ram_i->dp_ram_i->writeByte (0x9a, 0xd0);
  cpu->top->ram_i->dp_ram_i->writeByte (0x9b, 0x05);

  cpu->top->ram_i->dp_ram_i->writeByte (0x9c, 0x73);
  cpu->top->ram_i->dp_ram_i->writeByte (0x9d, 0x00);
  cpu->top->ram_i->dp_ram_i->writeByte (0x9e, 0x00);
  cpu->top->ram_i->dp_ram_i->writeByte (0x9f, 0x00);

  // Do some ordinary clocked logic.

  // do this like we are single stepping in GDB to try to get the illegal instruction

  for (int j=0; j<5; j++) {
    stepSingle (cpu);
  }








/*
  cpu->rstn_i = 1;

  for (int i = 0; i < 100; i++)
    {
      cpu->clk_i = 0;
      cpu->eval ();
      cpuTime += 5;
      tfp->dump (cpuTime);

      cpu->clk_i = 1;
      cpu->eval ();
      cpuTime += 5;
      tfp->dump (cpuTime);
    }
*/
  // Close VCD

  tfp->close ();

  // Tidy up

  delete tfp;
  delete cpu;

}	// main ()


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
