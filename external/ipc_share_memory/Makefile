#common makefile header
PROJECT_DIR := $(shell pwd)
PROM    = ipc_client
PROM_SERVICE    = ipc_service
CXXFLAGS ?= -fPIC -O3 -I$(PROJECT_DIR) -lpthread 
OBJ = ipc_client.o 
OBJ_SERVICE=ipc_service.o service_ipc.o 
$(PROM) $(PROM_SERVICE): $(OBJ) $(OBJ_SERVICE)
	$(CXX) -o $(PROM) $(OBJ) $(CXXFLAGS)
	$(CXX) -o $(PROM_SERVICE) $(OBJ_SERVICE) $(CXXFLAGS)
%.o: %.cpp %.c
	$(CXX) -c $< -o $@ $(CXXFLAGS)
clean:
	@rm -rf $(OBJ) $(OBJ_SERVICE) $(PROM) $(PROM_SERVICE)
install:
	cp -f $(PROM) $(TARGET_DIR)/usr/bin/
	cp -f $(PROM_SERVICE) $(TARGET_DIR)/usr/bin/
