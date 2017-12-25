const libsys = require('..');

describe('address', function() {
    describe('.getAddressArrayBuffer()', function() {
        it('Returns 2-tuple representing a pointer', function() {
            const ab = new ArrayBuffer(10);
            const tuple = libsys.getAddressArrayBuffer(ab);

            expect(Array.isArray(tuple)).toBe(true);
            expect(tuple.length).toBe(2);
            expect(typeof tuple[0]).toBe('number');
            expect(typeof tuple[1]).toBe('number');
        });
    });

    describe('.getAddressTypedArray()', function() {
        it('Returns 2-tuple representing a pointer', function() {
            const ab = new ArrayBuffer(10);
            const uint8 = new Uint8Array(ab);
            const tuple = libsys.getAddressTypedArray(uint8);

            expect(Array.isArray(tuple)).toBe(true);
            expect(tuple.length).toBe(2);
            expect(typeof tuple[0]).toBe('number');
            expect(typeof tuple[1]).toBe('number');
        });

        it('is same address as ArrayBuffer', function() {
            const ab = new ArrayBuffer(10);
            const uint8 = new Uint8Array(ab);

            const tuple1 = libsys.getAddressArrayBuffer(ab);
            const tuple2 = libsys.getAddressTypedArray(uint8);

            expect(tuple1).toEqual(tuple2);
        });
    });

    describe('.getAddressTypedBuffer()', function() {
        it('Returns 2-tuple representing a pointer', function() {
            const buf = Buffer.from('foobar');
            const tuple = libsys.getAddressBuffer(buf);

            expect(Array.isArray(tuple)).toBe(true);
            expect(tuple.length).toBe(2);
            expect(typeof tuple[0]).toBe('number');
            expect(typeof tuple[1]).toBe('number');
        });

        it('is same address as TypedArray', function() {
            const buf = Buffer.from('foobar');

            const tuple1 = libsys.getAddressTypedArray(buf);
            const tuple2 = libsys.getAddressBuffer(buf);

            expect(tuple1).toEqual(tuple2);
        });
    });

    describe('.getAddress()', () => {
        it('returns same address as corresponding methods', () => {
            const ab = new ArrayBuffer(1);
            expect(libsys.getAddress(ab)).toEqual(libsys.getAddressArrayBuffer(ab));

            const uint8 = new Uint8Array(ab);
            expect(libsys.getAddress(uint8)).toEqual(libsys.getAddressTypedArray(uint8));

            const buf = Buffer.from('foo');
            expect(libsys.getAddress(buf)).toEqual(libsys.getAddressBuffer(buf));
        });
    });
});
