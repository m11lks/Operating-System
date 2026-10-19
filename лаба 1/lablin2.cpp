#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <iostream>
#include <vector>
#include <string>
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <atomic>

#define FIXED_BLOCK_SIZE 4096

struct aio_operation {
    struct aiocb aio;
    char *buffer;
    int write_operation;
    int fd_read;
    int fd_write;
    size_t block_size;
    off_t file_size;
    std::atomic<off_t> *p_offset;
    std::atomic<int> *active_ops;
};

void aio_completion_handler(sigval_t sigval)
{
    struct aio_operation *op = (struct aio_operation *)sigval.sival_ptr;
    int ret = aio_return(&op->aio);

    if (ret <= 0) {
        op->active_ops->fetch_sub(1);
        return;
    }

    if (op->write_operation == 0) {
        op->write_operation = 1;
        op->aio.aio_fildes = op->fd_write;
        op->aio.aio_nbytes = ret;
        
        if (aio_write(&op->aio) == -1) {
            op->active_ops->fetch_sub(1);
        }
    } else {
        off_t current_pos = op->p_offset->fetch_add(op->block_size);

        if (current_pos >= op->file_size) {
            op->active_ops->fetch_sub(1);
            return;
        }

        op->write_operation = 0;
        op->aio.aio_fildes = op->fd_read;
        op->aio.aio_offset = current_pos;
        op->aio.aio_nbytes = (op->file_size - current_pos < (off_t)op->block_size) 
                             ? (op->file_size - current_pos) : (off_t)op->block_size;
        
        op->aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
        op->aio.aio_sigevent.sigev_notify_function = aio_completion_handler;
        op->aio.aio_sigevent.sigev_value.sival_ptr = op;

        if (aio_read(&op->aio) == -1) {
            op->active_ops->fetch_sub(1);
        }
    }
}

void blc_size_test(std::string src, std::string dst, size_t block_size)
{
    int fd_in = open(src.c_str(), O_RDONLY | O_NONBLOCK, 0666);
    int fd_out = open(dst.c_str(), O_CREAT | O_WRONLY | O_TRUNC | O_NONBLOCK, 0666);

    if (fd_in < 0 || fd_out < 0) return;

    struct stat st;
    fstat(fd_in, &st);
    off_t file_size = st.st_size;

    std::atomic<off_t> offset(0);
    std::atomic<int> active(0);
    struct aio_operation op;

    op.buffer = (char*)malloc(block_size);
    op.fd_read = fd_in;
    op.fd_write = fd_out;
    op.block_size = block_size;
    op.file_size = file_size;
    op.write_operation = 0;
    op.p_offset = &offset;
    op.active_ops = &active;

    off_t start_pos = offset.fetch_add(block_size);

    memset(&op.aio, 0, sizeof(struct aiocb));
    op.aio.aio_fildes = fd_in;
    op.aio.aio_offset = start_pos;
    op.aio.aio_buf = op.buffer;
    op.aio.aio_nbytes = (file_size - start_pos < (off_t)block_size) ? (file_size - start_pos) : (off_t)block_size;
    op.aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
    op.aio.aio_sigevent.sigev_notify_function = aio_completion_handler;
    op.aio.aio_sigevent.sigev_value.sival_ptr = &op;

    active.store(1);
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    aio_read(&op.aio);
    while (active.load() > 0) {
        usleep(10000);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    std::cout << "Time: " << t << "s, Speed: " << (file_size / 1024.0 / 1024.0) / t << " MB/s" << std::endl;

    free(op.buffer);
    close(fd_in);
    close(fd_out);
}

void numop_test(std::string src, std::string dst, size_t block_size, int num_ops)
{
    int fd_in = open(src.c_str(), O_RDONLY | O_NONBLOCK, 0666);
    int fd_out = open(dst.c_str(), O_CREAT | O_WRONLY | O_TRUNC | O_NONBLOCK, 0666);
    
    if (fd_in < 0 || fd_out < 0) return;

    struct stat st;
    fstat(fd_in, &st);
    off_t file_size = st.st_size;

    std::atomic<off_t> offset(0);
    std::atomic<int> active(0);
    struct aio_operation *ops[16];

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < num_ops; i++) {
        off_t start_pos = offset.fetch_add(block_size);
        if (start_pos >= file_size) break;

        ops[i] = (struct aio_operation*)malloc(sizeof(struct aio_operation));
        ops[i]->buffer = (char*)malloc(block_size);
        ops[i]->fd_read = fd_in;
        ops[i]->fd_write = fd_out;
        ops[i]->block_size = block_size;
        ops[i]->file_size = file_size;
        ops[i]->write_operation = 0;
        ops[i]->p_offset = &offset;
        ops[i]->active_ops = &active;

        memset(&ops[i]->aio, 0, sizeof(struct aiocb));
        ops[i]->aio.aio_fildes = fd_in;
        ops[i]->aio.aio_offset = start_pos;
        ops[i]->aio.aio_buf = ops[i]->buffer;
        ops[i]->aio.aio_nbytes = (file_size - start_pos < (off_t)block_size) ? (file_size - start_pos) : (off_t)block_size;
        ops[i]->aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
        ops[i]->aio.aio_sigevent.sigev_notify_function = aio_completion_handler;
        ops[i]->aio.aio_sigevent.sigev_value.sival_ptr = ops[i];

        active.fetch_add(1);
        aio_read(&ops[i]->aio);
    }

    while (active.load() > 0) {
        usleep(10000);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    std::cout << "Time: " << t << "s, Speed: " << (file_size / 1024.0 / 1024.0) / t << " MB/s" << std::endl;

    for (int i = 0; i < num_ops; i++) {
        if (ops[i]) {
            free(ops[i]->buffer);
            free(ops[i]);
        }
    }
    close(fd_in);
    close(fd_out);
}

int main()
{
    int choice;
    int n;
    std::string src, dst;
    size_t block_size;

    while (true) {
        std::cout << "\nMenu" << std::endl;
        std::cout << "1. Block size test" << std::endl;
        std::cout << "2. Num operations test" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Choice: ";
        if (!(std::cin >> choice)) break;
        if (choice == 3) break;

        std::cout << "Source file: ";
        std::cin >> src;
        std::cout << "Dest: ";
        std::cin >> dst;

        if (choice == 1) {
            std::cout << "Block size (bytes): ";
            std::cin >> block_size;
            blc_size_test(src, dst, block_size);
        } else if (choice == 2) {
            std::cout << "Num operations (1, 2, 4, 8, 12, 16): ";
            std::cin >> n;
            numop_test(src, dst, FIXED_BLOCK_SIZE, n);
        }
    }

    return 0;
}