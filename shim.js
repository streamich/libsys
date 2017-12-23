// Adds system shims to `process` global.
const libsys = require('./libsys');

if (!process.syscall) process.syscall = libsys.syscall;
if (!process.syscall64) process.syscall64 = libsys.syscall64;
if (!process.errno) process.errno = libsys.errno;
if (!process.getAddress) process.getAddress = libsys.addressArrayBuffer64;
if (!process.frame) process.frame = libsys.malloc;
if (!process.call) process.call = libsys.call;
