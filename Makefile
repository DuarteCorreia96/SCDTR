# Let's start with some usefull definition for colorize the echo 
RED='\e[1;31m'
GREEN='\e[1;32m'
YELLOW='\e[1;33m'
BLUE='\e[1;34m'
WHITE='\e[0m'
VIOLET='\e[1;35m'
CYAN='\e[1;36m'
red='\e[0;31m'
green='\e[0;32m'
yellow='\e[0;33m'
blue='\e[0;34m'
violet='\e[0;35m'
cyan='\e[0;36m'

OK_STRING=$(GREEN)[OK]$(WHITE)
ERROR_STRING=$(RED)[ERRORS]$(WHITE)
WARN_STRING=$(YELLOW)[WARNINGS]$(WHITE)

CC= g++
CFLAGS = -std=c++11 
LDFLAGS = -lpigpio -lpthread -lrt

OBJS = read_from_I2C.o
OBJS_T = rpi_test.o

I2C: $(OBJS)
	@echo -e creating executable... $(GREEN) I2C.exe $(WHITE)
	@g++ -o I2C.exe $^ $(LDFLAGS)

I2C_teste: $(OBJS_T)
	@echo -e creating executable... $(GREEN) I2C_teste.exe $(WHITE)
	@g++ -o I2C_teste.exe $^ $(LDFLAGS)

%.o: %.cpp
	@echo -e compiling $(GREEN) $<  $(WHITE) to $(YELLOW) $@ $(WHITE)
	@g++ $(CFLAGS) -c $<

clean: 
	@echo cleaning...
	rm -f *.o
	rm -f *.exe