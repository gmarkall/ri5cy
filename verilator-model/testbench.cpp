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

#include <iostream>
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "Vtop.h"
#include "Vtop_top.h"
#include "Vtop_ram__A10.h"
#include "Vtop_dp_ram__A10.h"


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
  cpu->debug_halt_i   = 0;
  cpu->debug_resume_i = 0;
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

  // Read from memory

  // Word read first
  for (size_t i = 0; i < 4; i++)
    {
      uint32_t res = cpu->top->ram_i->dp_ram_i->readWord (i);
      std::cout << std::hex << res << " ";
    }

  std::cout << std::endl;

  // Try byte read
  for (size_t i = 0; i < 16; i++)
    {
      uint32_t res = cpu->top->ram_i->dp_ram_i->readByte (i);
      std::cout << std::hex << res << " ";
    }

  // Write a valid instruction to memory.
  cpu->top->ram_i->dp_ram_i->writeWord (0x20, 0x00000593);
  cpu->top->ram_i->dp_ram_i->writeWord (0x80, 0x00000593);

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
