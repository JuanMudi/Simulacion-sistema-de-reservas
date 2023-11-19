
#include "controladorReserva.h"

struct arguments arguments;

// VARIABLES GLOBALES
int horaActual = 0;
Reserva **reservas;
int cantReservas = 0;
Reserva *reserva;
int solicitudesNegadas = 0;
int solicitudesAceptadas = 0;
int solicitudesReprogramadas = 0;

int main(int argc, char **argv) {
  reservas = malloc(0);
  reserva = malloc(sizeof(Reserva));

  init_arguments(argc, argv, &arguments);
  if (arguments.horaInicio == -1 || arguments.horaFinal == -1 ||
      arguments.segundosHora == -1 || arguments.totalPersonas == -1 ||
      arguments.pipeCrecibe == NULL) {
    printf("Ingrese todos los parametros");
    exit(0);
  }
  horaActual = arguments.horaInicio;

  signal(SIGALRM, manejadorSenal);
  alarm(arguments.segundosHora);

  // Creación del FIFO
  //  Crear FIFO y verificar si ya existe.
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

  while (1) {
    int bytesRead = read(fd, reserva, sizeof(Reserva));

    if (bytesRead > 0) {
      if (bytesRead < sizeof(Reserva)) {
        printf("Conexión con: %s\n", reserva->nombreFamilia);
        char fifo[128] = "/tmp/";
        strcat(fifo, reserva->nombreFamilia);

        if (mkfifo(fifo, 0666) == -1) {
          if (errno != EEXIST) {
            perror("mkfifo");
            exit(EXIT_FAILURE);
          }
        }
        int fd = open(fifo, O_WRONLY);
        char aux2[10];
        sprintf(aux2, "%d", horaActual);
        write(fd, aux2, 10);
        close(fd);
      } else {
        printf(
            "--------------------------------------------------------------\n");
        printf("Reserva:\nFamilia: %s\nHora Inicio:%d\nCantidad personas: %d\n",
               reserva->nombreFamilia, reserva->horaInicio,
               reserva->numPersonas);
        printf(
            "--------------------------------------------------------------\n");
        pthread_t threadId;
        if (pthread_create(&threadId, NULL, manejarSolicitudReserva, NULL) !=
            0) {
          perror("Failed to create thread");
        }
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
  if (horaActual >= arguments.horaFinal) {
    generarReporte(*reservas);
    exit(0);
  }
  horaActual += 1;
  printf("->HORA ACTUAL<-: %d hrs\n", horaActual);
  return;
}
void enviarTiempo(const char *pipe, const char *nombre) {
  if (mkfifo(pipe, 0666) == -1) {
    if (errno != EEXIST) {
      perror("mkfifo");
      exit(EXIT_FAILURE);
    }
  }
  printf("Enviando hora actual a %s\n", nombre);
  fflush(stdout); // Asegura que el mensaje se imprima inmediatamente.
  int fd = open(pipe, O_WRONLY);
  write(fd, nombre, sizeof(nombre));
  close(fd);
}
bool esPosibleReserva(Reserva **reservas, int *numReservas, int capacidadMax,
                      Reserva nuevaReserva, int horaFinal) {
  if (verificarReserva(*reservas, *numReservas, capacidadMax, nuevaReserva,
                       horaFinal)) {
    agregarReserva(reservas, numReservas, nuevaReserva);
    solicitudesAceptadas++;
    enviarEstado("RESERVA ACEPTADA", nuevaReserva);
    return true;
  } else {
    // Intentar encontrar un nuevo horario
    int nuevoHorario = buscarNuevoHorario(*reservas, *numReservas, capacidadMax,
                                          horaFinal, nuevaReserva.numPersonas);
    if (nuevoHorario != -1) {

      nuevaReserva.horaInicio = nuevoHorario;

      printf("Nuevo Horario: %d", nuevoHorario);
      agregarReserva(reservas, numReservas, nuevaReserva);
      char aux[100] = "RESERVA ACEPTADA A LAS ";
      char aux2[10];
      sprintf(aux2, "%d", nuevoHorario);
      strcat(aux, aux2);
      solicitudesReprogramadas++;
      enviarEstado(aux, nuevaReserva);

      return true;
    }
  }

  solicitudesNegadas++;
  enviarEstado("RESERVA NEGADA", nuevaReserva);
  return false; // No fue posible hacer la reserva
}
bool verificarReserva(Reserva reservas[], int numReservas, int capacidadMax,
                      Reserva nuevaReserva, int horaFinal) {
  if (nuevaReserva.horaInicio < horaActual) {
    return false; // La hora de inicio es menor que la hora actual
  }

  int personasPorHora[24] = {0};

  // Llenar el arreglo con la cantidad de personas por cada hora
  for (int i = 0; i < numReservas; i++) {
    for (int hora = reservas[i].horaInicio; hora < reservas[i].horaInicio + 2;
         hora++) {
      personasPorHora[hora] += reservas[i].numPersonas;
    }
  }

  // Verificar si hay espacio para la nueva reserva
  for (int hora = nuevaReserva.horaInicio; hora < nuevaReserva.horaInicio + 2;
       hora++) {
    if (hora >= horaFinal ||
        personasPorHora[hora] + nuevaReserva.numPersonas > capacidadMax) {
      return false; // No hay espacio o la hora es fuera del horario
    }
  }
  return true; // Es posible hacer la reserva
}

int buscarNuevoHorario(Reserva reservas[], int numReservas, int capacidadMax,
                       int horaFinal, int numPersonas) {
  int personasPorHora[24] = {0};

  // Llenar el arreglo con la cantidad de personas por cada hora
  for (int i = 0; i < numReservas; i++) {
    for (int hora = reservas[i].horaInicio; hora < reservas[i].horaInicio + 2;
         hora++) {
      personasPorHora[hora] += reservas[i].numPersonas;
    }
  }

  // Buscar un nuevo horario disponible
  for (int hora = horaActual; hora <= horaFinal - 2; hora++) {
    if (personasPorHora[hora] + numPersonas <= capacidadMax &&
        personasPorHora[hora + 1] + numPersonas <= capacidadMax) {
      return hora; // Nuevo horario encontrado
    }
  }

  return -1; // No se encontró un nuevo horario
}

void agregarReserva(Reserva **reservas, int *numReservas,
                    Reserva nuevaReserva) {
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
void enviarEstado(char *status, Reserva reserva) {
  char fifo[128] = "/tmp/";
  strcat(fifo, reserva.nombreFamilia);
  printf("\nFifo: %s\n", fifo);
  printf("Fifo: %s\n", fifo);

  if (mkfifo(fifo, 0666) == -1) {
    if (errno != EEXIST) {
      perror("mkfifo");
      exit(EXIT_FAILURE);
    }
  }

  printf("Enviando Respuesta: %s\n", status);
  fflush(stdout); // Asegura que el mensaje se imprima inmediatamente.
  int fd = open(fifo, O_WRONLY);
  write(fd, status, 100);
  close(fd);
}

void *manejarSolicitudReserva(void *arg) {
  printf("\nENTRANDO A HILO\n");

  // Lógica para procesar la reserva
  bool esPosible =
      esPosibleReserva(reservas, &cantReservas, arguments.totalPersonas,
                       *reserva, arguments.horaFinal);
  esPosible ? printf("-> Es posible la reserva\n")
            : printf("-> No es posible la reserva\n");

  // Limpieza y salida del hilo
  printf("\nSALIENDO DEL HILO\n");
  pthread_exit(NULL);
}

void generarReporte(Reserva reservas1[]) {
  printf("--------------------------------------------------------------\n");
  printf("GENERANDO REPORTE\n");
  printf("--------------------------------------------------------------\n");

  int maxPersonas = 0;
  int minPersonas = INT_MAX;
  int numHorasPico = 0;
  int numHorasMenosConcurridas = 0;
  int horasPico[arguments.horaFinal - arguments.horaInicio];
  int horasMenosConcurridas[arguments.horaFinal - arguments.horaInicio];

  int personasPorHora[24] = {0};

  // Encontrar el máximo y mínimo número de personas por hora
  for (int i = 0; i < cantReservas; i++) {
    for (int hora = reservas1[i].horaInicio; hora < reservas1[i].horaInicio + 2;
         hora++) {
      personasPorHora[hora] += reservas1[i].numPersonas;
      printf("Agregando en: %d -> %d\n", hora, reservas1[i].numPersonas);
    }
  }

  // Encuentra el máximo y el mínimo número de personas
  for (int i = arguments.horaInicio; i < arguments.horaFinal; ++i) {

    if (personasPorHora[i] > maxPersonas) {

      maxPersonas = personasPorHora[i];
    }
    if (personasPorHora[i] < minPersonas) {
      minPersonas = personasPorHora[i];
    }
  }

  // Identifica las horas pico y menos concurridas
  for (int i = arguments.horaInicio; i < arguments.horaFinal; ++i) {

    if (personasPorHora[i] == maxPersonas) {

      horasPico[numHorasPico++] = i;
    }
    if (personasPorHora[i] == minPersonas) {

      horasMenosConcurridas[numHorasMenosConcurridas++] = i;
    }
  }
  

  // Imprimir el reporte
  printf("Reporte de Ocupación del Parque:\n");
if(cantReservas==0)
  {
    printf("NO SE REGISTRARON RESERVAS EL DIA DE HOY\n");
    exit(0);
  }

  printf("Horas Pico: ");
  for (int i = 0; i < numHorasPico; i++) {
    printf("%d ", horasPico[i]);
  }
  printf("\n");

  printf("Horas con Menor Cantidad de Personas: ");
  for (int i = 0; i < numHorasMenosConcurridas; i++) {
    printf("%d ", horasMenosConcurridas[i]);
  }
  printf("\n");

  printf("Número de Solicitudes Negadas: %d\n", solicitudesNegadas);
  printf("Número de Solicitudes Aceptadas: %d\n", solicitudesAceptadas);
  printf("Número de Solicitudes Reprogramadas: %d\n", solicitudesReprogramadas);
}
