# `syscall` for node.js

[![][npm-badge]][npm-url] [![][travis-badge]][travis-url]

Execute POSIX/Linux `syscall` command from Node.js.

## Installation

    npm i libsys

Compiles on Linux, Mac and Windows in WSL process.

## Usage

See

  - [Linux syscalls](https://filippo.io/linux-syscall-table/)
  - [Mac syscalls](https://opensource.apple.com/source/xnu/xnu-1504.3.12/bsd/kern/syscalls.master)

```js
var syscall = require('libsys').syscall;

// Print `Hello world` in console using kernel's `syscall` command.
var SYS_write = process.platform === 'darwin';
var buf = new Buffer('Hello world\n');
syscall(SYS_write, 1, buf, buf.length);
```

See [libjs](http://www.npmjs.com/package/libjs) for POSIX command implementation.

Part of [jskernel](http://www.github.com/streamich/jskernel) project.

## Reference

### `syscall` and `syscall64`

```ts
syscall(command: number, ...args: Targ[]): number;
syscall64(command: number, ...args: TArg[]): [number, number];
```

`syscall` accepts up to 6 command arguments `args`. See discussion on *Arguments*
below to see how JavaScript objects are converted to 64-bit integers.

`syscall` returns a `number` which is the result returned by the kernel,
negative numbers usually represent an error. If sytem call returns -1, the
C `errno` variable will be returned, which usually has more information about the error.

### `errno`

Returns `errno` variable.

```ts
function errno(): number;
```

### `malloc`

`malloc` returns an `ArrayBuffer` object of size `size` that is mapped to memory location
specified in `addr` argument.

```ts
malloc(addr: Targ, size: number): ArrayBuffer;
```

### `addressBuffer` and `addressBuffer64`

Return memory address of `Buffer`'s data contents.

```ts
addressBuffer(buffer: Buffer): number;
addressBuffer64(buffer: Buffer): [number, number];
```

`addr64` returns a tuple which represents 64-bit number, where first element contains the lower
32 bits and second element has the high 32 bits.

### `addressArrayBuffer` and `addressArrayBuffer64`

Returns `ArrayBuffer` address.

### `addressTypedArray` and `addressTypedArray64`

Returns `Uint8Array` address.

### `call`

Execute machine code at specified memory address. The memory address is converted
to function pointer and called using your architecture calling conventions.

Up to ten arguments in arguments array supported.

```ts
call(addr: Targ, offset: number = 0, args?: number[]);
```

### Arguments

```ts
type Targ = number|[number, number]|[number, number, number]|string|ArrayBuffer|TypedArray|Buffer;
```

Different JavaScript objects can be used as C arguments. Here is how they are converted to 64-bit integers:

 - `number` is treated as 32-bit integer and gets extended to 64-bit integer;
 - `[number, number]` treated as a `[lo, hi]` tuple of two 32-bit integers, which are combined to 64-bit integer;
 - `[number, number, number]` treated as a `[lo, hi, offset]` tuple, same as above with the difference that `offset` is added to the resulting 64-bit integer;
 - `string` gets converted to C null-terminated string and 64-bit pointer created to the beginning of that string;
 - `ArrayBuffer`, `TypedArray`, `Buffer` 64-bit pointer to the beginning of data contents of those objects is created;

## Building addon:

    npm run build

## License

[Unlicense](./LICENSE) - public domain.


[npm-url]: https://www.npmjs.com/package/libsys
[npm-badge]: https://img.shields.io/npm/v/libsys.svg
[travis-url]: https://travis-ci.org/streamich/libsys
[travis-badge]: https://travis-ci.org/streamich/libsys.svg?branch=master

