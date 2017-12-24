const addon = require('../build/Release/sys.node');
const rax = require('ass-js/lib/x86/operand').rax;
const Code = require('ass-js/lib/x86/x64/code').Code;

const isMac = process.platform === 'darwin';
const SYS_mmap = isMac ? 197 : 9;

// Allocate executable memory, returns `ArrayBuffer`.
function alloc(size) {
  const flags = 2 /* MAP_PRIVATE */ | (isMac ? 4096 : 32 /* MAP_ANONYMOUS */);
  const protnum = 1 | 2 | 4; // Read, write and execute;
  const addr = addon.syscall64(SYS_mmap, 0, size, protnum, flags, -1, 0);
  return addon.malloc(addr, size);
}

describe('address', function() {
    describe('.addressArrayBuffer64()', function() {
        it('Returns 2-tuple representing a pointer', function() {
            const ab = new ArrayBuffer(10);
            const tuple = addon.addressArrayBuffer64(ab);

            expect(Array.isArray(tuple)).toBe(true);
            expect(tuple.length).toBe(2);
            expect(typeof tuple[0]).toBe('number');
            expect(typeof tuple[1]).toBe('number');
        });
    });
});
