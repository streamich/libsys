#include <inttypes.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include "../atomics/atomics.h"
#include "../syscall/syscall.h"
#include "../call/call.h"

#define DEBUG 0
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

/* fix yield on OS X */
#ifdef __APPLE__
#define pthread_yield() pthread_yield_np()
#endif

typedef struct async_headers async_headers;
typedef struct async_syscall async_syscall;
typedef struct async_call async_call;
typedef struct worker_start_record worker_start_record;

struct async_headers {
    int8_t lock;                    // Used for CMPXCHG, which is supposed to be fastest on 32 bits. Workers exchange this value for their ID.
    int8_t type;                    // Type of the async operation.
    int8_t ready;                   // Whether it is safe to follow `next` pointer to the next async record.
    int8_t left;                    // Whether `next` address is ready, this is needed because JavaScript can write into `next` field only in 32-bit chunks
    uint32_t id;                    // ID of this record, used to identify this record by parent thread.
    uint64_t next;                  // Address of the next record header.
};

struct async_syscall {
    struct async_headers headers;
    uint32_t sys;                   // Syscall number.
    uint32_t len;                   // Number of arguments `args` for this syscall.
    int64_t args[];
};

struct async_call {
    struct async_headers headers;
    uint64_t addr;                  // Address of the function to be called.
    uint64_t len;                   // Number of arguments `args` for this function.
    int64_t args[];
};

struct async_result {
    struct async_headers headers;
    int64_t result;
};

struct worker_start_record {
    int32_t worker;                 // Worker ID, starts from 1.
    int32_t pipe_fd;                // One end of `pipe()` file descriptor, used to notify parent thread about processed records.
    struct async_headers* headers;
};

const int8_t LOCK_BLOCKED = -1;     // When lock is -1, record is not meant for processing.
const int8_t LOCK_FREE = 0;         // When lock is 0, it is free to be picked up by one of the threads.
const int8_t TYPE_SYSCALL = 0;      // Record specifies a syscall to be executed.
const int8_t TYPE_CALL = 1;         // Record specifies a regular function call to be executed.
const int8_t TYPE_EXIT = 2;         // Tells thread to quit.

inline int64_t worker_exec_syscall (int32_t worker, async_syscall* record) {
    debug_print("~> worker_exec_syscall [worker = %x, syscall = %u, len = %x] \n", worker, record->sys, record->len);
    switch (record->len) {
        case 0: return syscall_0(record->sys);
        case 1: return syscall_1(record->sys, record->args[0]);
        case 2: return syscall_2(record->sys, record->args[0], record->args[1]);
        case 3: return syscall_3(record->sys, record->args[0], record->args[1], record->args[2]);
        case 4: return syscall_4(record->sys, record->args[0], record->args[1], record->args[2], record->args[3]);
        case 5: return syscall_5(record->sys, record->args[0], record->args[1], record->args[2], record->args[3], record->args[4]);
        case 6: return syscall_6(record->sys, record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5]);
        default: return -1;
    }
}

int64_t worker_exec_call (int32_t worker, async_call* record) {
    debug_print("~> worker_exec_call [worker = %x, address = %llu, len = %llx] \n", worker, record->addr, record->len);
    switch (record->len) {
        case 0: return ((callback) record->addr)();
        case 1: return ((callback1) record->addr)(record->args[0]);
        case 2: return ((callback2) record->addr)(record->args[0], record->args[1]);
        case 3: return ((callback3) record->addr)(record->args[0], record->args[1], record->args[2]);
        case 4: return ((callback4) record->addr)(record->args[0], record->args[1], record->args[2], record->args[3]);
        case 5: return ((callback5) record->addr)(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4]);
        case 6: return ((callback6) record->addr)(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5]);
        case 7: return ((callback7) record->addr)(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5], record->args[6]);
        case 8: return ((callback8) record->addr)(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5], record->args[6], record->args[7]);
        case 9: return ((callback9) record->addr)(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5], record->args[6], record->args[7], record->args[8]);
        case 10: return ((callback10) record->addr)(record->args[0], record->args[1], record->args[2], record->args[3], record->args[4], record->args[5], record->args[6], record->args[7], record->args[8], record->args[9]);
        default: return -1;
    }
}

inline void* worker_process_new_block (int32_t worker, int32_t pipe_fd, async_headers* headers) {
start:
    debug_print("~> start [worker = %x, lock = %x]\n", worker, headers->lock);
    if (headers->lock == LOCK_FREE) {
        cmpxchg8(&(headers->lock), LOCK_FREE, worker);
        if (headers->lock == worker) goto process_block;
        else goto look_for_next_block;
    }

look_for_next_block:
    if (headers->ready == 0) goto sleep;
    atomic_increment(&(headers->left), 1);
    headers = (async_headers*) headers->next;
    goto start;

process_block:
    debug_print("~> process_block [worker = %x]\n", worker);
    switch (headers->type) {
        case TYPE_SYSCALL:
            ((async_result*) headers)->result = worker_exec_syscall(worker, (async_syscall*) headers);
            write(pipe_fd, &(headers->id), sizeof(headers->id));
            debug_print("   syscall result: [worker = %x, result = %lld] \n", worker, ((async_result*) headers)->result);
            break;
        case TYPE_CALL:
            ((async_result*) headers)->result = worker_exec_call(worker, (async_call*) headers);
            write(pipe_fd, &(headers->id), sizeof(headers->id));
            debug_print("   call result: [worker = %x, result = %lld] \n", worker, ((async_result*) headers)->result);
            break;
        case TYPE_EXIT:
            goto exit;
            break;
    }
    goto look_for_next_block;

sleep:
    // debug_print("Waiting for next block [worker = %i]\n", worker);
    if (headers->type == TYPE_EXIT) goto exit;
    pthread_yield();
    goto look_for_next_block;

exit:
    atomic_increment(&(headers->left), 1);
    debug_print("Exiting [worker = %x] \n", worker);
    pthread_exit(NULL);
}

void* worker_start (void* arg) {
    worker_start_record* start_record = (worker_start_record*) arg;
    assert(start_record->worker != 0); // Worker ID cannot be 0, because it is LOCK_FREE.
    worker_process_new_block(start_record->worker, start_record->pipe_fd, start_record->headers);
    return NULL;
}

int32_t create_async_pool (void* headers, uint32_t nthreads) {
    int32_t fds[2];
    pipe(fds);
    for (; nthreads > 0; nthreads--) {
        pthread_t* thread = (pthread_t*) malloc(sizeof(pthread_t));
        pthread_attr_t* attr = (pthread_attr_t*) malloc(sizeof(pthread_attr_t));
        pthread_attr_init(attr);
        worker_start_record* record = (worker_start_record*) malloc(sizeof(worker_start_record));
        record->worker = nthreads;
        record->pipe_fd = fds[1];
        record->headers = (async_headers*) headers;
        pthread_create(thread, attr, worker_start, record);
        debug_print("created thread [worker = %x]\n", record->worker);
    }
    return fds[0];
}
