#include "algorithms.h"
#include "page_layout.h"
#include <stdlib.h>

/* ============================================================
 * RANDOM — escolha aleatória entre todos os frames.
 * Serve de baseline: sem custo de bookkeeping, mas ignora
 * qualquer padrão de acesso.
 * ============================================================ */
int algo_random(void) { return (int)(rand() % num_frames); }

/* ============================================================
 * LRU — Least Recently Used.
 * Substitui o frame cujo último acesso foi o mais antigo.
 * Implementação por varredura linear: O(n) por substituição,
 * sem overhead por acesso normal.
 * ============================================================ */
int algo_lru(void) {
  int victim = 0;
  unsigned long oldest = pages[0].last_use;
  for (unsigned i = 1; i < num_frames; i++) {
    if (pages[i].last_use < oldest) {
      oldest = pages[i].last_use;
      victim = i;
    }
  }
  return victim;
}

/* ============================================================
 * NRU — Not Recently Used.
 * Classifica frames em 4 classes pelo par (referenced, dirty):
 *   Classe 0: !R !D  →  melhor candidato
 *   Classe 1: !R  D
 *   Classe 2:  R !D
 *   Classe 3:  R  D  →  pior candidato
 * Escolhe aleatoriamente dentro da melhor classe disponível.
 * O bit R é zerado periodicamente pelo loop principal
 * (a cada NRU_PERIOD acessos), simulando a ação do SO real.
 * ============================================================ */
#define NRU_PERIOD 200
#define MAX_FRAMES 16384 /* cap para o VLA de candidatos */

int algo_nru(void) {
  int best_class = 4;
  int candidates[MAX_FRAMES];
  int nc = 0;

  for (unsigned i = 0; i < num_frames; i++) {
    int cls = (pages[i].referenced ? 2 : 0) | (pages[i].dirty ? 1 : 0);
    if (cls < best_class) {
      best_class = cls;
      nc = 0;
    }
    if (cls == best_class)
      candidates[nc++] = (int)i;
  }
  return candidates[rand() % nc];
}

/* ============================================================
 * LFU — Least Frequently Used.
 * Substitui o frame com menor número total de acessos desde
 * que foi carregado. Tende a reter páginas "quentes" mas pode
 * acumular páginas antigas com frequência historicamente alta.
 * ============================================================ */
int algo_lfu(void) {
  int victim = 0;
  unsigned long min_freq = pages[0].freq;
  for (unsigned i = 1; i < num_frames; i++) {
    if (pages[i].freq < min_freq) {
      min_freq = pages[i].freq;
      victim = (int)i;
    }
  }
  return victim;
}
