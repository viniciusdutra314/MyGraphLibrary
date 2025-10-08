#include "data_structures.h"
#include <graph_library.h>

IMPLEMENT_VECTOR_INTERFACE(VertexWithPriority, VecVertexWithPriority)
IMPLEMENT_VECTOR_INTERFACE(size_t, VecSizeT)

size_t __left(size_t index)
{
    return 2 * index + 1;
}
size_t  __right(size_t index)
{
    return 2 * index + 2;
}
size_t  __parent(size_t index)
{
    return (index - 1) / 2;
}
int __is_valid(size_t index, MinHeap const *heap)
{
    size_t heap_size = VecVertexWithPriority_size(&heap->data);
    return index < heap_size;
}

void __exchange(size_t i, size_t j, MinHeap *heap)
{
    VertexWithPriority temp = VecVertexWithPriority_get(&heap->data, i);
    VecVertexWithPriority_set(&heap->data, i, VecVertexWithPriority_get(&heap->data, j));
    VecVertexWithPriority_set(&heap->data, j, temp);
}

void __min_heapify(size_t index, MinHeap *heap)
{
    size_t left = __left(index);
    size_t right = __right(index);
    size_t smallest = index;
    if (__is_valid(left, heap) && VecVertexWithPriority_get(&heap->data, left).distance < VecVertexWithPriority_get(&heap->data, smallest).distance)
    {
        smallest = left;
    }
    if (__is_valid(right, heap) && VecVertexWithPriority_get(&heap->data, right).distance < VecVertexWithPriority_get(&heap->data, smallest).distance)
    {
        smallest = right;
    }
    if (smallest != index)
    {
        __exchange(index, smallest, heap);
        __min_heapify(smallest, heap);
    }
}

void MinHeap_init(MinHeap *heap)
{
    VecVertexWithPriority_init(&heap->data);
    VecSizeT_init(&heap->index_map);
}
void MinHeap_free(MinHeap *heap)
{
    VecVertexWithPriority_free(&heap->data);
}
int MinHeap_is_empty(MinHeap const *heap)
{
    return VecVertexWithPriority_is_empty(&heap->data);
}

void MinHeap_add(MinHeap *heap, size_t vertex_id, double distance)
{
    VertexWithPriority element = {vertex_id, distance};
    VecVertexWithPriority_push_back(&heap->data, element);
    size_t index = VecVertexWithPriority_size(&heap->data) - 1;
    while (index > 0 && VecVertexWithPriority_get(&heap->data, __parent(index)).distance > VecVertexWithPriority_get(&heap->data, index).distance)
    {
        __exchange(index, __parent(index), heap);
        index = __parent(index);
    }
}

size_t MinHeap_get(MinHeap *heap)
{
    VertexWithPriority min_element = VecVertexWithPriority_get(&heap->data, 0);
    size_t last_index = VecVertexWithPriority_size(&heap->data) - 1;
    VecVertexWithPriority_set(&heap->data, 0, VecVertexWithPriority_get(&heap->data, last_index));
    VecVertexWithPriority_pop_back(&heap->data);
    __min_heapify(0, heap);
    return min_element.vertex_id;
}
void MinHeap_decrease_key(MinHeap *heap, size_t vertex_id, double new_distance)
{
    size_t index = VecSizeT_get(&heap->index_map, vertex_id);
    VecVertexWithPriority_set(&heap->data, index, (VertexWithPriority){vertex_id, new_distance});
    while (index > 0 && VecVertexWithPriority_get(&heap->data, index).distance < VecVertexWithPriority_get(&heap->data, __parent(index)).distance)
    {
        __exchange(index, __parent(index), heap);
        index = __parent(index);
    }
}