#include "page_layout.h"
#include "page_table.h"
#include "simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Definição das globais declaradas em page_layout.h */
page_t *pages = NULL;
unsigned num_frames = 0;
unsigned long tick = 0;
int debug = 0;

void usage(void) {
  fprintf(stderr, "usage: ./simulador method arquivo.log tamanho_pagina_kb "
                  "tamanho_memoria_kb [debug]\n");
}

int parse_args(char **argv, char *method, char *log_file, unsigned *frame_size,
               unsigned *memory_size) {
  if (strcmp(argv[1], "random") == 0 || strcmp(argv[1], "lru") == 0 ||
      strcmp(argv[1], "nru") == 0 || strcmp(argv[1], "lfu") == 0) {
    strcpy(method, argv[1]);
  } else {
    fprintf(stderr, "Invalid method. Use random, lru, nru or lfu.\n");
    return -1;
  }
  strcpy(log_file, argv[2]);
  *frame_size = (unsigned)atoi(argv[3]);
  *memory_size = (unsigned)atoi(argv[4]);
  return 0;
}

int main(int argc, char **argv) {
  if (argc < 5 || argc > 6) {
    usage();
    return -1;
  }

  char method[10];
  char log_file[100];
  unsigned page_size = 0;
  unsigned memory_size = 0;

  if (parse_args(argv, method, log_file, &page_size, &memory_size) != 0)
    return -1;

  if (argc == 6)
    debug = 1;

  /* bits de offset dentro da página */
  unsigned bit_offset = 0;
  unsigned tmp = page_size * 1024;
  while (tmp > 1) {
    tmp >>= 1;
    bit_offset++;
  }

  /* número de frames físicos */
  num_frames = (memory_size * 1024) / (page_size * 1024);
  if (num_frames == 0) {
    fprintf(stderr, "Memoria menor que uma pagina.\n");
    return -1;
  }

  /* page_bits: bits que identificam a página virtual (cap em 20 → 4 MB) */
  unsigned page_bits = 32 - bit_offset;
  if (page_bits > 20)
    page_bits = 20;

  pages = calloc(num_frames, sizeof(page_t));
  if (!pages) {
    perror("calloc");
    return -1;
  }

  page_table_init(page_bits, num_frames);
  srand(42);

  FILE *log_fp = fopen(log_file, "r");
  if (log_fp == NULL) {
    fprintf(stderr, "Could not open log file.\n");
    return -1;
  }

  clock_t start_time = clock();

  SimStats stats = simulate(log_fp, method, bit_offset);

  clock_t end_time = clock();
  double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

  fclose(log_fp);

  printf("Executando o simulador...\n");
  printf("Arquivo de entrada: %s\n", log_file);
  printf("Tamanho da memoria: %u KB\n", memory_size);
  printf("Tamanho das paginas: %u KB\n", page_size);
  printf("Tecnica de reposicao: %s\n", method);
  printf("Paginas lidas: %lu\n", stats.page_faults);
  printf("Paginas escritas: %lu\n", stats.dirty_writes);

  printf("CSV_OUTPUT,%s,%s,%u,%u,%lu,%lu,%f\n", method, log_file, memory_size, page_size, stats.page_faults, stats.dirty_writes, time_spent);

  page_table_free();
  free(pages);
  return 0;
}