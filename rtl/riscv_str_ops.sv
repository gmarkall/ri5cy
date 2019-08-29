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
  input logic                    rst_n,
  input logic                    enable_i,
  input logic [STR_OP_WIDTH-1:0] operator_i,
  input logic [31:0]             operand_i,

  output logic [31:0]            result_o,

  output logic [31:0]            ready_o,
  input logic                    ex_ready_i

);

  logic [31:0] result;
  logic [31:0] leet_intermediate;
  logic [7:0] char;

  enum logic [2:0] {IDLE, STEP0, STEP1, STEP2, FINISH} leet_CS, leet_NS;

  logic leet_active;
  logic leet_ready;

  always_comb begin
    leet_ready = 1'b0;
    leet_NS = leet_CS;
    leet_active = 1'b1;

    case (leet_CS)
      IDLE: begin
        leet_active = 1'b0;
        leet_ready = 1'b1;
        if (operator_i == STR_OP_LEET && enable_i) begin
          leet_ready = 1'b0;
          leet_NS = STEP0;
        end
      end
      STEP0: begin
        leet_NS = STEP1;
      end
      STEP1: begin
        leet_NS = STEP2;
      end
      STEP2: begin
        leet_NS = FINISH;
      end
      FINISH: begin
        leet_ready = 1'b1;
        if (ex_ready_i)
          leet_NS = IDLE;
      end
    endcase
  end

  always_ff @(posedge clk, negedge rst_n)
  begin
    if (~rst_n)
      leet_CS <= IDLE;
    else
      leet_CS <= leet_NS;

    case (leet_CS)
      IDLE: begin
        if (operator_i == STR_OP_LEET && enable_i)
          leet_intermediate <= operand_i;
      end
      STEP0: begin
        // Turn "e" and "E" into "3"
        char = leet_intermediate[7:0];
        if ((char == 69 || char == 101))
          char = 51;
        leet_intermediate[7:0] <= char;

        char = leet_intermediate[15:8];
        if ((char == 69 || char == 101))
          char = 51;
        leet_intermediate[15:8] <= char;

        char = leet_intermediate[23:16];
        if ((char == 69 || char == 101))
          char = 51;
        leet_intermediate[23:16] <= char;

        char = leet_intermediate[31:24];
        if ((char == 69 || char == 101))
          char = 51;
        leet_intermediate[31:24] <= char;
      end
      STEP1: begin
        // Turn "s" and "S" into "5"
        char = leet_intermediate[7:0];
        if ((char == 83 || char == 115))
          char = 53;
        leet_intermediate[7:0] <= char;

        char = leet_intermediate[15:8];
        if ((char == 83 || char == 115))
          char = 53;
        leet_intermediate[15:8] <= char;

        char = leet_intermediate[23:16];
        if ((char == 83 || char == 115))
          char = 53;
        leet_intermediate[23:16] <= char;

        char = leet_intermediate[31:24];
        if ((char == 83 || char == 115))
          char = 53;
        leet_intermediate[31:24] <= char;
      end
      STEP2: begin
        // Turn "l" and "L" into "1"
        char = leet_intermediate[7:0];
        if ((char == 76 || char == 108))
          char = 49;
        leet_intermediate[7:0] <= char;

        char = leet_intermediate[15:8];
        if ((char == 76 || char == 108))
          char = 49;
        leet_intermediate[15:8] <= char;

        char = leet_intermediate[23:16];
        if ((char == 76 || char == 108))
          char = 49;
        leet_intermediate[23:16] <= char;

        char = leet_intermediate[31:24];
        if ((char == 76 || char == 108))
          char = 49;
        leet_intermediate[31:24] <= char;

      end
    endcase
  end

  always_comb begin
    result = 32'b0;

    if (operator_i == STR_OP_UPPER) begin
      char = operand_i[7:0];
      if ((char > 96) && (char < 123))
        char = char - 32;
      result[7:0] = char;

      char = operand_i[15:8];
      if ((char > 96) && (char < 123))
        char = char - 32;
      result[15:8] = char;

      char = operand_i[23:16];
      if ((char > 96) && (char < 123))
        char = char - 32;
      result[23:16] = char;

      char = operand_i[31:24];
      if ((char > 96) && (char < 123))
        char = char - 32;
      result[31:24] = char;
    end

    // Mux results
    if (enable_i) begin
      // Dummy value for unimplemented instructions
      result_o = 32'hDEADBEEF;

      if (operator_i == STR_OP_UPPER)
        result_o = result;
      if (operator_i == STR_OP_LEET && leet_CS == FINISH)
        result_o = leet_intermediate;
    end
  end

  assign ready_o = leet_ready;

  always_ff @(posedge clk)
  begin
    if (enable_i) begin
      case (operator_i)
        STR_OP_UPPER:
          $display("%t: Exec Upper instruction", $time);
        STR_OP_LOWER:
          $display("%t: Exec Lower instruction", $time);
        STR_OP_LEET:
          $display("%t: Exec Leet speak instruction", $time);
        STR_OP_ROT13:
          $display("%t: Exec Rot13 instruction", $time);
      endcase
    end
  end

endmodule
