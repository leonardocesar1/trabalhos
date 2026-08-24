#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdio.h>

typedef struct {
  unsigned long total_accesses;
  unsigned long page_faults;
  unsigned long dirty_writes;
} SimStats;

/* Roda o loop de simulação sobre o arquivo já aberto.
 * method: "random" | "lru" | "nru" | "lfu"
 * s     : número de bits de offset (page = addr >> s)       */
SimStats simulate(FILE *fp, const char *method, unsigned s);

#endif /* SIMULATOR_H */
