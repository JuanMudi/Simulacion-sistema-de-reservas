#ifndef CONTROLADORRESERVA_H
#define CONTROLADORRESERVA_H

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>


#include "argumentsController.h"


#define BUFFER_SIZE 200

typedef struct {
    char nombreFamilia[100]; // Nombre de la familia o grupo que realiza la reserva
    int numPersonas;    // Número de personas en el grupo
    int horaInicio;     // Hora de inicio de la reserva
} Reserva;


//FUNCIONES

// Funciones para manejar las solicitudes, el tiempo y la emisión de reportes
void simularTiempo();

// Función para el manejo de señales
void manejadorSenal(int signum);

//Función para envio de tiempo en primera conexión
void enviarTiempo(const char* pipe,const char* nombre);
bool esPosibleReserva(Reserva **reservas, int *numReservas, int capacidadMax, Reserva nuevaReserva, int horaFinal);
int buscarNuevoHorario(Reserva reservas[], int numReservas, int capacidadMax, int horaFinal, int numPersonas);
void agregarReserva(Reserva **reservas, int *numReservas, Reserva nuevaReserva);
bool verificarReserva(Reserva reservas[], int numReservas, int capacidadMax, Reserva nuevaReserva, int horaFinal);
void enviarEstado(char *status,Reserva reserva);
void* manejarSolicitudReserva(void* arg);
void generarReporte();



#endif // CONTROLADORRESERVA_H