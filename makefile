GCC := gcc
CFLAGS := -I./include
LDFLAGS := -lpthread

PROGS := controller agent

all: $(PROGS)

controller: src/controladorReserva.c
	$(GCC) $(CFLAGS) -g -o controller src/controladorReserva.c src/argumentsController.c $(LDFLAGS)

agent: src/agenteReserva.c
	$(GCC) $(CFLAGS) -g -o agent src/agenteReserva.c src/argumentsAgent.c $(LDFLAGS)

clean:
	rm -f $(PROGS)
