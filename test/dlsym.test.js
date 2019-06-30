const libsys = require('..');

describe('dlsym', function() {
    it('method exists', () => {
        expect(typeof libsys.dlsym).toBe('function');
    });

    it('returns an address', () => {
        const addr = libsys.dlsym('fcntl');
        expect(addr.length).toBe(2);
        expect(typeof addr[0]).toBe('number');
        expect(typeof addr[1]).toBe('number');
    });

    it('returns correct dlsym function address', () => {
        const addr1 = libsys.dlsym('dlsym');
        const addr2 = libsys.__testDlsymAddr()
        expect(addr1).toEqual(addr2);
    });

    it('call getpid()', () => {
        const addr1 = libsys.dlsym('getpid');
        const pid1 = process.pid;
        const [pid2] = libsys.call64(addr1, 0, []);
        expect(pid2).toBe(pid1);
    });
});
