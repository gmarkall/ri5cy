Running unit tests with CTest
=============================

This is a transitional bodge. First run:

```
make -f Makefile.gnu
```

to build the model and testbench. Then run

```
CMake .
make
```

to build the tests.

Then run the tests with:

```
make test
```

Expected output should be similar to:

```
$ make test
Running tests...
Test project /home/graham/projects/sam-leonard/RI5CY-Xcrypto/verilator-model
      Start  1: 00-mvcop
 1/43 Test  #1: 00-mvcop .........................   Passed    0.01 sec
      Start  2: 01-cmov
 2/43 Test  #2: 01-cmov ..........................   Passed    0.01 sec
      Start  3: 02-li
 3/43 Test  #3: 02-li ............................   Passed    0.01 sec
      Start  4: 03-bop
 4/43 Test  #4: 03-bop ...........................   Passed    0.01 sec
      Start  5: 04-ins-ext
 5/43 Test  #5: 04-ins-ext .......................   Passed    0.01 sec
      Start  6: 05-packed-arith-add
 6/43 Test  #6: 05-packed-arith-add ..............   Passed    0.01 sec
      Start  7: 05-packed-arith-mul
 7/43 Test  #7: 05-packed-arith-mul ..............   Passed    0.01 sec
      Start  8: 05-packed-arith-sub
 8/43 Test  #8: 05-packed-arith-sub ..............   Passed    0.01 sec
      Start  9: 06-packed-shift
 9/43 Test  #9: 06-packed-shift ..................   Passed    0.01 sec
      Start 10: 07-packed-shifti
10/43 Test #10: 07-packed-shifti .................   Passed    0.01 sec
      Start 11: 08-load
11/43 Test #11: 08-load ..........................   Passed    0.01 sec
      Start 12: 10-load-h
12/43 Test #12: 10-load-h ........................   Passed    0.01 sec
      Start 13: 11-load-b
13/43 Test #13: 11-load-b ........................   Passed    0.01 sec
      Start 14: 12-store-w
14/43 Test #14: 12-store-w .......................   Passed    0.01 sec
      Start 15: 13-store-h
15/43 Test #15: 13-store-h .......................   Passed    0.01 sec
      Start 16: 14-store-b
16/43 Test #16: 14-store-b .......................   Passed    0.01 sec
      Start 17: 15-gather-b
17/43 Test #17: 15-gather-b ......................   Passed    0.01 sec
      Start 18: 16-scatter-b
18/43 Test #18: 16-scatter-b .....................   Passed    0.01 sec
      Start 19: 17-gather-h
19/43 Test #19: 17-gather-h ......................   Passed    0.01 sec
      Start 20: 18-scatter-h
20/43 Test #20: 18-scatter-h .....................   Passed    0.01 sec
      Start 21: 19-random
21/43 Test #21: 19-random ........................   Passed    0.01 sec
      Start 22: 20-add2-mp
22/43 Test #22: 20-add2-mp .......................   Passed    0.01 sec
      Start 23: 21-add3-mp
23/43 Test #23: 21-add3-mp .......................   Passed    0.01 sec
      Start 24: 22-slli-mp
24/43 Test #24: 22-slli-mp .......................   Passed    0.01 sec
      Start 25: 23-sll-mp
25/43 Test #25: 23-sll-mp ........................   Passed    0.01 sec
      Start 26: 24-srl-mp
26/43 Test #26: 24-srl-mp ........................   Passed    0.01 sec
      Start 27: 25-srli-mp
27/43 Test #27: 25-srli-mp .......................   Passed    0.01 sec
      Start 28: 26-acc2-mp
28/43 Test #28: 26-acc2-mp .......................   Passed    0.01 sec
      Start 29: 27-acc1-mp
29/43 Test #29: 27-acc1-mp .......................   Passed    0.01 sec
      Start 30: 28-sub2-mp
30/43 Test #30: 28-sub2-mp .......................   Passed    0.01 sec
      Start 31: 29-sub3-mp
31/43 Test #31: 29-sub3-mp .......................   Passed    0.01 sec
      Start 32: 30-mac-mp
32/43 Test #32: 30-mac-mp ........................   Passed    0.01 sec
      Start 33: 31-equ-mp
33/43 Test #33: 31-equ-mp ........................   Passed    0.01 sec
      Start 34: 32-ltu-mp
34/43 Test #34: 32-ltu-mp ........................   Passed    0.01 sec
      Start 35: 33-gtu-mp
35/43 Test #35: 33-gtu-mp ........................   Passed    0.01 sec
      Start 36: 34-packed-arith-mul-h
36/43 Test #36: 34-packed-arith-mul-h ............   Passed    0.01 sec
      Start 37: 35-packed-arith-clmul-h
37/43 Test #37: 35-packed-arith-clmul-h ..........   Passed    0.01 sec
      Start 38: 36-packed-arith-clmul-l
38/43 Test #38: 36-packed-arith-clmul-l ..........   Passed    0.01 sec
      Start 39: 37-mclmul
39/43 Test #39: 37-mclmul ........................   Passed    0.01 sec
      Start 40: 38-aes-sub
40/43 Test #40: 38-aes-sub .......................   Passed    0.01 sec
      Start 41: 39-aes-mix
41/43 Test #41: 39-aes-mix .......................   Passed    0.01 sec
      Start 42: 40-sbox
42/43 Test #42: 40-sbox ..........................   Passed    0.01 sec
      Start 43: 41-sha3
43/43 Test #43: 41-sha3 ..........................   Passed    0.01 sec

100% tests passed, 0 tests failed out of 43

Total Test time (real) =   0.39 sec
```

The output from the testbench can be seen in `Testing/Temporary/LastTest.log`,
e.g.:

```
Start testing: Aug 22 00:45 BST
----------------------------------------------------------
1/43 Testing: 00-mvcop
1/43 Test: 00-mvcop
Command: "/home/graham/projects/sam-leonard/RI5CY-Xcrypto/verilator-model/testbench" "00-mvcop.bin"
Directory: /home/graham/projects/sam-leonard/RI5CY-Xcrypto/verilator-model/unit
"00-mvcop" start time: Aug 22 00:45 BST
Output:
----------------------------------------------------------
About to halt and set traps on exceptions
About to resume
Cycling clock to run for a few instructions
                  56: Illegal instruction (core 0) at PC 0x000000a8:
Halting
<end of output>
Test time =   0.01 sec
----------------------------------------------------------
Test Passed.
"00-mvcop" end time: Aug 22 00:45 BST
"00-mvcop" time elapsed: 00:00:00
----------------------------------------------------------

2/43 Testing: 01-cmov
2/43 Test: 01-cmov
Command: "/home/graham/projects/sam-leonard/RI5CY-Xcrypto/verilator-model/testbench" "01-cmov.bin"
Directory: /home/graham/projects/sam-leonard/RI5CY-Xcrypto/verilator-model/unit
"01-cmov" start time: Aug 22 00:45 BST
Output:
----------------------------------------------------------
About to halt and set traps on exceptions
About to resume
Cycling clock to run for a few instructions
                  60: Illegal instruction (core 0) at PC 0x000000b0:
Halting
<end of output>
Test time =   0.01 sec
----------------------------------------------------------
Test Passed.
"01-cmov" end time: Aug 22 00:45 BST
"01-cmov" time elapsed: 00:00:00
----------------------------------------------------------

3/43 Testing: 02-li
3/43 Test: 02-li

...
```
