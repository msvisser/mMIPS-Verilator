VERILATOR-MAIN=mmips.v
VERILATOR-INCLUDE=add.v alu.v aluctrl.v branch_ctrl.v ctrl.v decoder.v hazard.v hazard_ctrl.v imm2word.v memdev.v mmips_defines.v mux.v regfile16.v register.v shift.v signextend.v
VERILATOR-SIM=mmips_sim.cpp
VERILATOR-OUTPUT=obj_dir/Vmmips.cpp

all: run

run: $(VERILATOR-OUTPUT) clean-exe
	cd obj_dir && make -j -f Vmmips.mk Vmmips && ./Vmmips

trace: $(VERILATOR-OUTPUT) clean-exe
	cd obj_dir && CPPFLAGS=-DTRACE make -j -f Vmmips.mk Vmmips && ./Vmmips

clean-exe:
	rm -f obj_dir/Vmmips obj_dir/mmips_sim.o

verilator: $(VERILATOR-OUTPUT)

$(VERILATOR-OUTPUT): $(addprefix verilog/,$(VERILATOR-MAIN)) $(addprefix verilog/,$(VERILATOR-INCLUDE))
	rm -rf obj_dir
	verilator -Wall -Wno-fatal -Iverilog --exe $(VERILATOR-SIM) --trace --cc $(VERILATOR-MAIN) -v $(VERILATOR-INCLUDE)

clean:
	rm -rf obj_dir