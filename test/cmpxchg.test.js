const {cmpxchg16, cmpxchg32, getAddress} = require('..');

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

    it('returns value in memory on success', () => {
      const buf = Buffer.alloc(4, 0);
      buf.writeInt32LE(1);
      const res = cmpxchg32(buf, 1, 10);
      expect(res).toBe(1);
    });

    it('returns value in memory on failure', () => {
      const buf = Buffer.alloc(4, 0);
      buf.writeInt32LE(1);
      const res = cmpxchg32(buf, 2, 10);
      expect(res).toBe(1);
    });
});


describe('cmpxchg16', function() {
  it('method exists', () => {
      expect(typeof cmpxchg16).toBe('function');
  });

  it('can exchange value', () => {
    const buf = Buffer.alloc(2, 0);
    expect(buf.readInt16LE()).toBe(0);
    cmpxchg16(buf, 0, 1);
    expect(buf.readInt16LE()).toBe(1);
    cmpxchg16(buf, 1, -1);
    cmpxchg16(buf, 1, 6);
    expect(buf.readInt16LE()).toBe(-1);
  });

  it('does not affect adjacent memory blocks', () => {
    const buf = Buffer.alloc(6, 0);

    expect(buf.readInt16LE(0)).toBe(0);
    expect(buf.readInt16LE(2)).toBe(0);
    expect(buf.readInt16LE(4)).toBe(0);

    const addr = [...getAddress(buf), 2];
    cmpxchg16(addr, 0, -1);

    expect(buf.readInt16LE(0)).toBe(0);
    expect(buf.readInt16LE(2)).toBe(-1);
    expect(buf.readInt16LE(4)).toBe(0);
  });
});
