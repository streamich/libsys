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

process.jumpers[22] = (a, b) => { console.log('lol, 22 called', a, b); };

const asm = X64();

const tpl = _ => {
    const address = _('label', 'jump_address');
    _('dq', [process.jumperAddress]);

    _('push', 'rdi');
    _('push', 'rsi');
    _('push', 'rdx');

    _('mov', ['rdi', 22], 64);
    _('mov', ['rsi', 12], 64);
    _('mov', ['rdx', 8], 64);

    _('call', [_('rip').disp(address)], 64);

    _('pop', 'rdx');
    _('pop', 'rsi');
    _('pop', 'rdi');
    _('ret');
};

asm.code(tpl);
console.log(String(asm));
const code = asm.compile([]);
console.log(String(asm));

const ab = alloc(code.length);
const uint8 = new Uint8Array(ab);

for (let i = 0; i < code.length; i++) uint8[i] = code[i];

let result = libsys.call(ab, 8, []);
console.log('result', result);

result = libsys.call(ab, 8, []);
console.log('result', result);


console.log('END');
