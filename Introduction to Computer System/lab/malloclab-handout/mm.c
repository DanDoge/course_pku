/*
 * mm.c
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 *
 * final version
 */

/* description
 *  this approach(last modified date: 10/12)
 *  uses segregated fit & best fit & LIFO & immediate coalescing
 *  that is each require will be allocated in the smallest block
 *  availble(if any), or we will add one at-least-4kb-big block
 *  reference: (some slides from cmu recitation class)
 *             http://www.cs.cmu.edu/afs/cs/academic/class/15213-s08
                     /www/recitation/mallocadvice.pdf
 */

/* here's what heap's head looks like
 *  pointer to start of linked list of blocks smaller than or equal to 32
 *  64bytes' start
 *  128bytes' start
 *  256bytes' start
 *  512bytes' start
 *  768bytes' start
 *  1024bytes' start
 *  2048bytes' start
 *  4096
 *  4096 * 2
 *  4096 * 3
 *  oterwise
 *  each time we need to add one block to some linked lst, add to the start
 *  and when delete some block, just like normal linked lists
 */

/* here's what a free block looks loke
 *  size     --- header start here <- where the base ptr should be
 *  next     ---
 *  prior    ---        ends  here
 *  (real block starts here)       <- where the return ptr should be
 *  .....
 *  (suppose it ends here)
 *  size     --- footer here
 */

/* here's what an allocated block looks loke
 *  size     --- headers here  <- where the original ptr should be
 *  (real block starts here)   <- where the return ptr should be
 *  .....
 *  (suppose it ends here)
 *  size     --- footer here
 */

/* log
 *  01/12 21:31 compile with no errors(coalesce unfinished)
 *  03/12 16:27 rewrite all
 *        17:06 finished, segv
 *        18:33 can only run for several trace lines...gg
 *        23:13 something must be wrong with linked list
 *  04/12 00:41 correctness --> passed (coalesce/check unfinished)
 *              some parameters: 59%     46       0/100
 *              need to sleep now...
 *  04/12 10:58 work on coalesce
 *        14:12 done        --> passed (low score)
 *              some parameters: 69%     75      17/100
 *        15:58 well, i should have started after this lecture...
 *        16:10 seems better, for i give 4 cores to this virtual machine?
 *              some parameters: 74%  14721      65/100
 *        16:38 comments added
 *        16:59 some parameters: 74%  23021      65/100 (on class machines)
 *  06/12 20:17 start optimizing: first-fit ==> best-fit       done 22:06
 *                                rewrite realloc              done 21:27
 *        22:07 some parameters: 74%  12294      64/100
 *  07/12 22:36 optimazing falied(compress pointers)
 *              some parameters: 80%  12762      77/100 (local)
 *        22:55 roll back to 04/12 sad...
 *              some parameters: 78%  14748      73/100 (local)
 *  10/12 16:36 some parameters: 81%  11085      73/100 (local)
 *        16:50 dbg finished, stop modifing malloclab
 *  11/12 21:05 polish some lines
 *              some parameters: 84%  11379      79/100 (local)
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */
/* #define DEBUG */
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT-1)) & ~0x7)

/* minimumal size of a block */
#define min_page_size  (3 * ALIGNMENT)

/* size of a physical page(4kb) */
#define CHUNKSIZE (1 << 12)

/* inline functions
 *  the "set_xxx" functions return address of xxx
 *  the "get_xxx" functions return value of xxx
 *                (although it can still be a function)
 *  the "xxx_p" functions return a pointer,
 *  the "xxx" functions return *xxx_p if they share the same prefix
 */
#define set_value(addr, val) (*(unsigned long* )(addr) = (val))
#define get_header_p(ptr_real_block) (ptr_real_block - 8)
#define get_size(ptr_h) ((*(unsigned long* )(ptr_h)))
#define get_size_aligned(ptr_h) ((*(unsigned long* )(ptr_h)) & (~7))
#define get_next_blk_p(ptr_h) (ptr_h + get_size_aligned(ptr_h))
#define get_prior_blk_p(ptr_h) (ptr_h - get_size_aligned(ptr_h - 8))
#define get_next_blk_lkd_p(ptr_h) *(unsigned long* )((void* )((ptr_h) + 8))
#define get_prior_blk_lkd_p(ptr_h) *(unsigned long* )((void* )((ptr_h) + 16))
#define get_footer_p(ptr_h) (ptr_h + get_size_aligned(ptr_h) - 8)
#define get_first_blk_p(ptr) (void* )(*(unsigned long* )(ptr))
#define set_next_blk_lkd_p(ptr_h) (unsigned long* )((void* )ptr_h + 8)
#define set_prior_blk_lkd_p(ptr_h) (unsigned long* )((void* )ptr_h + 16)


/* global variables
 *  heap_list_head points to the head of whole list
 *  whereas heap_head points to the first block(allocated or not allocated)
 *  heap_end points to the footer of the last block
 */
static char* heap_head = NULL;
static char* heap_end = NULL;
static char* heap_list_head = NULL;

/* global functions */
static void* extent_heap(size_t words);
static void* place(void* bp, size_t asize);
static void* find_fit(size_t* asize);
static void* coalesce(void* bp);
static int   get_offset(size_t asize);
static void  delete_from_linked_list(void* p, int offset);
static void  add_to_linked_list(void* p, size_t size);
static int aligned(const void* p);
/*
 * malloc         01/12
 * realloc        01/12
 * mm_init        01/12
 * place          01/12
 * findfit        01/12
 * extend_heap    01/12
 * place          01/12
 * free           01/12
 * mm_checkheap   02/12
 * coalesce       03/12
 */

/* some debug function(s), used in mm_checkheap() */
void dbg(){
    /* not finished!!! */
    return ;// same as "remove # define debug"
    /* print heap_list */
    for(int i = 0; i < 8 * 14; i += 8){
        printf("%p", *(unsigned long* )(heap_list_head + i));
    }
    printf("\n");
    /* print each list(if not empty) */
    for(int i = 0; i < 8 * 14; i += 8){
        if(*(unsigned long* )(heap_list_head + i)){
            void* p = get_first_blk_p(heap_list_head + i);
            while(p){
                printf("%llx, %llx, %llx\n",
                      *(unsigned long* )p,
                      get_next_blk_lkd_p(p),
                      get_prior_blk_lkd_p(p));
                p = get_next_blk_lkd_p(p);
            }
            printf("\n");
        }
    }
    /* print blocks */
    for(void*p = heap_head;
        get_footer_p(p) != heap_end;
        p = get_next_blk_p(p)){
        printf("%llx", *(unsigned long* )p);
    }
    /* check segregated lists */
    for(int i = 0; i < 8 * 14; i += 8){
        if(*(unsigned long* )(heap_list_head + i)){
            void* p = get_first_blk_p(heap_list_head + i);
            while(p){
                if(get_offset(get_size_aligned(p)) != i){
                    printf("error: not in right segregated list\n");
                }
                if(p < heap_head || get_footer_p(p) > heap_end){
                    printf("error: boundarys\n");
                }
                if(get_next_blk_lkd_p(p) && (get_prior_blk_lkd_p(p)) != p){
                    printf("error: linked list\n");
                }
                p = get_next_blk_lkd_p(p);
            }
        }
    }
    /* check blocks */
    for(void* p = heap_head;
        get_footer_p(p) != heap_end;
        p = get_next_blk_p(p)){
        if(!aligned(p)){
            printf("error: alignment\n");
        }
        if(get_prior_blk_p(get_next_blk_p(p)) != p){
            printf("error: blocks\n");
        }
        if(p < heap_head || get_footer_p(p) > heap_end){
            printf("error: boundarys\n");
        }
    }
}


/*
 * Initialize: return -1 on error, 0 on success.
 */
int mm_init(void) {
    /* 01/12 first version */
    /* 03/12 second version */
    if((heap_list_head = mem_sbrk(ALIGNMENT * 14)) == (void* )-1){
        return -1;
    }
    heap_end = heap_head = (char* )(heap_list_head + ALIGNMENT * 14);
    memset(heap_list_head, 0, ALIGNMENT * 14);
    //puts("init_exit");
    return 0;
}

/*
 * malloc
 *  almost same as mm_textbook.c
 */
void *malloc (size_t size) {
    /* 01/12 first version */
    size_t asize = 0;
    size_t extendsize = 0;
    char* bp = NULL;

    if(heap_list_head == 0){
        mm_init();
    }
    if(size == 0){
        return NULL;
    }
    asize = 8 * ((size + 23) / 8);

    if((bp = find_fit(&asize)) != NULL){
        bp = place(bp, asize);

    }else{
        /* at least one page(4kb) */
        extendsize = MAX(asize, 1 << 8);
        extent_heap(extendsize / ALIGNMENT);
        bp = find_fit(&asize);
        bp = place(bp, asize);
    }
    return bp;
}

/*
 * free
 *  try to coalesce this block before return
 */
void free (void *ptr) {
    /* 01/12 first version */
    if(ptr == 0){
        return ;
    }
    if(heap_list_head == 0){
        mm_init();
    }

    void* ptr_header = get_header_p(ptr);
    /* not in use, reset size's last bit */
    get_size(ptr_header) &= ~1;
    set_value(get_footer_p(ptr_header), get_size(ptr_header));
    add_to_linked_list(ptr_header, get_size_aligned(ptr_header));
    ptr_header = coalesce(ptr_header);
    return ;
}

/*
 * realloc - you may want to look at mm-naive.c
 *  emm...yes
 */
void *realloc(void *oldptr, size_t size) {
    /* 01/12 first version */
    size_t oldsize;
    void* newptr;

    if(size == 0){
        free(oldptr);
        return NULL;
    }

    if(oldptr == NULL){
        return malloc(size);
    }

    newptr = malloc(size);
    if(!newptr){
        return 0;
    }

    oldsize = get_size_aligned(get_header_p(oldptr));
    if(size < oldsize){
        oldsize = size;
    }
    memcpy(newptr, oldptr, oldsize);

    free(oldptr);
    return newptr;
}

/*
 * calloc - you may want to look at mm-naive.c
 * This function is not tested by mdriver, but it is
 * needed to run the traces.
 */
void *calloc (size_t nmemb, size_t size) {
    /* 01/12 first version */
    size_t bytes = nmemb * size;
    void *newptr;

    newptr = malloc(bytes);
    memset(newptr, 0, bytes);

    return newptr;
}

/* return offset from heap_list_head */
static int get_offset(size_t asize){
    /* 01/12 first version */
    /* 04/12 second version */
    if(asize <= 32){
        return 0;
    }else if(asize <= 64){
        return 1 << 3;
    }else if(asize <= 128){
        return 2 << 3;
    }else if(asize <= 256){
        return 3 << 3;
    }else if(asize <= 512){
        return 4 << 3;
    }else if(asize <= 768){
        return 5 << 3;
    }else if(asize <= 1024){
        return 6 << 3;
    }else if(asize <= 2048){
        return 7 << 3;
    }else if(asize <= 4096){
        return 8 << 3;
    }else if(asize <= 8192){
        return 9 << 3;
    }else if(asize <= 8192 * 2){
        return 10 << 3;
    }else if(asize <= 4096 * 3){
        return 11 << 3;
    }else{
        return 12 << 3;
    }
    return -1;
}


static void* find_fit(size_t* asize_ref){
    /* 01/12 first version */
    /* 03/12 second version */
    /* 04/12 third version */
    /* 06/12 fourth version */
    /* find the first suitable linked list that's not empty(if any) */
    int origin_offset = get_offset(*asize_ref);
    for(int offset = origin_offset;
        offset < ALIGNMENT * 13;
        offset += 8){

        long min_size = (long)1 << 32;
        void* best_fit_ptr = NULL;
        /* find the best suitable block(if any) */
        /* too slow... */
        for(void* p = get_first_blk_p(heap_list_head + offset);
            p != NULL;
            p = (unsigned long* )get_next_blk_lkd_p(p)){

            /* well, ((ux - uy) >= 0) === 1 */
            if((long)get_size(p) >= (long)(*asize_ref)){
                if(get_size(p) < min_size){
                    min_size = get_size(p);
                    best_fit_ptr = p;
                    /* wish it could run faster */
                    if(min_size == (long)(*asize_ref)
                       || (min_size != (long)1 << 32
                           && offset != origin_offset)){
                        goto label1;
                    }
                }
            }
        }
        label1:
        if(best_fit_ptr != NULL){
            if(min_size - (long)(*asize_ref) >= 24){
                /* delete p from linked list */
                delete_from_linked_list(best_fit_ptr, offset);
                /* Initialize block <- no, that should be done in place() */
                add_to_linked_list((void* )((char*)best_fit_ptr + (*asize_ref))
                                  , get_size(best_fit_ptr) - (*asize_ref));
            }else{
                *asize_ref = get_size_aligned(best_fit_ptr);
                delete_from_linked_list(best_fit_ptr, offset);
            }
            return best_fit_ptr;
        }
    }
    return NULL;
}

static void delete_from_linked_list(void* p, int offset){
    /* 01/12 first version */
    /* 03/12 second version */
    /* dealing with some special cases first */
    if(get_size_aligned(p) <= min_page_size){
        return ;
    }
    if(get_prior_blk_lkd_p(p) == (unsigned long)(heap_list_head + offset)
      && get_next_blk_lkd_p(p) == 0){
        /* the only element in this list */
        set_value(heap_list_head + offset, 0);
    }else if(get_prior_blk_lkd_p(p)
             == (unsigned long)(heap_list_head + offset)){
        /* the first but not the last */
        set_value(heap_list_head + offset
                 , get_next_blk_lkd_p(p));
        /* same as p->next->prior = xxx */
        set_value(set_prior_blk_lkd_p(get_next_blk_lkd_p(p))
                 , (unsigned long)(heap_list_head + offset));
    }else if(get_next_blk_lkd_p(p) == 0){
        /* the last element */
        set_value(set_next_blk_lkd_p(get_prior_blk_lkd_p(p)), 0);
    }else{
        /* normal cases */
        /* ugly code */
        set_value(set_prior_blk_lkd_p(get_next_blk_lkd_p(p))
                 , get_prior_blk_lkd_p(p));
        set_value(set_next_blk_lkd_p(get_prior_blk_lkd_p(p))
                 , get_next_blk_lkd_p(p));
    }
    return ;
}

static void add_to_linked_list(void* p, unsigned long size){
    /* 01/12 first version */
    /* 03/12 second version */
    if(size <= min_page_size){
        /* become trash, bad <- won't happen actually*/
        //puts("trash, and i dont know what to do!");
        //set_value(p, 0);
        get_size(p) = size;
        set_value(get_footer_p(p), size);
        return ;
    }else{
        /* let it be the first element */
        int offset = get_offset(size);
        set_value(set_next_blk_lkd_p(p)
                 , *(unsigned long* )(heap_list_head + offset));
        set_value(set_prior_blk_lkd_p(p)
                 , (unsigned long)(heap_list_head + offset));
        set_value(heap_list_head + offset
                 , (unsigned long)p);
        get_size(p) = size;
        if(get_next_blk_lkd_p(p) != 0){
            set_value(set_prior_blk_lkd_p(get_next_blk_lkd_p(p))
                     , (unsigned long)p);
        }
        set_value(get_footer_p(p), size);
        return ;
    }
}

/* only extent heap by "words" words */
static void* extent_heap(size_t words){
    /* 01/12 first version */
    /* 03/12 second version */
    /* actually dont know why we should do this <- keep it temporarily */
    size_t size = (words % 2) ? ((words + 1) << 3) : (words << 3);
    //size_t size = words << 3;
    char* bp = NULL;
    if((bp = mem_sbrk(size)) == (void* )-1){
        return NULL;
    }
    add_to_linked_list((void* )bp, size);
    heap_end = get_footer_p(bp);
    return (void*)coalesce(bp);
}

/* turn a free block to an occupied block, return where "next" should be */
static void* place(void* bp, size_t asize){
    /* 01/12 first version */
    get_size(bp) = asize | 1;
    set_value(get_footer_p(bp), get_size(bp));
    return (void* )((char* )bp + 8);
}

static void* coalesce(void* bp){
    /* 04/12 first version */
    /* dealing with special cases first */
    delete_from_linked_list(bp, get_offset(get_size_aligned(bp)));
    if(bp == heap_head && get_footer_p(bp) == heap_end){
        /* only one block in heap */
        bp = bp;
    }else if(bp == heap_head){
        /* the first block but not the last */
        void* ptr_next = get_next_blk_p(bp);
        if(get_size(ptr_next) == get_size_aligned(ptr_next)){
            delete_from_linked_list(ptr_next
                                   , get_offset(get_size_aligned(ptr_next)));
            get_size(bp) += get_size_aligned(ptr_next);
            set_value(get_footer_p(bp)
                     , get_size(bp));
        }
    }else if(get_footer_p(bp) == heap_end){
        /* the last block but not the first */
        void* ptr_prior = get_prior_blk_p(bp);
        if(get_size(ptr_prior) == get_size_aligned(ptr_prior)){
            delete_from_linked_list(ptr_prior
                                   , get_offset(get_size_aligned(ptr_prior)));
            get_size(ptr_prior) += get_size(bp);
            set_value(get_footer_p(ptr_prior)
                     , get_size(ptr_prior));
            bp = ptr_prior;
        }
    }else{
        /* normal cases */
        void* ptr_next = get_next_blk_p(bp);
        if(get_size(ptr_next) == get_size_aligned(ptr_next)){
            delete_from_linked_list(ptr_next
                                   , get_offset(get_size_aligned(ptr_next)));
            get_size(bp) += get_size_aligned(ptr_next);
            set_value(get_footer_p(bp)
                     , get_size(bp));
        }
        /* buggy, and i don't know why <- fixed now(04/12)*/
        void* ptr_prior = get_prior_blk_p(bp);
        if(get_size(ptr_prior) == get_size_aligned(ptr_prior)){
            delete_from_linked_list(ptr_prior
                                   , get_offset(get_size_aligned(ptr_prior)));
            get_size(ptr_prior) += get_size(bp);
            set_value(get_footer_p(ptr_prior)
                     , get_size(ptr_prior));
            bp = ptr_prior;
        }
    }
    add_to_linked_list(bp, get_size_aligned(bp));
    return bp;
}

/*
 * Return whether the pointer is in the heap.
 * May be useful for debugging.
 */
static int in_heap(const void *p) {
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}

/*
 * Return whether the pointer is aligned.
 * May be useful for debugging.
 */
static int aligned(const void *p) {
    return (size_t)ALIGN(p) == (size_t)p;
}

/*
 * mm_checkheap
 */
void mm_checkheap(int lineno) {
    /* 04/12 first version */
    dbg();
    lineno = lineno;
    return ;
}
