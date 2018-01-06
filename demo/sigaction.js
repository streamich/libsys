const libsys = require('..');
const {X64} = require('ass-js');

const isMac = process.platform === 'darwin';
const SYS_mmap = isMac      ? (0x2000000 + 197) : 9;
const SYS_sigaction = isMac ? (0x2000000 + 46) : 13;
const SYS_write = isMac ? 0x2000004 : 1;
const SYS_exit = isMac ? 0x2000001 : 60;
const SIGINT = 2;
const STDOUT = 1;

// Allocate executable memory, returns `ArrayBuffer`.
function alloc(size) {
    const flags = 2 /* MAP_PRIVATE */ | (isMac ? 4096 : 32 /* MAP_ANONYMOUS */);
    const protnum = 1 | 2 | 4; // Read, write and execute;
    const addr = libsys.syscall64(SYS_mmap, 0, size, protnum, flags, -1, 0);
    return libsys.frame(addr, size);
}

process.jumpers[33] = (a, b) => { console.log('JUMPER 33 CALLED', a, b); };

const code = (template) => {
    const asm = X64();
    asm.code(template);
    const code = asm.compile([]);
    console.log(String(asm));

    const ab = alloc(code.length);
    const uint8 = new Uint8Array(ab);

    for (let i = 0; i < code.length; i++) uint8[i] = code[i];

    return ab;
};

const abHello = code(_ => {
    const str = _.lbl('string');

    // .text
    // _('push', 'rdi');
    // _('push', 'rsi');
    // _('push', 'rdx');

    // _('mov', ['rax', SYS_write], 64);
    // _('mov', ['rdi', STDOUT], 64);
    // _('lea', ['rsi', _('rip').disp(str)], 64);
    // _('mov', ['rdx', 14], 64);
    // _('syscall');

    _('mov', ['rdi', 'rax']);
    _('mov', ['rax', SYS_exit]);
    _('syscall');

    // _('pop', 'rdx');
    // _('pop', 'rsi');
    // _('pop', 'rdi');
    // _('ret');

    // .data
    _.insert(str);
    _('db', 'Hello World!\n');
});

const abSigaction = code(_ => {
    const data = _.lbl('data');
    const old = _.lbl('old');
    const hello = _.lbl('hello');

    // .text
    _('push', 'rdi');
    _('push', 'rsi');
    _('push', 'rdx');

    // _('mov', ['rax', libsys.getAddress(abHello)]);
    // _('call', 'rax');
    // _('mov', ['rax', libsys.getAddress(abHello)]);
    // _('call', 'rax');

    _('mov', ['rax', SYS_sigaction], 64);
    _('mov', ['rdi', SIGINT], 64);
    _('lea', ['rsi', _('rip').disp(data)], 64);
    _('lea', ['rdx', _('rip').disp(old)], 64);
    _('syscall');

    _('pop', 'rdx');
    _('pop', 'rsi');
    _('pop', 'rdi');
    _('ret');

    // .data
    _.insert(data);
    // _('dq', [libsys.getAddress(abHello)]);
    _('dq', [process.nothing]);
    // _('dd', 0xFFFEFEFF);
    _('dd', 0);
    _('dd', 0);

    _.insert(old);
    _('dq', 0);
    _('dq', 0);
});

// 00 80 4e 02 01 00 00 00
// 00 00 00 00 00 00 00 00
// 89 00 a5 00 01 00 00 00
// ff fe fe ff 00 00 00 00

const ab = code(_ => {
    _('dq', [process.nothing]);
    _('dq', 0);
});
const buf = Buffer.from(ab);
const buf2 = Buffer.alloc(buf.length);
for (let i = 0; i < buf.length; i++) buf2[i] = buf[i];

libsys.call(process.nothing, 0, []);
process.exit();

libsys.syscall64(SYS_sigaction, SIGINT, 0, buf2);
buf2[8] = 0;
buf2[9] = 0;
buf2[10] = 0;
buf2[11] = 0;
buf2[12] = 0;
buf2[13] = 0;
buf2[14] = 0;
buf2[15] = 0;
console.log(buf2);
libsys.syscall64(SYS_sigaction, SIGINT, buf2, 0);
console.log(buf2);
libsys.syscall64(SYS_sigaction, SIGINT, 0, buf2);
console.log(buf2);

// process.exit();

// console.log('ADDR Hello: ', libsys.getAddress(abHello));
// console.log('MY PID IS: ', process.pid);
// let result = libsys.call(abSigaction, 0, []);
// console.log('result', result);

// console.log(Buffer.from(abSigaction).slice(20));

setTimeout(() => {
    console.log('END');
}, 5000);
