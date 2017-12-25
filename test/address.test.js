const addon = require('../build/Release/sys.node');
const rax = require('ass-js/lib/x86/operand').rax;
const Code = require('ass-js/lib/x86/x64/code').Code;

describe('address', function() {
    describe('.addressArrayBuffer64()', function() {
        it('Returns 2-tuple representing a pointer', function() {
            const ab = new ArrayBuffer(10);
            const tuple = addon.addressArrayBuffer64(ab);

            expect(Array.isArray(tuple)).toBe(true);
            expect(tuple.length).toBe(2);
            expect(typeof tuple[0]).toBe('number');
            expect(typeof tuple[1]).toBe('number');
        });
    });
});
