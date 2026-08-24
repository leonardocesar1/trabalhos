#include "page_table.h"
#include <stdlib.h>

/* ============================================================
 * Tabela densa
 * entries[page] = frame_idx, ou -1 se ausente.
 * Custo: 4 * 2^page_bits bytes alocados de uma vez.
 * ============================================================ */

typedef struct {
  int *entries;
  unsigned size; /* 2^page_bits */
} DenseTable;

static DenseTable dense;

static void dense_init(unsigned page_bits) {
  dense.size = 1u << page_bits;
  dense.entries = malloc(dense.size * sizeof(int));
  for (unsigned i = 0; i < dense.size; i++)
    dense.entries[i] = -1;
}
static int dense_lookup(unsigned page) {
  return (page < dense.size) ? dense.entries[page] : -1;
}
static void dense_insert(unsigned page, int fi) {
  if (page < dense.size)
    dense.entries[page] = fi;
}
static void dense_remove(unsigned page) {
  if (page < dense.size)
    dense.entries[page] = -1;
}
static void dense_free(void) { free(dense.entries); }

/* ============================================================
 * Tabela hierárquica 2 níveis
 * page_bits dividido ao meio: l1_bits = page_bits/2 (arredondado
 * para cima), l2_bits = page_bits - l1_bits.
 * O array de L1 é alocado de uma vez; cada array de L2 é
 * alocado sob demanda quando o primeiro acesso ao grupo ocorre.
 * ============================================================ */
typedef struct {
  int **l1;         /* l1[i1] → array de l2_size ints, ou NULL */
  unsigned l1_size; /* 2^l1_bits */
  unsigned l2_size; /* 2^l2_bits */
  unsigned l1_bits;
  unsigned l2_bits;
} Hier2Table;

static Hier2Table h2;

static void hier2_init(unsigned page_bits) {
  h2.l1_bits = (page_bits + 1) / 2; /* metade superior, arredonda p/ cima */
  h2.l2_bits = page_bits - h2.l1_bits;
  h2.l1_size = 1u << h2.l1_bits;
  h2.l2_size = 1u << h2.l2_bits;
  h2.l1 = calloc(h2.l1_size, sizeof(int *)); /* NULL = L2 não alocado */
}
static int hier2_lookup(unsigned page) {
  unsigned i1 = (page >> h2.l2_bits) & (h2.l1_size - 1);
  unsigned i2 = page & (h2.l2_size - 1);
  if (!h2.l1[i1])
    return -1;
  return h2.l1[i1][i2];
}
static void hier2_insert(unsigned page, int fi) {
  unsigned i1 = (page >> h2.l2_bits) & (h2.l1_size - 1);
  unsigned i2 = page & (h2.l2_size - 1);
  if (!h2.l1[i1]) {
    h2.l1[i1] = malloc(h2.l2_size * sizeof(int));
    for (unsigned k = 0; k < h2.l2_size; k++)
      h2.l1[i1][k] = -1;
  }
  h2.l1[i1][i2] = fi;
}
static void hier2_remove(unsigned page) {
  unsigned i1 = (page >> h2.l2_bits) & (h2.l1_size - 1);
  unsigned i2 = page & (h2.l2_size - 1);
  if (h2.l1[i1])
    h2.l1[i1][i2] = -1;
}
static void hier2_free(void) {
  for (unsigned i = 0; i < h2.l1_size; i++)
    if (h2.l1[i])
      free(h2.l1[i]);
  free(h2.l1);
}

/* ============================================================
 * Tabela hierárquica 3 níveis
 * page_bits dividido em três partes o mais uniformemente possível:
 *   l1 = ceil(page_bits/3), l2 = ceil((page_bits-l1)/2), l3 = resto.
 * Arrays de L2 e L3 alocados sob demanda.
 * ============================================================ */
typedef struct {
  int ***l1; /* l1[i1][i2][i3] = frame_idx */
  unsigned l1_size, l2_size, l3_size;
  unsigned l1_bits, l2_bits, l3_bits;
} Hier3Table;

static Hier3Table h3;

static void hier3_init(unsigned page_bits) {
  h3.l1_bits = (page_bits + 2) / 3;
  h3.l2_bits = (page_bits - h3.l1_bits + 1) / 2;
  h3.l3_bits = page_bits - h3.l1_bits - h3.l2_bits;
  h3.l1_size = 1u << h3.l1_bits;
  h3.l2_size = 1u << h3.l2_bits;
  h3.l3_size = 1u << h3.l3_bits;
  h3.l1 = calloc(h3.l1_size, sizeof(int **));
}
static int hier3_lookup(unsigned page) {
  unsigned i1 = (page >> (h3.l2_bits + h3.l3_bits)) & (h3.l1_size - 1);
  unsigned i2 = (page >> h3.l3_bits) & (h3.l2_size - 1);
  unsigned i3 = page & (h3.l3_size - 1);
  if (!h3.l1[i1] || !h3.l1[i1][i2])
    return -1;
  return h3.l1[i1][i2][i3];
}
static void hier3_insert(unsigned page, int fi) {
  unsigned i1 = (page >> (h3.l2_bits + h3.l3_bits)) & (h3.l1_size - 1);
  unsigned i2 = (page >> h3.l3_bits) & (h3.l2_size - 1);
  unsigned i3 = page & (h3.l3_size - 1);
  if (!h3.l1[i1])
    h3.l1[i1] = calloc(h3.l2_size, sizeof(int *));
  if (!h3.l1[i1][i2]) {
    h3.l1[i1][i2] = malloc(h3.l3_size * sizeof(int));
    for (unsigned k = 0; k < h3.l3_size; k++)
      h3.l1[i1][i2][k] = -1;
  }
  h3.l1[i1][i2][i3] = fi;
}
static void hier3_remove(unsigned page) {
  unsigned i1 = (page >> (h3.l2_bits + h3.l3_bits)) & (h3.l1_size - 1);
  unsigned i2 = (page >> h3.l3_bits) & (h3.l2_size - 1);
  unsigned i3 = page & (h3.l3_size - 1);
  if (h3.l1[i1] && h3.l1[i1][i2])
    h3.l1[i1][i2][i3] = -1;
}
static void hier3_free(void) {
  for (unsigned i = 0; i < h3.l1_size; i++) {
    if (!h3.l1[i])
      continue;
    for (unsigned j = 0; j < h3.l2_size; j++)
      if (h3.l1[i][j])
        free(h3.l1[i][j]);
    free(h3.l1[i]);
  }
  free(h3.l1);
}

/* ============================================================
 * Tabela invertida (hash por encadeamento)
 * Tamanho fixo proporcional ao número de frames — O(nframes),
 * independente do espaço de endereçamento virtual.
 * ============================================================ */
typedef struct HashEntry {
  unsigned int page;
  int frame_idx;
  struct HashEntry *next;
} HashEntry;

typedef struct {
  HashEntry **buckets;
  unsigned nbuckets;
} InvTable;

static InvTable inv;

static void inv_init(unsigned nframes) {
  inv.nbuckets = nframes * 2 + 1; /* carga ~50% */
  inv.buckets = calloc(inv.nbuckets, sizeof(HashEntry *));
}
static int inv_lookup(unsigned page) {
  for (HashEntry *e = inv.buckets[page % inv.nbuckets]; e; e = e->next)
    if (e->page == page)
      return e->frame_idx;
  return -1;
}
static void inv_insert(unsigned page, int fi) {
  unsigned b = page % inv.nbuckets;
  for (HashEntry *e = inv.buckets[b]; e; e = e->next)
    if (e->page == page) {
      e->frame_idx = fi;
      return;
    }
  HashEntry *e = malloc(sizeof(HashEntry));
  e->page = page;
  e->frame_idx = fi;
  e->next = inv.buckets[b];
  inv.buckets[b] = e;
}
static void inv_remove(unsigned page) {
  HashEntry **ep = &inv.buckets[page % inv.nbuckets];
  while (*ep) {
    if ((*ep)->page == page) {
      HashEntry *tmp = *ep;
      *ep = (*ep)->next;
      free(tmp);
      return;
    }
    ep = &(*ep)->next;
  }
}
static void inv_free(void) {
  for (unsigned i = 0; i < inv.nbuckets; i++) {
    HashEntry *e = inv.buckets[i];
    while (e) {
      HashEntry *nx = e->next;
      free(e);
      e = nx;
    }
  }
  free(inv.buckets);
}

/* ============================================================
 * Interface pública
 * ============================================================ */
void page_table_init(unsigned page_bits, unsigned nframes) {
  dense_init(page_bits);
  hier2_init(page_bits);
  hier3_init(page_bits);
  inv_init(nframes);
}

void page_table_free(void) {
  dense_free();
  hier2_free();
  hier3_free();
  inv_free();
}

int page_lookup(unsigned page) { return page_lookup_function(page); }

void page_insert(unsigned page, int fi) {
  dense_insert(page, fi);
  hier2_insert(page, fi);
  hier3_insert(page, fi);
  inv_insert(page, fi);
}

void page_remove(unsigned page) {
  dense_remove(page);
  hier2_remove(page);
  hier3_remove(page);
  inv_remove(page);
}

int page_lookup_hier2(unsigned page) { return hier2_lookup(page); }
int page_lookup_hier3(unsigned page) { return hier3_lookup(page); }
int page_lookup_inv(unsigned page) { return inv_lookup(page); }

int (*page_lookup_function)(unsigned page) = dense_lookup;
