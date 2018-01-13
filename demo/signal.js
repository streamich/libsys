const libsys = require('..');
const {X64} = require('ass-js');

const isMac = process.platform === 'darwin';
const SYS_mmap = isMac ? (0x2000000 + 197) : 9;
const SYS_write = isMac ? 0x2000004 : 1;
const STDOUT = 1;

// Allocate executable memory, returns `ArrayBuffer`.
function alloc(size) {
    const flags = 2 /* MAP_PRIVATE */ | (isMac ? 4096 : 32 /* MAP_ANONYMOUS */);
    const protnum = 1 | 2 | 4; // Read, write and execute;
    const addr = libsys.syscall64(SYS_mmap, 0, size, protnum, flags, -1, 0);
    return libsys.frame(addr, size);
}

const asm = X64();

const tpl = _ => {
    const str = _.lbl('string');

    // .text
    _('push', 'rdi');
    _('push', 'rsi');
    _('push', 'rdx');

    _('mov', ['rax', SYS_write], 64);
    _('mov', ['rdi', STDOUT], 64);
    _('lea', ['rsi', _('rip').disp(str)], 64);
    _('mov', ['rdx', 14], 64);
    _('syscall');

    _('pop', 'rdx');
    _('pop', 'rsi');
    _('pop', 'rdi');
    _('ret');

    // .data
    _.insert(str);
    _('db', 'Hello World!\n');
};

asm.code(tpl);
// console.log(String(asm));
const code = asm.compile([]);
console.log(String(asm));

const ab = alloc(code.length);
const uint8 = new Uint8Array(ab);

for (let i = 0; i < code.length; i++) uint8[i] = code[i];

console.log('MY PID IS: ', process.pid);

setTimeout(() => {
    let result = libsys.call(ab, 0, []);
    console.log('DONE', result);
}, 3000);
