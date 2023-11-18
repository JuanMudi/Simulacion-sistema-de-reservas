#ifndef ARGUMENTSCONTROLLER_H
#define ARGUMENTSCONTROLLER_H

// Incluir librerías estándar de C si son necesarias
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>

struct arguments {
    int horaInicio;
    int horaFinal;
    int segundosHora;
    int totalPersonas;
    char *pipeCrecibe;
};


static struct argp_option options[] = {
    {"horaInicio", 'i', "INT", 0, "Hora inicial de la simulación"},
    {"horaFinal", 'f', "INT", 0, "Hora final de la simulación"},
    {"segundosHora", 's', "INT", 0, "Segundos para simular una hora"},
    {"totalPersonas", 't', "INT", 0, "Cantidad máxima de personas por hora"},
    {"pipeCrecibe", 'p', "STRING", 0, "Pipe nominal para recibir datos"},
    {0}
};



error_t parse_opt(int key, char *arg, struct argp_state *state);

static struct argp argp = {options, parse_opt, 0, 0};

void init_arguments(int argc, char *argv[], struct arguments *arguments);




#endif // ARGUMENTSCONTROLLER_H
