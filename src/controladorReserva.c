
#include "controladorReserva.h"

struct arguments arguments;

//VARIABLES GLOBALES
int horaActual = 0;
int capacidadActual;

int main(int argc, char **argv) {
    Reserva reservas[100];
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

    char buf[BUFFER_SIZE];

    while (horaActual < arguments.horaFinal) {
        int bytesRead = read(fd, buf, BUFFER_SIZE);
        if (bytesRead > 0) {
            printf("Se ha recibido conexión de: %s\n",buf);
        }

    }

    close(fd);
    unlink(arguments.pipeCrecibe);
    return 0;
}

void manejadorSenal(int signum) {
        simularTiempo();
        alarm(arguments.segundosHora);
}

void simularTiempo() {
    horaActual +=1;
    printf("Hora actual: %d hrs\n",horaActual);
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


