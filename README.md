# mMIPS Verilator
This is a Verilator testbench for the Computation II mMIPS processor.
Using Verilator the simulation can be run in just a few seconds, instead of the multiple minutes it takes when running in ISim. Even with all traces on the the simulation will run in less than a minute.

To run this testbench the following files need to be supplied:

- `mips_mem.bin`, the compiled software binary
- `verilog/*.v`, all verilog files supplied with the mMIPS processor, except for `rom.v`, `ram.v`, `testbench.v` and `mMIPS_sim.v`

## Running
To run the testbench the supplied Makefile can be used. When you are using this testbench for the first time or when the Verilog has changed you are required to run `make verilator` to make sure the verilated output is up-to-date.

For simply running the simulation you can use `make run` or just `make`. If you want to trace the mMIPS signals you can use `make trace` (**WARNING!**: the trace output will be very large, ~300MB with 250k clock cycles).