#ifndef AGENTERESERVA_H
#define AGENTERESERVA_H

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "argumentsAgent.h"

typedef struct {
    char nombreFamilia[100]; // Nombre de la familia o grupo que realiza la reserva
    int numPersonas;    // Número de personas en el grupo
    int horaInicio;     // Hora de inicio de la reserva
} Reserva;

struct conexion{
    char nombre[50];
    char pipe[100];
};

void conectar();



#endif // AGENTERESERVA_H