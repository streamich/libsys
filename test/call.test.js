const libsys = require('..');
const {X64} = require('ass-js');

const isMac = process.platform === 'darwin';
const SYS_mmap = isMac ? (0x2000000 + 197) : 9;

// Allocate executable memory, returns `ArrayBuffer`.
function alloc(size) {
  const flags = 2 /* MAP_PRIVATE */ | (isMac ? 4096 : 32 /* MAP_ANONYMOUS */);
  const protnum = 1 | 2 | 4; // Read, write and execute;
  const addr = libsys.syscall64(SYS_mmap, 0, size, protnum, flags, -1, 0);
  return libsys.frame(addr, size);
}

describe('libsys', function() {
    describe('.call()', function () {
        it('calls executable code', () => {
            var _ = X64();

            _._('mov', ['rax', 0xBABE]);
            _._('ret');

            const code = _.compile();
            const ab = alloc(code.length);
            const uint8 = new Uint8Array(ab);

            for (let i = 0; i < code.length; i++) uint8[i] = code[i];

            const result = libsys.call(ab, 0, []);

            expect(result).toBe(0xBABE);
        });

        it('add(a, b) = a + b', () => {
            var _ = X64();

            _._('add', ['rdi', 'rsi']);
            _._('mov', ['rax', 'rdi']);
            _._('ret');

            const code = _.compile();
            const ab = alloc(code.length);
            const uint8 = new Uint8Array(ab);

            for (let i = 0; i < code.length; i++) uint8[i] = code[i];

            const add = (a, b) => libsys.call(ab, 0, [a, b]);

            expect(add(1, 1)).toBe(2);
            expect(add(2, 2)).toBe(4);
            expect(add(1000, 999)).toBe(1999);
            expect(add(-5, -3)).toBe(-8);
            expect(add(-5, 10)).toBe(5);
        });
    });

    describe('.call64()', function () {
        it('calls executable code', () => {
            var _ = X64();

            _._('mov', ['rax', 0xBABE]);
            _._('ret');

            const code = _.compile();
            const ab = alloc(code.length);
            const uint8 = new Uint8Array(ab);

            for (let i = 0; i < code.length; i++) uint8[i] = code[i];

            const [result, zero] = libsys.call64(ab, 0, []);

            expect(result).toBe(0xBABE);
            expect(zero).toBe(0);
        });

        it('add(a, b) = a + b', () => {
            var _ = X64();

            _._('add', ['rdi', 'rsi']);
            _._('mov', ['rax', 'rdi']);
            _._('ret');

            const code = _.compile();
            const ab = alloc(code.length);
            const uint8 = new Uint8Array(ab);

            for (let i = 0; i < code.length; i++) uint8[i] = code[i];

            const add = (a, b) => libsys.call64(ab, 0, [a, b])[0];

            expect(add(1, 1)).toBe(2);
            expect(add(2, 2)).toBe(4);
            expect(add(1000, 999)).toBe(1999);
            expect(add(-5, -3)).toBe(-8);
            expect(add(-5, 10)).toBe(5);
        });
    });

    describe('.call_0()', function () {
        it('calls executable code', () => {
            var _ = X64();

            _._('mov', ['rax', 0xBABE]);
            _._('ret');

            const code = _.compile();
            const ab = alloc(code.length);
            const uint8 = new Uint8Array(ab);

            for (let i = 0; i < code.length; i++) uint8[i] = code[i];

            const result = libsys.call_0(ab);

            expect(result).toBe(0xBABE);
        });
    });

    describe('.call_1()', function () {
        it('calls executable code', () => {
            var _ = X64();

            _._('mov', ['rax', 'rdi']);
            _._('ret');

            const code = _.compile();
            const ab = alloc(code.length);
            const uint8 = new Uint8Array(ab);

            for (let i = 0; i < code.length; i++) uint8[i] = code[i];

            const result = libsys.call_1(ab, 0xBABE);

            expect(result).toBe(0xBABE);
        });
    });

    describe('.call64_0()', function () {
        it('calls executable code', () => {
            var _ = X64();

            _._('mov', ['rax', 0xBABE]);
            _._('ret');

            const code = _.compile();
            const ab = alloc(code.length);
            const uint8 = new Uint8Array(ab);

            for (let i = 0; i < code.length; i++) uint8[i] = code[i];

            const [result, zero] = libsys.call64_0(ab);

            expect(result).toBe(0xBABE);
            expect(zero).toBe(0);
        });
    });

    describe('.call64_1()', function () {
        it('calls executable code', () => {
            var _ = X64();

            _._('mov', ['rax', 'rdi']);
            _._('ret');

            const code = _.compile();
            const ab = alloc(code.length);
            const uint8 = new Uint8Array(ab);

            for (let i = 0; i < code.length; i++) uint8[i] = code[i];

            const [result, zero] = libsys.call64_1(ab, 0xBABE);

            expect(result).toBe(0xBABE);
            expect(zero).toBe(0);
        });
    });
});
