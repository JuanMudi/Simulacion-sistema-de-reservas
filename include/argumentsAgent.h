#ifndef ARGUMENTSCONTROLLER_H
#define ARGUMENTSCONTROLLER_H

// Incluir librerías estándar de C si son necesarias
#include <argp.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct arguments {
  char *nombre;
  char *archivo;
  char *pipeCrecibe;
};

static struct argp_option options[] = {
    {"nombre", 's', "STRING", 0, "Nombre del agente"},
    {"archivo", 'a', "STRING", 0,
     "Archivo de entrada para solicitudes del agente"},
    {"pipeCrecibe", 'p', "STRING", 0, "Pipe nominal para recibir datos"},
    {0}};

error_t parse_agent_opt(int key, char *arg, struct argp_state *state);

static struct argp argp = {options, parse_agent_opt, 0, 0};

void init_agent_arguments(int argc, char *argv[], struct arguments *arguments);

#endif // ARGUMENTSCONTROLLER_H
