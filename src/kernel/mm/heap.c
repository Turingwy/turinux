#include "types.h"
#include "heap.h"
#include "vmm.h"
#include "pmm.h"
#include "stdio.h"

static heap_header_t *heap_first;
static uint32_t heap_top = HEAP_START;

void *kmalloc(uint32_t len)
{
    len += sizeof(heap_header_t);
    heap_header_t *cur_header = heap_first;
    heap_header_t *prev_header = NULL;
    
    while(cur_header)
    {
        if(cur_header->allocated == 0 && cur_header->length >= len) 
        {
            split_chunk(cur_header, len);
            cur_header->allocated = 1;
            return (void *)((uint32_t)cur_header+sizeof(heap_header_t));
        }
        prev_header = cur_header;
        cur_header = cur_header->next;
    }
    uint32_t chunk_start;
    if(!prev_header)
        chunk_start = HEAP_START;
    else
        chunk_start = (uint32_t)prev_header+prev_header->length;
    alloc_chunk(chunk_start, len);
    uint32_t i;
    cur_header = (heap_header_t *)chunk_start;
    cur_header->allocated = 1;
    cur_header->length += len;
    cur_header->next = NULL;
    cur_header->prev = prev_header;
    
    if(prev_header)
        prev_header->next = cur_header;
    if(heap_first == NULL)
        heap_first = cur_header;
    return (void *)(chunk_start + sizeof(heap_header_t));
}

void split_chunk(heap_header_t *header, uint32_t len)
{
    if(header->length - len <= sizeof(heap_header_t)) 
        return;
    heap_header_t *chunk_next = (uint32_t)header + header->length;
    chunk_next->length = header->length - len;
    chunk_next->prev = header;
    chunk_next->next = header->next;
    chunk_next->allocated = 0;
    header->next = chunk_next;
    header->length = len;    
}

void alloc_chunk(uint32_t chunk, uint32_t len)
{
    while(chunk+len > heap_top)
    {
        uint32_t page = (uint32_t)kpalloc();
        map(kern_pgd, heap_top, V2P(page), PAGE_WRITE | PAGE_PRESENT);
        heap_top += PAGE_SIZE;
    }
}

void kfree(void *p)
{
    heap_header_t *chunk = (heap_header_t *)((uint32_t)p - sizeof(heap_header_t));
    chunk->allocated = 0;
    glue_chunk(chunk);
}

void glue_chunk(heap_header_t *chunk)
{
    if(chunk->next && chunk->allocated == 0) 
    {
        chunk->length += chunk->next->length;
        chunk->next = chunk->next->next;
        if(chunk->next->next) 
        {
            chunk->next->next->prev = chunk;
        }
    }

    if(chunk->prev && chunk->allocated == 0) 
    {
        chunk->prev->length += chunk->length;
        chunk->prev->next = chunk->next;
        if(chunk->next)
        {
            chunk->next->prev = chunk->prev;
        }
        chunk = chunk->prev;
    }

    if(chunk->next == NULL) 
    {
        if(chunk->prev == NULL)
            heap_first = NULL;
        else
            chunk->prev->next = NULL;

        while (heap_top - PAGE_SIZE >= (uint32_t)chunk)
        {
            heap_top -= PAGE_SIZE;
            uint32_t p;
            getmapping(kern_pgd, heap_top, &p);
            kpfree(P2V(p));
            unmap(kern_pgd, heap_top);
        }
    }

}


