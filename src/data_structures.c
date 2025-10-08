#include "data_structures.h"

IMPLEMENT_VECTOR_INTERFACE(VertexDistance, VecVertexDistance);


__left(uint64_t index)
{
    return 2 * index + 1;
};
__right(uint64_t index)
{
    return 2 * index + 2;
};
__parent(uint64_t index)
{
    return (index - 1) / 2;
};
int __is_valid(uint64_t index, MinHeap const *heap)
{
    uint64_t heap_size = VecVertexDistance_size(&heap->data);
    return index < heap_size;
};

__exchange(uint64_t i, uint64_t j, MinHeap *heap)
{
    VertexDistance temp = VecVertexDistance_get(&heap->data, i);
    VecVertexDistance_set(&heap->data, i, VecVertexDistance_get(&heap->data, j));
    VecVertexDistance_set(&heap->data, j, temp);
};



void __max_heapify(uint64_t index, MinHeap *heap)
{
    uint64_t left = __left(index);
    uint64_t right = __right(index);
    uint64_t largest = index;
    if (__is_valid(left, heap) && VecVertexDistance_get(&heap->data, left).distance > VecVertexDistance_get(&heap->data, largest).distance)
    {
        largest = left;
    }
    if (__is_valid(right, heap) && VecVertexDistance_get(&heap->data, right).distance > VecVertexDistance_get(&heap->data, largest).distance)
    {
        largest = right;
    }
    if (largest != index)
    {
        __exchange(index, largest, heap);
        __max_heapify(largest, heap);
    }
};



void MinHeap_init(MinHeap *heap)
{
    VecVertexDistance_init(&heap->data);
};
void MinHeap_free(MinHeap *heap)
{
    VecVertexDistance_free(&heap->data);
};
void MinHeap_is_empty(MinHeap const *heap)
{
    return VecVertexDistance_is_empty(&heap->data);
};

void MinHeap_add(MinHeap *heap, uint64_t vertex_id,double distance)
{
    VertexDistance element={vertex_id, distance};
    VecVertexDistance_push_back(&heap->data, element);
    uint64_t index = VecVertexDistance_size(&heap->data) - 1;
    while (index > 0 && VecVertexDistance_get(&heap->data, __parent(index)).distance < VecVertexDistance_get(&heap->data, index).distance)
    {
        __exchange(index, __parent(index), heap);
        index = __parent(index);
    }
};


uint64_t MinHeap_get(MinHeap* heap)
{
    VertexDistance max_element = VecVertexDistance_get(&heap->data, 0);
    uint64_t last_index = VecVertexDistance_size(&heap->data) - 1;
    VecVertexDistance_set(&heap->data, 0, VecVertexDistance_get(&heap->data, last_index));
    vector_VertexDistance_pop_back(&heap->data);
    __max_heapify(0, heap);
    return max_element.vertex_id;
};