
#include "argumentsController.h"

error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 'i':
            arguments->horaInicio = atoi(arg);
            if(arguments->horaInicio < 7)
            {
                argp_failure(state, 1, 0, "Hora de inicio invalida. Ingrese una hora mayor a 7");
            }
            break;
        case 'f':
            arguments->horaFinal = atoi(arg);
            if(arguments->horaFinal > 19)
            {
                argp_failure(state, 1, 0, "Hora de fin invalida. Ingrese una hora menor a 19");
            }            
            break;
        case 's':
            arguments->segundosHora = atoi(arg);
            break;
        case 't':
            arguments->totalPersonas = atoi(arg);
            break;
        case 'p':
            arguments->pipeCrecibe = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

void init_arguments(int argc, char *argv[], struct arguments *arguments)
{
  if (!arguments) {
      fprintf(stderr, "Error: NO EXISTEN SUFICIENTES PARAMETROS");
      exit(EXIT_FAILURE);
  }
    arguments->horaInicio == -1;
    arguments->horaFinal == -1;
    arguments->segundosHora == -1;
    arguments->totalPersonas == -1;
    arguments->pipeCrecibe == NULL;
 
  argp_parse(&argp, argc, argv, 0, 0, arguments);
}
