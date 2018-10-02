# libsys

[![][npm-badge]][npm-url] [![][travis-badge]][travis-url]

  - Execute [Linux](https://filippo.io/linux-syscall-table/)/[Mac](https://opensource.apple.com/source/xnu/xnu-1504.3.12/bsd/kern/syscalls.master) `syscall`s from Node.js
  - Get memory address of `Buffer`, `ArrayBuffer`, `TypedArray`
  - Call arbitrary machine code from Node.js
  - Create machine code trampolines that will call Node.js functions


## Installation

```shell
npm install libsys
```

Compiles on Linux, Mac and Windows ([in WSL process](https://docs.microsoft.com/en-us/windows/wsl/install-win10)).


## Usage

Print `Hello world` to console

```js
const libsys = require('libsys');

const STDOUT = 1;
const isMac = process.platform === 'darwin';
const SYS_write = isMac ? 0x2000004 : 1;
const buf = Buffer.from('Hello world\n');

libsys.syscall(SYS_write, STDOUT, buf, buf.length);
```


## More goodies

  - [libjs](http://www.github.com/streamich/libjs) &mdash; POSIX command implementation (`libc` in JavaScript).
  - [bamboo](http://www.github.com/streamich/bamboo) &mdash; Node.js clone in pure JavaScript.
  - [jskernel](http://www.github.com/streamich/jskernel) &mdash; proposal of Node in kernel space.
  - [Assembler.js](http://www.github.com/streamich/ass-js) &mdash; X86_64 assembler in JavaScript.


## Reference

  - [`syscall`](#syscall) &mdash; Executes system call with varied type arguments, returns 32-bit result
  - [`syscall64`](#syscall64) &mdash; Executes system call with varied type arguments, return 64-bit result
  - [`syscall_N`](#syscall_0) &mdash; Executes system call with N `number` arguments, returns 32-bit result
  - [`syscall64_N`](#syscall64_0) &mdash; Executes system call with N `number` arguments, returns 64-bit result
  - [`getAddressArrayBuffer`](#getaddressarraybuffer) &mdash; Returns 64-bit address of `ArrayBuffer`
  - [`getAddressTypedArray`](#getaddresstypedarray) &mdash; Returns 64-bit address of `TypedArray` (including `Uint8Array`, etc..)
  - [`getAddressBuffer`](#getaddressbuffer) &mdash; Returns 64-bit address of Node's `Buffer`
  - [`getAddress`](#getaddress) &mdash; Returns 64-bit address of any buffer type
  - [`frame`](#frame) &mdash; Creates `ArrayBuffer` in the specified memory location
  - [`call`](#call) &mdash; Calls machine code at specified address with up to 10 arguments, returns 32-bit result
  - [`call64`](#call64) &mdash; Calls machine code at specified address with up to 10 arguments, returns 64-bit result
  - [`call_0`](#call_0) &mdash; Call machine code with no arguments
  - `call_1`
  - `call64_0`
  - `call64_1`
  - `jumper`
  - `sigaction` - Executes `sigaction` system call

### Arguments

Different JavaScript types can be used as `Targ` argument in some functions. Here is how they are converted to 64-bit integers:

```ts
type Targ = number | [number, number] | [number, number, number] | string | ArrayBuffer | TypedArray | Buffer;
```

 - `number` is treated as 32-bit integer and gets extended to 64-bit integer;
 - `[number, number]` treated as a `[lo, hi]` tuple of two 32-bit integers, which are combined into 64-bit integer;
 - `[number, number, number]` treated as a `[lo, hi, offset]` tuple, same as above with the difference that `offset` is added to the resulting 64-bit integer;
 - `string` gets converted to C null-terminated string and 64-bit pointer created to the beginning of that string;
 - `ArrayBuffer`, `TypedArray`, `Buffer` 64-bit pointer to the beginning of data contents of those objects is created;


### `syscall`

```ts
syscall(command: number, ...args: Targ[]): number;
```

`syscall` accepts up to 6 command arguments `args`. See discussion on *Arguments*
above to see how JavaScript objects are converted to 64-bit integers.

`syscall` returns a `number` which is the result returned by the kernel,
negative numbers usually represent an error.


### `syscall64`

```ts
syscall64(command: number, ...args: TArg[]): [number, number];
```

Same as `syscall`, but returns 64-bit result.


### `syscall_0`

```ts
syscall_0(command: number): number;
syscall_1(command: number, arg1: number): number;
syscall_2(command: number, arg1: number, arg2: number): number;
syscall_3(command: number, arg1: number, arg2: number, arg3: number): number;
syscall_4(command: number, arg1: number, arg2: number, arg3: number, arg4: number): number;
syscall_5(command: number, arg1: number, arg2: number, arg3: number, arg4: number, arg5: number): number;
syscall_6(command: number, arg1: number, arg2: number, arg3: number, arg4: number, arg5: number, arg6: number): number;
```

Executes system calls and returns 32-bit result. Expects all arguments to be of type `number`.


### `syscall64_0`

```ts
syscall64_0(command: number): [number, number];
syscall64_1(command: number, arg1: number): [number, number];
syscall64_2(command: number, arg1: number, arg2: number): [number, number];
syscall64_3(command: number, arg1: number, arg2: number, arg3: number): [number, number];
syscall64_4(command: number, arg1: number, arg2: number, arg3: number, arg4: number): [number, number];
syscall64_5(command: number, arg1: number, arg2: number, arg3: number, arg4: number, arg5: number): [number, number];
syscall64_6(command: number, arg1: number, arg2: number, arg3: number, arg4: number, arg5: number, arg6: number): [number, number];
```

Executes system calls and returns 64-bit result. Expects all arguments to be of type `number`.


### `getAddressArrayBuffer`

```ts
getAddressArrayBuffer(ab: ArrayBuffer): [number, number];
```

### `getAddressTypedArray`

```ts
getAddressTypedArray(ta: TypedArray | Uint8Array): [number, number];
```

### `getAddressBuffer`

```ts
getAddressBuffer(buf: Buffer): [number, number];
```

Return memory address of `Buffer`'s data contents.


### `getAddress`

```ts
getAddress(buffer: Buffer | ArrayBuffer | TypedArray): [number, number];
```

### `frame`

```ts
frame(addresss: Targ, size: number): ArrayBuffer;
```

`frame` returns an `ArrayBuffer` object of size `size` that is mapped to memory location
specified in `addr` argument.


### `call`

```ts
call(address: Targ, offset?: number, arguments?: Targ[]);
```

Execute machine code at specified memory address `address`. The memory address is converted
to function pointer and called using your architecture calling conventions. `offest` is added
to the address, but defaults to 0.

Up to 10 `arguments` can be supplied.


### `call64`

Same as `call` but ruturns a 64-bit `[number, number]`.


### `call_0`

```ts
call_0(address: Targ): number;
```

Call machine code at `address` without any arguments using architecture specific calling conventions. Returns a 32-bit result.


## License

[Unlicense](./LICENSE) - public domain.



[npm-url]: https://www.npmjs.com/package/libsys
[npm-badge]: https://img.shields.io/npm/v/libsys.svg
[travis-url]: https://travis-ci.org/streamich/libsys
[travis-badge]: https://travis-ci.org/streamich/libsys.svg?branch=master

