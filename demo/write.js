const addon = require('../build/Release/sys.node');

const STDOUT = 1;
const isMac = process.platform === 'darwin';
const SYS_write = isMac ? 0x2000004 : 1;

const buf = Buffer.from('foobar\n');

addon.syscall(SYS_write, STDOUT, buf, buf.length);
