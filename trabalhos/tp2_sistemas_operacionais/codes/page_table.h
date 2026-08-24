#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

/* ============================================================
 * Quatro implementações de tabela de páginas.
 * A interface unificada (page_lookup / page_insert / page_remove)
 * usa a tabela densa como lookup principal e mantém as demais
 * sincronizadas para fins de comparação de desempenho.
 * ============================================================ */

void page_table_init(unsigned page_bits, unsigned nframes);
void page_table_free(void);

/* Interface unificada usada pelo simulador */
int page_lookup(unsigned page);
void page_insert(unsigned page, int frame_idx);
void page_remove(unsigned page);

/* Lookups individuais por tabela — para benchmarking */
int page_lookup_hier2(unsigned page);
int page_lookup_hier3(unsigned page);
int page_lookup_inv(unsigned page);

extern int (*page_lookup_function)(unsigned page);

#endif /* PAGE_TABLE_H */
