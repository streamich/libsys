const addon = require('../build/Release/sys.node');

const STDOUT = 1;
const isMac = process.platform === 'darwin';
const SYS_write = isMac ? 0x2000004 : 1;
const SYS_getpid = isMac ? (0x2000000 + 20) : 39;

// Allocate executable memory, returns `ArrayBuffer`.
function alloc(size) {
  const flags = 2 /* MAP_PRIVATE */ | (isMac ? 4096 : 32 /* MAP_ANONYMOUS */);
  const protnum = 1 | 2 | 4; // Read, write and execute;
  const addr = addon.syscall64(SYS_mmap, 0, size, protnum, flags, -1, 0);
  return addon.malloc(addr, size);
}

describe('syscall', function() {
    describe('.syscall()', function() {
        it('Writes buffer to STDOUT', function() {
            const str = 'Hello world, hell yeah!';
            const buf = new Buffer(str + '\n');
            const res = addon.syscall(SYS_write, STDOUT, buf, buf.length);

            expect(res).toBe(buf.length);
        });

        it('gets correct process ID', () => {
            const res = addon.syscall(SYS_getpid);

            expect(res).toBe(process.pid);
        });
    });

    describe('.syscall64()', function() {
        it('Writes buffer to STDOUT', function() {
            const str = 'Hello world, hell yeah!';
            const buf = new Buffer(str + '\n');
            const [length, zero] = addon.syscall64(SYS_write, STDOUT, buf, buf.length);

            expect(length).toBe(buf.length);
            expect(zero).toBe(0);
        });

        it('gets correct process ID', () => {
            const [pid, zero] = addon.syscall64(SYS_getpid);

            expect(pid).toBe(process.pid);
            expect(zero).toBe(0);
        });
    });

    describe('.syscall_0()', () => {
        it('gets correct process ID', () => {
            const res = addon.syscall_0(SYS_getpid);

            expect(res).toBe(process.pid);
        });
    });

    describe('.syscall_1()', () => {
        it('gets correct process ID', () => {
            const res = addon.syscall_1(SYS_getpid, 0);

            expect(res).toBe(process.pid);
        });
    });

    describe('.syscall_2()', () => {
        it('gets correct process ID', () => {
            const res = addon.syscall_2(SYS_getpid, 0, 0);

            expect(res).toBe(process.pid);
        });
    });

    describe('.syscall_3()', () => {
        it('gets correct process ID', () => {
            const res = addon.syscall_3(SYS_getpid, 0, 0, 0);

            expect(res).toBe(process.pid);
        });
    });

    describe('.syscall_4()', () => {
        it('gets correct process ID', () => {
            const res = addon.syscall_4(SYS_getpid, 0, 0, 0, 0);

            expect(res).toBe(process.pid);
        });
    });

    describe('.syscall_5()', () => {
        it('gets correct process ID', () => {
            const res = addon.syscall_5(SYS_getpid, 0, 0, 0, 0, 0);

            expect(res).toBe(process.pid);
        });
    });

    describe('.syscall_6()', () => {
        it('gets correct process ID', () => {
            const res = addon.syscall_5(SYS_getpid, 0, 0, 0, 0, 0, 0);

            expect(res).toBe(process.pid);
        });
    });

    describe('.syscall64_0()', () => {
        it('gets correct process ID', () => {
            const [pid, zero] = addon.syscall64_0(SYS_getpid);

            expect(pid).toBe(process.pid);
            expect(zero).toBe(0);
        });
    });

    describe('.syscall64_1()', () => {
        it('gets correct process ID', () => {
            const [pid, zero] = addon.syscall64_1(SYS_getpid);

            expect(pid).toBe(process.pid);
            expect(zero).toBe(0);
        });
    });

    describe('.syscall64_2()', () => {
        it('gets correct process ID', () => {
            const [pid, zero] = addon.syscall64_2(SYS_getpid);

            expect(pid).toBe(process.pid);
            expect(zero).toBe(0);
        });
    });

    describe('.syscall64_3()', () => {
        it('gets correct process ID', () => {
            const [pid, zero] = addon.syscall64_3(SYS_getpid);

            expect(pid).toBe(process.pid);
            expect(zero).toBe(0);
        });
    });

    describe('.syscall64_4()', () => {
        it('gets correct process ID', () => {
            const [pid, zero] = addon.syscall64_4(SYS_getpid);

            expect(pid).toBe(process.pid);
            expect(zero).toBe(0);
        });
    });

    describe('.syscall64_5()', () => {
        it('gets correct process ID', () => {
            const [pid, zero] = addon.syscall64_5(SYS_getpid);

            expect(pid).toBe(process.pid);
            expect(zero).toBe(0);
        });
    });

    describe('.syscall64_6()', () => {
        it('gets correct process ID', () => {
            const [pid, zero] = addon.syscall64_6(SYS_getpid);

            expect(pid).toBe(process.pid);
            expect(zero).toBe(0);
        });
    });
});
