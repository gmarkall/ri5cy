// RAM wrapper for RI5CY

// Copyright (C) 2017 Embecosm <www.embecosm.com>

// Contributor: Jeremy Bennett <jeremy.bennett@embecosm.com>

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


// This muxes the sp_ram module of pulpino to the instruction and data ports of
// the RI5CY processor core.

module ram
    #(
    parameter ADDR_WIDTH = 22,
    parameter DATA_WIDTH = 32
  )(
    // Clock
    input logic 		  clk,
    input logic 		  rstn,

    input logic 		  instr_req_i,
    input logic [ADDR_WIDTH-1:0]  instr_addr_i,
    output logic [DATA_WIDTH-1:0] instr_rdata_o,
    output logic 		  instr_rvalid_o,
    output logic 		  instr_gnt_o,

    input logic 		  data_req_i,
    input logic [ADDR_WIDTH-1:0]  data_addr_i,
    input logic 		  data_we_i,
    input logic [3:0] 		  data_be_i,
    input logic [DATA_WIDTH-1:0]  data_wdata_i,
    output logic [DATA_WIDTH-1:0] data_rdata_o,
    output logic 		  data_rvalid_o,
    output logic 		  data_gnt_o
  );

   // Signals to RAM

   logic 	             ram_en;
   logic [ADDR_WIDTH-1:0]    ram_addr;
   logic [DATA_WIDTH-1:0]    ram_wdata;
   logic [DATA_WIDTH-1:0]    ram_rdata;
   logic 		     ram_we;
   logic [DATA_WIDTH/8-1:0]  ram_be;


   // Instantiate RAM

   sp_ram
     #(
       .ADDR_WIDTH (ADDR_WIDTH)
       )
   sp_ram_i
     (
      .clk     ( clk       ),

      .en_i    ( ram_en    ),
      .addr_i  ( ram_addr  ),
      .wdata_i ( ram_wdata ),
      .rdata_o ( ram_rdata ),
      .we_i    ( ram_we    ),
      .be_i    ( ram_be    )
      );

   // Mux the data and instruction accesses

   ram_mux
     #(
      .ADDR_WIDTH (ADDR_WIDTH),
      .OUT_WIDTH (ADDR_WIDTH),
      .IN0_WIDTH (ADDR_WIDTH),
      .IN1_WIDTH (ADDR_WIDTH)
      )
   ram_mux_i
     (
      .clk            ( clk            ),
      .rst_n          ( rstn           ),

      // Port 0 has priority
      .port0_req_i    ( instr_req_i    ),
      .port0_gnt_o    ( instr_gnt_o    ),
      .port0_rvalid_o ( instr_rvalid_o ),
      .port0_addr_i   ( instr_addr_i   ),
      .port0_we_i     ( 'b0            ),
      .port0_be_i     ( 'b1111         ),
      .port0_rdata_o  ( instr_rdata_o  ),
      .port0_wdata_i  ( 'b0            ),

      .port1_req_i    ( data_req_i     ),
      .port1_gnt_o    ( data_gnt_o     ),
      .port1_rvalid_o ( data_rvalid_o  ),
      .port1_addr_i   ( data_addr_i    ),
      .port1_we_i     ( data_we_i      ),
      .port1_be_i     ( data_be_i      ),
      .port1_rdata_o  ( data_rdata_o   ),
      .port1_wdata_i  ( data_wdata_i   ),

      .ram_en_o       ( ram_en         ),
      .ram_addr_o     ( ram_addr       ),
      .ram_we_o       ( ram_we         ),
      .ram_be_o       ( ram_be         ),
      .ram_rdata_i    ( ram_rdata      ),
      .ram_wdata_o    ( ram_wdata      )
      );
/* -----\/----- EXCLUDED -----\/-----

   assign data_gnt_o  = data_req_i;
   assign instr_gnt_o = instr_req_i;

   always_ff @(posedge clk)
     begin
	data_rvalid_o  <= data_req_i;
	instr_rvalid_o <= instr_req_i;
     end
 -----/\----- EXCLUDED -----/\----- */

endmodule	// ram
