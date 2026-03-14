#include "Queue.h"

#include "uifw/Core/Utils/Log.h"

#include <stdlib.h>

static void queue_ensure_allocated(ui_Queue *q, const uint32_t count)
{
  if (q->allocated < q->element_size * count) {
    const uint32_t newAllocated = count * q->element_size;
    void *temp = realloc(q->memory, newAllocated);

    if (!temp) {
      ui_LogFatal("ui_Queue memory allocation failed");
      return;
    }

    q->memory = temp;
    q->allocated = newAllocated;
  }
}

void ui_queueInit(ui_Queue *queue, const uint32_t elementSize)
{
  if (!queue) {
    ui_LogFatal("ui_queueInit requires a pointer to a valid ui_Queue.");
    return;
  }

  memset(queue, 0, sizeof(ui_Queue));

  queue->element_size = elementSize;
  queue->element_count = 0;

  queue_ensure_allocated(queue, 1);
}

bool ui_queueEmpty(ui_Queue *queue)
{
  if (!queue) {
    ui_LogFatal("ui_queueEmpty requires a pointer to a valid ui_Queue");
    return true;
  }

  return queue->element_count == 0;
}

void ui_queuePush(ui_Queue *queue, const void *element)
{
  if (!queue) {
    ui_LogFatal("ui_queuePush requires a pointer to a valid ui_Queue.");
    return;
  }

  queue_ensure_allocated(queue, queue->element_count + 1);

  memcpy(
    (void *)((uintptr_t)queue->memory + (queue->element_count * queue->element_size)),
    element, queue->element_size);

  queue->element_count++;
}

void ui_queuePeek(ui_Queue *queue, void *outData)
{
  if (!queue || !outData) {
    ui_LogFatal("ui_queuePeek requires a pointer to a valid ui_Queue.");
    return;
  }

  if (queue->element_count < 1) {
    ui_LogFatal("Cannot peek from an empty ui_Queue");
    return;
  }

  memcpy(outData, queue->memory, queue->element_size);
}

void ui_queuePop(ui_Queue *queue, void *outData)
{
  if (!queue || !outData) {
    ui_LogFatal("ui_queuePop requires a pointer to a valid ui_Queue.");
    return;
  }

  ui_queuePeek(queue, outData);

  const uint32_t countToMove = queue->element_count - 1;
  memmove(queue->memory, (void *)((uintptr_t)queue->memory + queue->element_size),
          queue->element_size * countToMove);

  queue->element_count--;
}

void ui_queueDestroy(ui_Queue *queue)
{
  if (queue) {
    if (queue->memory) {
      free(queue->memory);
    }

    memset(queue, 0, sizeof(ui_Queue));
  }
}
