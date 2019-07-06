#include <inttypes.h>
#include <pthread.h>
#include <assert.h>
#include "../atomics/atomics.h"
#include "../syscall/syscall.h"
#include "../call/call.h"
#include <stdatomic.h>

#define DEBUG 0
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

typedef int32_t lock_t;
typedef int8_t type_t;
typedef int8_t len_t;
typedef int8_t ready_t;
typedef atomic_char left_t;
typedef struct async_call_headers async_call_headers;
typedef struct async_call_record async_call_record;
typedef struct worker_start_record worker_start_record;

const lock_t LOCK_BLOCKED = -1;     // When lock is -1, record is not meant for processing.
const lock_t LOCK_FREE = 0;         // When lock is 0, it is free to be picked up by one of the threads.
const type_t TYPE_SYSCALL = 0;      // Record specifies a syscall to be executed.
const type_t TYPE_CALL = 1;         // Record specifies a regular function call to be executed.
const type_t TYPE_EXIT = 2;         // Tells thread to quit.

struct async_call_headers {
    lock_t lock;                    // Used for CMPXCHG, which is supposed to be fastest on 32 bits. Workers exchange this value for their ID.
    type_t type;                    // Type of the async operation.
    len_t len;                      // Number of arguments in the record.
    ready_t ready;                  // Whether `next` address is ready, this is needed because JavaScript can write into `next` field only in 32-bit chunks
    left_t left;                    // Number of threads visited and left this block, needed to know when we can release memory.
    async_call_record* next;        // Address of the next record header.
    int64_t arg;
};

struct async_call_record {
    struct async_call_headers headers;
    int64_t args[];
};

struct worker_start_record {
    int32_t worker;
    struct async_call_record* record;
};

inline int64_t worker_exec_syscall (int32_t worker, async_call_record* record) {
    debug_print("~> worker_exec_syscall [worker = %x, syscall = %lu, len = %x] \n", worker, record->headers.arg, record->headers.len);
    int64_t result;
    switch (record->headers.len) {
        case 0: return syscall_0(record->headers.arg);
        case 1: return syscall_1(record->headers.arg, record->args[0]);
        case 2: return syscall_2(record->headers.arg, record->args[0], record->args[1]);
        case 3: return syscall_3(record->headers.arg, record->args[0], record->args[1], record->args[2]);
        case 4: return syscall_4(record->headers.arg, record->args[0], record->args[1], record->args[2], record->args[3]);
        case 5: return syscall_5(record->headers.arg, record->args[0], record->args[1], record->args[2], record->args[3], record->args[4]);
        case 6: return syscall_6(record->headers.arg, record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5]);
        default: return -1;
    }
}

int64_t worker_exec_call (int32_t worker, async_call_record* record) {
    debug_print("~> worker_exec_call [worker = %x, address = %lu, len = %x] \n", worker, record->headers.arg, record->headers.len);
    int64_t result;
    switch (record->headers.len) {
        case 0: return ((callback) record->headers.arg)();
        case 1: return ((callback1) record->headers.arg)(record->args[0]);
        case 2: return ((callback2) record->headers.arg)(record->args[0], record->args[1]);
        case 3: return ((callback3) record->headers.arg)(record->args[0], record->args[1], record->args[2]);
        case 4: return ((callback4) record->headers.arg)(record->args[0], record->args[1], record->args[2], record->args[3]);
        case 5: return ((callback5) record->headers.arg)(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4]);
        case 6: return ((callback6) record->headers.arg)(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5]);
        case 7: return ((callback7) record->headers.arg)(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5], record->args[6]);
        case 8: return ((callback8) record->headers.arg)(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5], record->args[6], record->args[7]);
        case 9: return ((callback9) record->headers.arg)(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5], record->args[6], record->args[7], record->args[8]);
        case 10: return ((callback10) record->headers.arg)(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5], record->args[6], record->args[7], record->args[8], record->args[9]);
        default: return -1;
    }
}

inline void* worker_process_new_block (int32_t worker, async_call_record* record) {
start:
    debug_print("~> start [worker = %x, lock = %x]\n", worker, record->headers.lock);
    if (record->headers.lock == LOCK_FREE) {
        cmpxchg32(&(record->headers.lock), LOCK_FREE, worker);
        if (record->headers.lock == worker) goto process_block;
        else goto look_for_next_block;
    }

look_for_next_block:
    if (record->headers.ready == 0) goto sleep;
    ++record->headers.left;
    record = record->headers.next;
    goto start;

process_block:
    debug_print("~> process_block [worker = %x]\n", worker);
    switch (record->headers.type) {
        case TYPE_SYSCALL:
            record->headers.arg = worker_exec_syscall(worker, record);
            debug_print("   syscall result: [worker = %x, result = %ld] \n", worker, record->headers.arg);
            break;
        case TYPE_CALL:
            record->headers.arg = worker_exec_call(worker, record);
            debug_print("   call result: [worker = %x, result = %ld] \n", worker, record->headers.arg);
            break;
        case TYPE_EXIT:
            goto exit;
            break;
    }
    goto look_for_next_block;

sleep:
    // debug_print("Waiting for next block [worker = %i]\n", worker);
    if (record->headers.type == TYPE_EXIT) goto exit;
    pthread_yield_np();
    goto look_for_next_block;

exit:
    ++record->headers.left;
    debug_print("Exiting [worker = %x] \n", worker);
    pthread_exit(NULL);
}

void* worker_start (void* arg) {
    worker_start_record* start_record = (worker_start_record*) arg;
    assert(start_record->worker != 0); // Worker ID cannot be 0, because it is LOCK_FREE.
    worker_process_new_block(start_record->worker, start_record->record);
}

int create_async_pool (void* initial_record, uint32_t nthreads) {
    for (; nthreads > 0; nthreads--) {
        pthread_t* thread = (pthread_t*) malloc(sizeof(pthread_t));
        pthread_attr_t* attr = (pthread_attr_t*) malloc(sizeof(pthread_attr_t));
        pthread_attr_init(attr);
        worker_start_record* record = (worker_start_record*) malloc(sizeof(worker_start_record));
        record->worker = nthreads;
        record->record = (struct async_call_record*) initial_record;
        pthread_create(thread, attr, worker_start, record);
        debug_print("created thread [worker = %x]\n", record->worker);
    }
    return 25;
}
