#include <inttypes.h>
#include <pthread.h>
#include <assert.h>
#include "../atomics/atomics.h"
#include "../syscall/syscall.h"

#define DEBUG 1
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

const int32_t LOCK_BLOCKED = -1;    // When lock is -1, record is not meant for processing.
const int32_t LOCK_FREE = 0;        // When lock is 0, it is free to be picked up by one of the threads.
const int16_t TYPE_SYSCALL = 0;     // Record specifies a syscall to be executed.
const int16_t TYPE_CALL = 1;        // Record specifies a regular function call to be executed.

typedef struct async_call_headers async_call_headers;
typedef struct async_call_record async_call_record;
typedef struct worker_start_record worker_start_record;

struct async_call_headers {
    int32_t lock;                   // Used for CMPXCHG, which is supposed to be fastest on 32 bits. Workers exchange this value for their ID.
    int16_t type;                   // Type of the async operation.
    int16_t len;                    // Number of arguments in the record.
    async_call_record* next;        // Address of the next record header.
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
    printf("Executing syscall.\n");
    return 13;
    /*
    int64_t result;
    switch (record->headers.len) {
        case 1: return syscall_0(record->args[0]);
        case 2: return syscall_1(record->args[0], record->args[1]);
        case 3: return syscall_2(record->args[0], record->args[1], record->args[2]);
        case 4: return syscall_3(record->args[0], record->args[1], record->args[2], record->args[3]);
        case 5: return syscall_4(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4]);
        case 6: return syscall_5(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5]);
        case 7: return syscall_6(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5], record->args[6]);
    }
    return -1;
    */
}

inline void* worker_process_new_block (int32_t worker, async_call_record* record) {
start:
    debug_print("~> start [worker = %i, lock = %i]\n", worker, record->headers.lock);
    if (record->headers.lock == LOCK_FREE) {
        cmpxchg32(&(record->headers.lock), LOCK_FREE, worker);
        if (record->headers.lock == worker) goto process_block;
        else goto look_for_next_block;
    }

look_for_next_block:
    if (record->headers.next == 0) goto sleep;
    record = record->headers.next;
    goto start;

process_block:
    debug_print("~> process_block [worker = %i]\n", worker);
    switch (record->headers.type) {
        case TYPE_SYSCALL: record->args[0] = worker_exec_syscall(worker, record); break;
    }
    goto look_for_next_block;

sleep:
    // debug_print("Waiting for next block [worker = %i]\n", worker);
    pthread_yield_np();
    goto look_for_next_block;
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
        debug_print("created thread [worker = %i]\n", record->worker);
    }
    return 25;
}
