
#include "argumentsAgent.h"

error_t parse_agent_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;

  switch (key) {
  case 's':
    arguments->nombre = arg;
    break;
  case 'a':
    arguments->archivo = arg;
    break;
  case 'p':
    arguments->pipeCrecibe = arg;
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

void init_agent_arguments(int argc, char *argv[], struct arguments *arguments) {
  if (!arguments) {
    fprintf(stderr, "Error: NO EXISTEN SUFICIENTES PARAMETROS");
    exit(EXIT_FAILURE);
  }
  arguments->nombre == NULL;
  arguments->archivo == NULL;
  arguments->pipeCrecibe == NULL;

  argp_parse(&argp, argc, argv, 0, 0, arguments);
}
