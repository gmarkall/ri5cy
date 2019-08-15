// Copyright 2019 Embecosm Ltd.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

////////////////////////////////////////////////////////////////////////////////
// Engineer:       Graham Markall - graham.markall@embecosm.com               //
//                                                                            //
// Design Name:    STR_OPS                                                    //
// Project Name:   RI5CY                                                      //
// Language:       SystemVerilog                                              //
//                                                                            //
// Description:    Simple string ops module for teaching / workshop.          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

import riscv_defines::*;

module riscv_str_ops
(
  input logic                    clk,
  input logic                    enable_i,
  input logic [STR_OP_WIDTH-1:0] operator_i,
  input logic [31:0]             operand_i,

  output logic [31:0]            result_o
);

  always_comb begin
    result_o = enable_i ? 32'h12345678 : 32'b0;
  end

  always_ff @(posedge clk)
  begin
    if (enable_i) begin
      $display("Operand: %x", operand_i);
      if (operator_i == STR_OP_UPPER)
        $display("%t: Upper instruction\n", $time);
      else if (operator_i == STR_OP_LOWER)
        $display("%t: Lower instruction\n", $time);
      else if (operator_i == STR_OP_LEET)
        $display("%t: Leet speak instruction\n", $time);
      else if (operator_i == STR_OP_ROT13)
        $display("%t: Rot13 instruction\n", $time);
    end
    else
      $display("%t: Not enabled\n", $time);
  end

endmodule
