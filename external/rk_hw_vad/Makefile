#common makefile header
PROJECT_DIR := $(shell pwd)
PROM    = rk_hw_vad 
CXXFLAGS ?= -fPIC -O3 -I$(PROJECT_DIR) -lpthread -lasound 
OBJ = rk_hw_vad.o recorder.o 
$(PROM) : $(OBJ)
	$(CXX) -o $(PROM) $(OBJ) $(CXXFLAGS)
%.o: %.cpp %.c
	$(CXX) -c $< -o $@ $(CXXFLAGS)
clean:
	@rm -rf $(OBJ) $(PROM) 
install:
	cp -f $(PROM) $(TARGET_DIR)/usr/bin/
