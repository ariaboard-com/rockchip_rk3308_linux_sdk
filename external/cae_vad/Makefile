#common makefile header
PROJECT_DIR := $(shell pwd)
PROM    = cae_vad
CXXFLAGS ?= -fPIC -O3 -I$(PROJECT_DIR) -lpthread -lasound -ldl
OBJ = InfoLed.o  main.o cae_lib.o
$(PROM): $(OBJ)
	$(CXX) -o $(PROM) $(OBJ) $(CXXFLAGS)
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)
clean:
	@rm -rf $(OBJ) $(PROM)
install:
	sudo install -D -m 755 cae_vad -t /usr/bin/
