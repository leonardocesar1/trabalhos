#ifndef ALGORITHMS_H
#define ALGORITHMS_H

/* Retorna o índice do frame vítima.
 * Requer que frames[] e nframes estejam inicializados (frame.h). */

int algo_random(void);
int algo_lru(void);
int algo_nru(void);
int algo_lfu(void);

#endif /* ALGORITHMS_H */
