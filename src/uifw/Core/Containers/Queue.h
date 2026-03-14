#pragma once

#include <stdint.h>

typedef struct
{
  uint32_t element_size;
  uint32_t element_count;
  uint32_t allocated;
  void *memory;
} ui_Queue;

void ui_queueInit(ui_Queue *queue, uint32_t elementSize);

bool ui_queueEmpty(ui_Queue *queue);

void ui_queuePush(ui_Queue *queue, const void *element);
void ui_queuePeek(ui_Queue *queue, void *outData);
void ui_queuePop(ui_Queue *queue, void *outData);

void ui_queueDestroy(ui_Queue *queue);