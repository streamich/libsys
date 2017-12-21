var addon = require('../build/Release/sys.node');
var expect = require('chai').expect;


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
});
