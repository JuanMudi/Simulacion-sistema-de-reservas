
#include "controladorReserva.h"

struct arguments arguments;

//VARIABLES GLOBALES
int horaActual = 0;


int main(int argc, char **argv) {
    Reserva **reservas = malloc(0);

    int cantReservas = 0;
    init_arguments(argc, argv, &arguments);
    if(arguments.horaInicio == -1 || 
    arguments.horaFinal == -1 || 
    arguments.segundosHora == -1 || 
    arguments.totalPersonas == -1 ||
    arguments.pipeCrecibe == NULL
    )
    {
        printf("Ingrese todos los parametros");
        exit(0);

    }
    horaActual = arguments.horaInicio;

    signal(SIGALRM, manejadorSenal);
    alarm(arguments.segundosHora);

    //Creación del FIFO
     // Crear FIFO y verificar si ya existe.
    if (mkfifo(arguments.pipeCrecibe, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    }

    int fd = open(arguments.pipeCrecibe, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    Reserva reserva;

    while (1) {
        int bytesRead = read(fd, &reserva, sizeof(Reserva));
        if(bytesRead>0){
            if(bytesRead<sizeof(Reserva))
            {
            printf("Conexión con: %s\n",reserva.nombreFamilia);
            }
            else{
            printf("--------------------------------------------------------------\n");
            printf("Reserva:\nFamilia: %s\nHora Inicio:%d\nCantidad personas: %d\n",reserva.nombreFamilia,reserva.horaInicio,reserva.numPersonas);
            printf("--------------------------------------------------------------\n");
            bool esPosible = esPosibleReserva(reservas,&cantReservas,arguments.totalPersonas,reserva,arguments.horaFinal);
            esPosible ? printf("-> Es posible la reserva\n") : printf("-> No es posible la reserva\n");

        }
        }

    }

    close(fd);
    unlink(arguments.pipeCrecibe);
    return 0;
}

void manejadorSenal(int signum) {
        simularTiempo();
        alarm(arguments.segundosHora);
        return;
}

void simularTiempo() {
    if(horaActual >= arguments.horaFinal)
        exit(0);
    horaActual +=1;
    printf("->HORA ACTUAL<-: %d hrs\n",horaActual);
    return;
}
void enviarTiempo(const char* pipe,const char* nombre) {
     if (mkfifo(pipe, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    }
    printf("Enviando hora actual a %s\n",nombre);
    fflush(stdout);  // Asegura que el mensaje se imprima inmediatamente.
    int fd = open(pipe, O_WRONLY);
    write(fd,nombre, sizeof(nombre));
    close(fd);
    

}
bool esPosibleReserva(Reserva **reservas, int *numReservas, int capacidadMax, Reserva nuevaReserva, int horaFinal) {
    if (verificarReserva(*reservas, *numReservas, capacidadMax, nuevaReserva, horaFinal)) {
        agregarReserva(reservas, numReservas, nuevaReserva);
        enviarEstado("RESERVA ACEPTADA",nuevaReserva);
        return true;
    } else {
        // Intentar encontrar un nuevo horario
        int nuevoHorario = buscarNuevoHorario(*reservas, *numReservas, capacidadMax, horaFinal, nuevaReserva.numPersonas);
        if (nuevoHorario != -1) {
            nuevaReserva.horaInicio = nuevoHorario;

            printf("Nuevo Horario: %d", nuevoHorario);
            agregarReserva(reservas, numReservas, nuevaReserva);
            char aux[] = "RESERVA ACEPTADA A LAS ";
            char aux2[10];
            sprintf(aux2,"%d",nuevoHorario);
            strcat(aux,aux2);
            enviarEstado(aux,nuevaReserva);

            return true;
        }
    }

    return false; // No fue posible hacer la reserva
}
bool verificarReserva(Reserva reservas[], int numReservas, int capacidadMax, Reserva nuevaReserva, int horaFinal) {
   if (nuevaReserva.horaInicio < horaActual) {
        return false; // La hora de inicio es menor que la hora actual
    }
   
   int personasPorHora[24] = {0};

    // Llenar el arreglo con la cantidad de personas por cada hora
    for (int i = 0; i < numReservas; i++) {
        for (int hora = reservas[i].horaInicio; hora < reservas[i].horaInicio + 2; hora++) {
            personasPorHora[hora] += reservas[i].numPersonas;
        }
    }

    // Verificar si hay espacio para la nueva reserva
    for (int hora = nuevaReserva.horaInicio; hora < nuevaReserva.horaInicio + 2; hora++) {
        if (hora >= horaFinal || personasPorHora[hora] + nuevaReserva.numPersonas > capacidadMax) {
            return false; // No hay espacio o la hora es fuera del horario
        }
    }

    return true; // Es posible hacer la reserva
}



int buscarNuevoHorario(Reserva reservas[], int numReservas, int capacidadMax, int horaFinal, int numPersonas) {
    int personasPorHora[24] = {0};

    // Llenar el arreglo con la cantidad de personas por cada hora
    for (int i = 0; i < numReservas; i++) {
        for (int hora = reservas[i].horaInicio; hora < reservas[i].horaInicio + 2; hora++) {
            personasPorHora[hora] += reservas[i].numPersonas;
        }
    }

    // Buscar un nuevo horario disponible
    for (int hora = horaActual; hora <= horaFinal - 2; hora++) {
        if (personasPorHora[hora] + numPersonas <= capacidadMax && personasPorHora[hora + 1] + numPersonas <= capacidadMax) {
            return hora; // Nuevo horario encontrado
        }
    }

    return -1; // No se encontró un nuevo horario
}



void agregarReserva(Reserva **reservas, int *numReservas, Reserva nuevaReserva) {
    // Redimensionar el arreglo de reservas para agregar una nueva
    *reservas = realloc(*reservas, (*numReservas + 1) * sizeof(Reserva));
    if (*reservas == NULL) {
        perror("Error reallocating memory");
        exit(EXIT_FAILURE);
    }

    // Agregar la nueva reserva al final del arreglo
    (*reservas)[*numReservas] = nuevaReserva;
    (*numReservas)++;
}
void enviarEstado(char *status, Reserva reserva)
{
    char fifo[] = "/tmp/";
    strcat(fifo,reserva.nombreFamilia);
    printf("Fifo: %s",fifo);

    if (mkfifo(fifo, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    }
    
    printf("Enviando Respuesta\n");
    fflush(stdout);  // Asegura que el mensaje se imprima inmediatamente.
    int fd = open(fifo, O_WRONLY);
    write(fd, status, sizeof(status));
    close(fd);
}



