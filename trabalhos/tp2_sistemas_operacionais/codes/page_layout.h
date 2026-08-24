#ifndef PAGE_LAYOUT_H
#define PAGE_LAYOUT_H

typedef struct {
  unsigned int page;      // qual página virtual está aqui
  int valid;              // frame está ocupado?
  int dirty;              // foi escrito (W)?
  int referenced;         // bit R — para NRU
  unsigned long last_use; // timestamp — para LRU
  unsigned long freq;     // frequência de uso — para LFU
} page_t;

extern page_t *pages;
extern unsigned num_frames;
extern unsigned long tick;
extern int debug;

#endif
