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
#include "Vtop_ram.h"
#include "Vtop_sp_ram__A16.h"


// Count of clock ticks

static vluint64_t  cpuTime = 0;


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

  cpu->irq_i          = 0;
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

  cpu->top->ram_i->sp_ram_i->writeByte (0x80, 0x93);
  cpu->top->ram_i->sp_ram_i->writeByte (0x81, 0x07);
  cpu->top->ram_i->sp_ram_i->writeByte (0x82, 0x00);
  cpu->top->ram_i->sp_ram_i->writeByte (0x83, 0x04);

  cpu->top->ram_i->sp_ram_i->writeByte (0x84, 0x13);
  cpu->top->ram_i->sp_ram_i->writeByte (0x85, 0x07);
  cpu->top->ram_i->sp_ram_i->writeByte (0x86, 0x60);
  cpu->top->ram_i->sp_ram_i->writeByte (0x87, 0x06);

  cpu->top->ram_i->sp_ram_i->writeByte (0x88, 0x23);
  cpu->top->ram_i->sp_ram_i->writeByte (0x89, 0xa0);
  cpu->top->ram_i->sp_ram_i->writeByte (0x8a, 0xe7);
  cpu->top->ram_i->sp_ram_i->writeByte (0x8b, 0x00);

  cpu->top->ram_i->sp_ram_i->writeByte (0x8c, 0x93);
  cpu->top->ram_i->sp_ram_i->writeByte (0x8d, 0x05);
  cpu->top->ram_i->sp_ram_i->writeByte (0x8e, 0x00);
  cpu->top->ram_i->sp_ram_i->writeByte (0x8f, 0x00);

  cpu->top->ram_i->sp_ram_i->writeByte (0x90, 0x13);
  cpu->top->ram_i->sp_ram_i->writeByte (0x91, 0x06);
  cpu->top->ram_i->sp_ram_i->writeByte (0x92, 0x00);
  cpu->top->ram_i->sp_ram_i->writeByte (0x93, 0x00);

  cpu->top->ram_i->sp_ram_i->writeByte (0x94, 0x93);
  cpu->top->ram_i->sp_ram_i->writeByte (0x95, 0x06);
  cpu->top->ram_i->sp_ram_i->writeByte (0x96, 0x00);
  cpu->top->ram_i->sp_ram_i->writeByte (0x97, 0x00);

  cpu->top->ram_i->sp_ram_i->writeByte (0x98, 0x93);
  cpu->top->ram_i->sp_ram_i->writeByte (0x99, 0x08);
  cpu->top->ram_i->sp_ram_i->writeByte (0x9a, 0xd0);
  cpu->top->ram_i->sp_ram_i->writeByte (0x9b, 0x05);

  cpu->top->ram_i->sp_ram_i->writeByte (0x9c, 0x73);
  cpu->top->ram_i->sp_ram_i->writeByte (0x9d, 0x00);
  cpu->top->ram_i->sp_ram_i->writeByte (0x9e, 0x00);
  cpu->top->ram_i->sp_ram_i->writeByte (0x9f, 0x00);

  // Do some ordinary clocked logic.

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
