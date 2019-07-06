const {async, getAddress} = require('..');

const {pid} = process;
const SYS_getpid = (0x2000000) + 47;

describe('async', function() {
    it('method exists', () => {
        expect(typeof async).toBe('function');
    });

    it('can call it', async () => {
        const numThreads = 2;                       // Number of threads to spin up.
        const emptyRecord = Buffer.alloc(2 * 8);    // Create empty call record, where threads are pointed initially.
        emptyRecord.writeInt32LE(-1, 0);            // Tell threads that this record is already in use.

        const result = async(emptyRecord, numThreads);
        expect(result).toBe(25);

        const recordGetpid = Buffer.alloc(3 * 8);
        recordGetpid.writeInt16LE(1, 6);
        recordGetpid.writeInt32LE(SYS_getpid, 2 * 8);

        console.log(emptyRecord);
        console.log(recordGetpid);

        const addr = getAddress(recordGetpid);
        console.log(addr);
        emptyRecord.writeInt32LE(addr[0], 8);
        emptyRecord.writeInt32LE(addr[1], 8 + 4);
        console.log(emptyRecord);
        console.log(recordGetpid);

        await new Promise(r => setTimeout(r, 10));

        console.log(emptyRecord);
        console.log(recordGetpid);
    });
});
