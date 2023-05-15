#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "slab.h"
#include <stdbool.h>

struct {
	struct spinlock lock;
	struct slab slab[NSLAB];
} stable;

bool get_bit(char* bit, int i) {
    return (bit[i / 8] & (1 << (i % 8))) != 0;
}

void set_bit(char* bit, int i) {
    bit[i / 8] |= (1 << (i % 8));
}

void clear_bit(char* bit, int i) {
    bit[i / 8] &= ~(1 << (i % 8));
}


void slabinit(){
	initlock(&stable.lock, "stable_lock");
	acquire(&stable.lock);

	for (int i = 0; i < NSLAB; i++) {

	   	struct slab *slab_ptr = &stable.slab[i];
        	slab_ptr->size = 8 << i;
        	slab_ptr->num_pages = 0;
        	slab_ptr->num_objects_per_page = PGSIZE / slab_ptr->size;
        	slab_ptr->num_free_objects = slab_ptr->num_objects_per_page;
        	slab_ptr->num_used_objects = 0;
        	slab_ptr->bitmap = kalloc();
        	memset(slab_ptr->bitmap, 0, PGSIZE);
        	slab_ptr->page[slab_ptr->num_pages++] = kalloc();
    }
	release(&stable.lock);
}



char *kmalloc(int size) {
    if (size < 0 || size > 2048) {
        return 0;
    }

    struct slab *slab_ptr = 0;
    acquire(&stable.lock);
    for (slab_ptr = stable.slab; slab_ptr < &stable.slab[NSLAB]; slab_ptr++) {
    	if (size <= slab_ptr->size) {
        	break;
        }
    }

    if (slab_ptr->num_free_objects == 0) {
	if(slab_ptr->size == 8 && slab_ptr->size > 64) {
		release(&stable.lock);
	}
        if (slab_ptr->num_pages >= MAX_PAGES_PER_SLAB) {
        	release(&stable.lock);
        	return 0;
        }

        slab_ptr->page[slab_ptr->num_pages] = kalloc();

	memset(slab_ptr->page[slab_ptr->num_pages], 0, PGSIZE);

	slab_ptr->num_pages += 1;
        slab_ptr->num_free_objects += slab_ptr->num_objects_per_page;
    }


    char* address = 0;
    int max = slab_ptr->num_pages * slab_ptr->num_objects_per_page;
     
    for (int i = 0; i < max; i++) {

	if (get_bit(slab_ptr->bitmap, i) == false) {
            	int index = i/slab_ptr->num_objects_per_page;
            	int offset = i%slab_ptr->num_objects_per_page;

		address = slab_ptr->page[index] + (offset * slab_ptr->size);
            	set_bit(slab_ptr->bitmap, i);

		slab_ptr->num_free_objects -= 1;
            	slab_ptr->num_used_objects += 1;
            	break;
        }
    }

    release(&stable.lock);
    return address;
}


void kmfree(char *address, int size) {
    struct slab *slab_ptr;
    int index, offset, max;

    acquire(&stable.lock);

    for (slab_ptr = stable.slab; slab_ptr < &stable.slab[NSLAB]; slab_ptr++) {
        if (size <= slab_ptr->size) {
            break;
        }
    }

    max = slab_ptr->num_pages * slab_ptr->num_objects_per_page;
    for (int i = 0; i < max; i++) {
        index = i / slab_ptr->num_objects_per_page;
        offset = i % slab_ptr->num_objects_per_page;

        if (address == (slab_ptr->page[index] + (offset * slab_ptr->size))) {
            memset(address, 0, slab_ptr->size);

            slab_ptr->num_free_objects +=1;
            slab_ptr->num_used_objects -=1;

            clear_bit(slab_ptr->bitmap, i);

            break;
        }
    }

    release(&stable.lock);
}




void slabdump(){
	cprintf("__slabdump__\n");

	struct slab *s;

	cprintf("size\tnum_pages\tused_objects\tfree_objects\n");

	for(s = stable.slab; s < &stable.slab[NSLAB]; s++){
		cprintf("%d\t%d\t\t%d\t\t%d\n",
			s->size, s->num_pages, s->num_used_objects, s->num_free_objects);
	}
}

int numobj_slab(int slabid)
{
	return stable.slab[slabid].num_used_objects;
}
