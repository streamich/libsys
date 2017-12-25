const libsys = require('..');

const STDOUT = 1;
const isMac = process.platform === 'darwin';
const SYS_write = isMac ? 0x2000004 : 1;

const buf = Buffer.from('Hello world\n');

libsys.syscall(SYS_write, STDOUT, buf, buf.length);
