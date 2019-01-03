RED='\e[1;31m'
GREEN='\e[1;32m'
YELLOW='\e[1;33m'
WHITE='\e[0m'

OK_STRING=$(GREEN)[OK]$(WHITE)
ERROR_STRING=$(RED)[ERRORS]$(WHITE)
WARN_STRING=$(YELLOW)[WARNINGS]$(WHITE)

CC= g++
CFLAGS = -std=c++11 -lpthread -lboost_system
LDFLAGS_SERVER = -lpigpio -lrt -lpthread -lboost_system
LDFLAGS_CLIENT = -lrt -lpthread -lboost_system

OBJS_SERVER = data_storage.o server.o rpi_server.o
OBJS_CLIENT = client.o

server: $(OBJS_SERVER)
	@echo -e creating executable... $(GREEN) server.exe $(WHITE)
	@g++ -o server.exe $^ $(LDFLAGS_SERVER)

client: $(OBJS_CLIENT)
	@echo -e creating executable... $(GREEN) client.exe $(WHITE)
	@g++ -o client.exe $^ $(LDFLAGS_CLIENT)

%.o: %.cpp
	@echo -e compiling $(GREEN) $<  $(WHITE) to $(YELLOW) $@ $(WHITE)
	@g++ $(CFLAGS) -c $<

clean: 
	@echo cleaning...
	rm -f *.o
	rm -f *.exe