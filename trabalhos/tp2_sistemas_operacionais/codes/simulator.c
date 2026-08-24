#include "simulator.h"
#include "algorithms.h"
#include "page_layout.h"
#include "page_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NRU_PERIOD 200

SimStats simulate(FILE *fp, const char *method, unsigned s) {
  SimStats stats = {0, 0, 0};
  unsigned long nru_next = NRU_PERIOD;

  /* Pilha de frames livres (índices 0..nframes-1 em ordem reversa) */
  int *free_stack = malloc(num_frames * sizeof(int));
  if (free_stack == NULL) {
    fprintf(stderr, "Failed to allocate free stack\n");
    exit(1);
  }
  int free_top = (int)num_frames;
  for (unsigned i = 0; i < num_frames; i++)
    free_stack[num_frames - 1 - i] = (int)i;

  unsigned addr;
  char rw;

  while (fscanf(fp, "%x %c", &addr, &rw) == 2) {
    stats.total_accesses++;
    tick++;

    /* Zera bits R periodicamente (NRU) */
    if (tick >= nru_next) {
      for (unsigned i = 0; i < num_frames; i++)
        pages[i].referenced = 0;
      nru_next = tick + NRU_PERIOD;
    }

    unsigned page = addr >> s;
    int fi = page_lookup(page);

    if (fi == -1) {
      /* PAGE FAULT */
      stats.page_faults++;

      if (free_top > 0) {
        fi = free_stack[--free_top];
      } else {
        /* Escolhe vítima */
        if (strcmp(method, "random") == 0)
          fi = algo_random();
        else if (strcmp(method, "lru") == 0)
          fi = algo_lru();
        else if (strcmp(method, "nru") == 0)
          fi = algo_nru();
        else
          fi = algo_lfu();

        if (pages[fi].dirty)
          stats.dirty_writes++;
        page_remove(pages[fi].page);

        if (debug)
          printf("[DEBUG] Evicting page %u from frame %d (dirty=%d)\n",
                 pages[fi].page, fi, pages[fi].dirty);
      }

      /* Carrega nova página no frame escolhido */
      pages[fi].page = page;
      pages[fi].valid = 1;
      pages[fi].dirty = 0;
      pages[fi].referenced = 0;
      pages[fi].last_use = tick;
      pages[fi].freq = 0;
      page_insert(page, fi);

      if (debug)
        printf("[DEBUG] tick=%lu addr=%x page=%u frame=%d FAULT\n", tick, addr,
               page, fi);
    } else {
      if (debug)
        printf("[DEBUG] tick=%lu addr=%x page=%u frame=%d HIT\n", tick, addr,
               page, fi);
    }

    /* Atualiza metadados do frame */
    pages[fi].referenced = 1;
    pages[fi].last_use = tick;
    pages[fi].freq++;
    if (rw == 'W')
      pages[fi].dirty = 1;
  }

  free(free_stack);
  return stats;
}
