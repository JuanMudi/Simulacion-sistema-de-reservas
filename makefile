GCC=gcc
CFLAGS= -I ./include -lpthread ./include/*.c

PROGS = controller agent

all: $(PROGS)

controller:
	$(GCC) $(CFLAGS) -o controller ./src/controladorReserva.c

agent:
	$(GCC) $(CFLAGS) -o agent ./src/agenteReserva.c

clean:
	rm -f controller agent