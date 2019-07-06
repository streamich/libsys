const {cmpxchg32, getAddress} = require('..');

describe('cmpxchg32', function() {
    it('method exists', () => {
        expect(typeof cmpxchg32).toBe('function');
    });

    it('can exchange value', () => {
      const buf = Buffer.alloc(4, 0);
      expect(buf.readInt32LE()).toBe(0);
      cmpxchg32(buf, 0, 3);
      expect(buf.readInt32LE()).toBe(3);
    });

    it('can exchange value multiple times', () => {
      const buf = Buffer.alloc(4, 0);
      expect(buf.readInt32LE()).toBe(0);
      cmpxchg32(buf, 0, 3);
      expect(buf.readInt32LE()).toBe(3);
      cmpxchg32(buf, 3, 2);
      expect(buf.readInt32LE()).toBe(2);
      cmpxchg32(buf, 2, 7);
      expect(buf.readInt32LE()).toBe(7);
    });

    it('does not exchange value if test value does not match', () => {
      const buf = Buffer.alloc(4, 0);
      expect(buf.readInt32LE()).toBe(0);
      cmpxchg32(buf, 1, 3);
      expect(buf.readInt32LE()).toBe(0);
    });

    it('does not exchange value if test value does not match - 2', () => {
      const buf = Buffer.alloc(4, 0);
      expect(buf.readInt32LE()).toBe(0);
      cmpxchg32(buf, 0, 1);
      cmpxchg32(buf, 1, 2);
      cmpxchg32(buf, 2, 3);
      cmpxchg32(buf, 2, 5);
      expect(buf.readInt32LE()).toBe(3);
    });

    it('can exchange value at address', () => {
      const buf = Buffer.alloc(4, 0);
      const addr = getAddress(buf);
      expect(buf.readInt32LE()).toBe(0);
      cmpxchg32(addr, 0, 3);
      expect(buf.readInt32LE(0)).toBe(3);
    });

    it('can exchange value at address and offset', () => {
      const buf = Buffer.alloc(8, 0);
      const addr = [...getAddress(buf), 4];
      
      expect(buf.readInt32LE(0)).toBe(0);
      expect(buf.readInt32LE(4)).toBe(0);

      cmpxchg32(addr, 0, 10);

      expect(buf.readInt32LE(0)).toBe(0);
      expect(buf.readInt32LE(4)).toBe(10);
    });
});
