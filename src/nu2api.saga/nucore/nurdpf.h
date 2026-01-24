#include "decomp.h"

typedef int nurdpgetvarfn(char *, float *, int *);

C_API_START

float NuRDPF(char *input);
float NuRDPFVar(char *input, nurdpgetvarfn *get_var_fn);

C_API_END
