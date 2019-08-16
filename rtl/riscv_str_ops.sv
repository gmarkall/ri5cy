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

  logic [31:0] result;

  always_comb begin
    if (operator_i == STR_OP_UPPER) begin
      if ((operand_i[7:0] > 96) && (operand_i[7:0] < 123))
        result[7:0] = operand_i[7:0] - 32;
      else
        result[7:0] = 8'b0;

      if ((operand_i[15:8] > 96) && (operand_i[15:8] < 123))
        result[15:8] = operand_i[15:8] - 32;
      else
        result[15:8] = 8'b0;

      if ((operand_i[23:16] > 96) && (operand_i[23:16] < 123))
        result[23:16] = operand_i[23:16] - 32;
      else
        result[23:16] = 8'b0;

      if ((operand_i[31:24] > 96) && (operand_i[31:24] < 123))
        result[31:24] = operand_i[31:24] - 32;
      else
        result[31:24] = 8'b0;

    end
    result_o = enable_i ? result : 32'b0;
  end

  always_ff @(posedge clk)
  begin
    if (enable_i) begin
      case (operator_i)
        STR_OP_UPPER:
          $display("%t: Upper instruction, op %x\n", $time, operand_i);
        STR_OP_LOWER:
          $display("%t: Lower instruction, op %x\n", $time, operand_i);
        STR_OP_LEET:
          $display("%t: Leet speak instruction, op %x\n", $time, operand_i);
        STR_OP_ROT13:
          $display("%t: Rot13 instruction, op %x\n", $time, operand_i);
      endcase
    end
    else
      $display("%t: Not enabled\n", $time);
  end

endmodule
