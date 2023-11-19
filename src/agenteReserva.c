
#include "agenteReserva.h"

struct arguments arguments;
int horaActual;

int main(int argc, char **argv) {
  init_agent_arguments(argc, argv, &arguments);

  if (arguments.nombre == NULL || arguments.archivo == NULL ||
      arguments.pipeCrecibe == NULL) {
    printf("Ingrese todos los parametros");
    exit(0);
  }

  conectar();

  const char *filename = arguments.archivo; // Nombre del archivo
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  // Contar el número de líneas (reservas) en el archivo
  int count = 0;
  char ch;
  while (!feof(file)) {
    ch = fgetc(file);
    if (ch == '\n') {
      count++;
    }
  }
  rewind(file);

  // Asignar memoria para las reservas
  Reserva *reservas = malloc(count * sizeof(Reserva));
  if (reservas == NULL) {
    perror("Error allocating memory");
    fclose(file);
    return 1;
  }

  // Leer cada línea y almacenarla en la estructura
  for (int i = 0; i < count; i++) {
    if (fscanf(file, "%99[^,],%d,%d\n", reservas[i].nombreFamilia,
               &reservas[i].numPersonas, &reservas[i].horaInicio) != 3) {
      fprintf(stderr, "Error reading file at line %d\n", i + 1);
      free(reservas);
      fclose(file);
      return 1;
    }
  }

  fclose(file);

  // Enviar las reservas
  for (int i = 0; i < count; i++) {
    if (reservas[i].horaInicio < horaActual)
      continue;
    printf("Reserva %d: %s, %d, %d\n", i + 1, reservas[i].nombreFamilia,
           reservas[i].numPersonas, reservas[i].horaInicio);
    fflush(stdout); // Asegura que el mensaje se imprima inmediatamente.
    int fd = open(arguments.pipeCrecibe, O_WRONLY);
    write(fd, &reservas[i], sizeof(Reserva));
    close(fd);

    char fifo[128] = "/tmp/";
    strcat(fifo, reservas[i].nombreFamilia);
    printf("Fifo: %s\n", fifo);

    if (mkfifo(fifo, 0666) == -1) {
      if (errno != EEXIST) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
      }
    };

    int fd2 = open(fifo, O_RDONLY);
    if (fd2 == -1) {
      perror("open");
      exit(EXIT_FAILURE);
    }

    char aux[100];
    int bytesRead = read(fd2, aux, sizeof(aux));
    if (bytesRead > 0) {
      printf("Respuesta: %s\n", aux);
    }
    close(fd2);
  }

  // Liberar la memoria asignada
  free(reservas);

  return 0;
}

void conectar() {
  printf("Realizando conexión\n");
  fflush(stdout); // Asegura que el mensaje se imprima inmediatamente.
  int fd = open(arguments.pipeCrecibe, O_WRONLY);
  write(fd, arguments.nombre, sizeof(arguments.nombre));
  close(fd);

  char fifo[128] = "/tmp/";
  ;
  strcat(fifo, arguments.nombre);

  if (mkfifo(fifo, 0666) == -1) {
    if (errno != EEXIST) {
      perror("mkfifo");
      exit(EXIT_FAILURE);
    }
  }

  int fd2 = open(fifo, O_RDONLY);
  if (fd2 == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  };

  char aux[10];
  int bytesRead = read(fd2, aux, sizeof(aux));
  if (bytesRead > 0) {
    printf("Hora Actual: %s\n", aux);
  }
  close(fd2);
  horaActual = atoi(aux);
}
