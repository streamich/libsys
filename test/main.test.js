const libsys = require('..');

describe('libsys', function() {
    it('returns expected API', () => {
        expect(typeof libsys.syscall).toBe('function');
        expect(typeof libsys.syscall64).toBe('function');
        expect(typeof libsys.syscall_0).toBe('function');
        expect(typeof libsys.syscall_1).toBe('function');
        expect(typeof libsys.syscall_2).toBe('function');
        expect(typeof libsys.syscall_3).toBe('function');
        expect(typeof libsys.syscall_4).toBe('function');
        expect(typeof libsys.syscall_5).toBe('function');
        expect(typeof libsys.syscall_6).toBe('function');
        expect(typeof libsys.syscall64_0).toBe('function');
        expect(typeof libsys.syscall64_1).toBe('function');
        expect(typeof libsys.syscall64_2).toBe('function');
        expect(typeof libsys.syscall64_3).toBe('function');
        expect(typeof libsys.syscall64_4).toBe('function');
        expect(typeof libsys.syscall64_5).toBe('function');
        expect(typeof libsys.syscall64_6).toBe('function');

        expect(typeof libsys.frame).toBe('function');

        expect(typeof libsys.call).toBe('function');
        expect(typeof libsys.call64).toBe('function');
        expect(typeof libsys.call_0).toBe('function');
        expect(typeof libsys.call_1).toBe('function');
        expect(typeof libsys.call64_0).toBe('function');
        expect(typeof libsys.call64_1).toBe('function');

        expect(typeof libsys.getAddress).toBe('function');
        expect(typeof libsys.getAddressArrayBuffer).toBe('function');
        expect(typeof libsys.getAddressTypedArray).toBe('function');
        expect(typeof libsys.getAddressBuffer).toBe('function');
    });
});
