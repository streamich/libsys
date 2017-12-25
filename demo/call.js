const libsys = require('..');
const {rax, rdi, rsi} = require('ass-js/lib/x86/operand');
const Code = require('ass-js/lib/x86/x64/code').Code;

const isMac = process.platform === 'darwin';
const SYS_mmap = isMac ? (0x2000000 + 197) : 9;

// Allocate executable memory, returns `ArrayBuffer`.
function alloc(size) {
    const flags = 2 /* MAP_PRIVATE */ | (isMac ? 4096 : 32 /* MAP_ANONYMOUS */);
    const protnum = 1 | 2 | 4; // Read, write and execute;
    const addr = libsys.syscall64(SYS_mmap, 0, size, protnum, flags, -1, 0);
    return libsys.frame(addr, size);
}

var _ = new Code;

_._('mov', [rax, -9]);
_._('ret');

const code = _.compile();
const ab = alloc(code.length);
const uint8 = new Uint8Array(ab);

for (let i = 0; i < code.length; i++) uint8[i] = code[i];

const result = libsys.call(ab, 0, []);

console.log('result', result);
