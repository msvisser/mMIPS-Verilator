all: run

run: clean_exe
	cd obj_dir && make -j -f Vmmips.mk Vmmips && ./Vmmips

trace: clean_exe
	cd obj_dir && CPPFLAGS=-DTRACE make -j -f Vmmips.mk Vmmips && ./Vmmips

clean_exe:
	rm -f obj_dir/Vmmips obj_dir/mmips_sim.o

verilator:
	rm -rf obj_dir
	verilator -Wall \
		--exe mmips_sim.cpp --trace -Iverilog --cc mmips.v -v \
		add.v alu.v aluctrl.v branch_ctrl.v ctrl.v decoder.v \
		hazard.v hazard_ctrl.v imm2word.v memdev.v mmips_defines.v \
		mux.v regfile16.v register.v shift.v signextend.v
