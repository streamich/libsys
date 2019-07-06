const {async, getAddress} = require('..');

const {pid} = process;
const SYS_getpid = (0x2000000) + 20;

describe('async', function() {
    it('method exists', () => {
        expect(typeof async).toBe('function');
    });

    for (let i = 1; i < 7; i++) {
        it(`can execute getpid with ${i} threads`, async () => {
            console.log('PID', pid);
            const numThreads = i;                       // Number of threads to spin up.
            const emptyRecord = Buffer.alloc(2 * 8);    // Create empty call record, where threads are pointed initially.
            emptyRecord.writeInt32LE(-1, 0);            // Tell threads that this record is already in use.

            const result = async(emptyRecord, numThreads);
            expect(result).toBe(25);

            const recordGetpid = Buffer.alloc(3 * 8);
            recordGetpid.writeInt32LE(SYS_getpid, 2 * 8);

            console.log(emptyRecord);
            console.log(recordGetpid);

            const addr = getAddress(recordGetpid);
            console.log(addr);
            
            emptyRecord.writeInt32LE(addr[0], 8);
            emptyRecord.writeInt32LE(addr[1], 8 + 4);

            console.log(emptyRecord);
            console.log(recordGetpid);

            emptyRecord.writeInt8(1, 6);
            
            console.log(emptyRecord);
            console.log(recordGetpid);

            await new Promise(r => setTimeout(r, 10));

            console.log(emptyRecord);
            console.log(recordGetpid);

            const res = recordGetpid.readInt32LE(2 * 8);
            console.log('result', res);

            const exitRecord = Buffer.alloc(3 * 8);
            exitRecord.writeInt8(2, 4);
            console.log(exitRecord);
            const exitAddr = getAddress(exitRecord);
            recordGetpid.writeInt32LE(exitAddr[0], 8);
            recordGetpid.writeInt32LE(exitAddr[1], 8 + 4);
            recordGetpid.writeInt8(1, 6);

            await new Promise(r => setTimeout(r, 5));

            const numExited = exitRecord.readInt8(7);
            console.log('numExited', emptyRecord.readInt8(7), recordGetpid.readInt8(7), numExited);
        });
    }
});
