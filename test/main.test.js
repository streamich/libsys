const addon = require('../build/Release/sys.node');

describe('libsys', function() {
    it('returns expected API', () => {
        expect(typeof addon.syscall).toBe('function');
        expect(typeof addon.syscall64).toBe('function');
        expect(typeof addon.syscall_0).toBe('function');
        expect(typeof addon.syscall_1).toBe('function');
        expect(typeof addon.syscall_2).toBe('function');
        expect(typeof addon.syscall_3).toBe('function');
        expect(typeof addon.syscall_4).toBe('function');
        expect(typeof addon.syscall_5).toBe('function');
        expect(typeof addon.syscall_6).toBe('function');
        expect(typeof addon.syscall64_0).toBe('function');
        expect(typeof addon.syscall64_1).toBe('function');
        expect(typeof addon.syscall64_2).toBe('function');
        expect(typeof addon.syscall64_3).toBe('function');
        expect(typeof addon.syscall64_4).toBe('function');
        expect(typeof addon.syscall64_5).toBe('function');
        expect(typeof addon.syscall64_6).toBe('function');

        expect(typeof addon.frame).toBe('function');
        expect(typeof addon.call).toBe('function');
        expect(typeof addon.call64).toBe('function');
    });
});
