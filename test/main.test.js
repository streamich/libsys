const addon = require('../build/Release/sys.node');
const expect = require('chai').expect;
const rax = require('ass-js/lib/x86/operand').rax;
const Code = require('ass-js/lib/x86/x64/code').Code;

const isMac = process.platform === 'darwin';
const SYS_mmap = isMac ? 197 : 9;

// Allocate executable memory, returns `ArrayBuffer`.
function alloc(size) {
  const flags = 2 /* MAP_PRIVATE */ | (isMac ? 4096 : 32 /* MAP_ANONYMOUS */);
  const protnum = 1 | 2 | 4; // Read, write and execute;
  const addr = addon.syscall64(SYS_mmap, 0, size, protnum, flags, -1, 0);
  return addon.malloc(addr, size);
}

describe('libsys', function() {
    describe('.syscall()', function() {
        it('Writes buffer to STDOUT', function() {
            var str = 'Hello world, hell yeah!';
            var buf = new Buffer(str + '\n');
            var SYS_write = process.platform === 'linux' ? 1 : 4;
            var res = addon.syscall(SYS_write, 1, buf, buf.length);
            expect(res).to.equal(buf.length);
        });
    });

    describe('.addressArrayBuffer64()', function() {
        it('Returns 2-tuple representing a pointer', function() {
            var ab = new ArrayBuffer(10);
            var tuple = addon.addressArrayBuffer64(ab);
            expect(tuple instanceof Array).to.equal(true);
            expect(tuple.length).to.equal(2);
            expect(typeof tuple[0]).to.equal('number');
            expect(typeof tuple[1]).to.equal('number');
        });
    });


    describe('.call()', function () {
        it('calls executable code', () => {
            var _ = new Code;

            _._('mov', [rax, 0xBABE]);
            _._('ret');

            const ab = alloc(1000);
            const uint8 = new Uint8Array(ab, 0, 1000);
            const code = _.compile();

            const a = new ArrayBuffer(100);
            const u = new Uint8Array(a);
            console.log(uint8);
            // uint8[1] = 1;
            // for (let i = 0; i < code.length; i++) {
            //     console.log(i, code[i]);
            //     a[i] = code[i];
            // }
            //
            // console.log(a);

            // var buf = StaticBuffer.from(_.compile(), 'rwe');
            // console.log(buf);
        });
    });
});
