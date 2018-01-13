const libsys = require('..');
const {X64} = require('ass-js');

const asm = X64();
asm._('dd', process.termination_handler[1]);
asm._('dd', process.termination_handler[0]);
asm._('dq', 0);

const buf = asm.compile();

const isMac = process.platform === 'darwin';
const SYS_sigaction = isMac ? (0x2000000 + 46) : 13;
const SIGINT = 2;

const ab = libsys.frame(process.new_action, 16)
const b = Buffer.from(ab);

console.log(b);

// libsys.syscall(SYS_sigaction, SIGINT, buf, 0);

setTimeout(() => {
    console.log(process.new_action);
    console.log('TIMEOUT');
}, 2000);
