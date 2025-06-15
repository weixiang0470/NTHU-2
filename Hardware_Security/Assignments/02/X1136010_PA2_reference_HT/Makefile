SHELL := /bin/csh
.PHONY: view clean setup

# Default file list
file_list = *.v

# Compile and simulate command
NCVERILOG = ncverilog -sv +access+r +define+FSDB $(file_list)

# Source commands
SETUP_CMD = source /home/tools/synopsys/CIC/verdi.cshrc; \
			source /home/tools/cadence/CIC/incisiv.cshrc; \
			source /home/tools/synopsys/CIC/customexplorer.cshrc; \
			source /home/tools/synopsys/CIC/verdi-ncverilog.cshrc

# View waveform
view:
	@tcsh -c 'if ("$(file)" == "") then \
		echo "Usage: make view file=<fsdb_file>"; \
	else if (! -e $(file)) then \
        echo "Error: $(file) not found"; \
	else \
		$(SETUP_CMD); wv $(file) &; \
	endif'

# Start Clean
clean:
	rm -rf $(shell find . ! -name '*.v' ! -name 'Makefile')

# Simulate specific testbench
sim:
	@tcsh -c 'if ("$(file)" == "") then \
        echo "Usage: make sim file=<testbench_file>"; \
    else if (! -e $(file)) then \
        echo "Error: $(file) not found"; \
    else \
        $(SETUP_CMD); \
        $(eval file_list = $(shell find . -name '*.v' ! -name '*_tb.v'; echo $(file))); \
        $(NCVERILOG); \
    endif'

# Help
help:
	@echo "Available targets:"
	@echo "  view          - View waveform (usage: make view file=<fsdb_file>)"
	@echo "  sim      	   - Simulate specific testbench (usage: make sim file=<testbench_file>)"
	@echo "  clean         - Clean all generated files"
	@echo "  help          - Show this help message"