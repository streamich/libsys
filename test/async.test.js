const {async, getAddress, dlsym} = require('..');

const {pid} = process;
const SYS_getpid = (0x2000000) + 20;

describe('async', function() {
    it('method exists', () => {
        expect(typeof async).toBe('function');
    });

    for (let i = 1; i < 7; i++) {
        it(`can execute getpid with ${i} threads`, async () => {
            // console.log('PID', pid);
            const numThreads = i;                       // Number of threads to spin up.
            const emptyRecord = Buffer.alloc(2 * 8);    // Create empty call record, where threads are pointed initially.
            emptyRecord.writeInt8(-1, 0);            // Tell threads that this record is already in use.

            const result = async(emptyRecord, numThreads);
            expect(typeof result).toBe('number');
            // console.log('result', result);


            const recordGetpid = Buffer.alloc(3 * 8, 0);
            recordGetpid.writeInt32LE(SYS_getpid, 2 * 8);

            // console.log(emptyRecord);
            // console.log(recordGetpid);

            const addr = getAddress(recordGetpid);
            // console.log(addr);
            
            emptyRecord.writeInt32LE(addr[0], 8);
            emptyRecord.writeInt32LE(addr[1], 8 + 4);

            // console.log(emptyRecord);
            // console.log(recordGetpid);

            emptyRecord.writeInt8(1, 2);

            // console.log(emptyRecord);
            // console.log(recordGetpid);

            await new Promise(r => setTimeout(r, 10));

            // console.log(emptyRecord);
            // console.log(recordGetpid);

            const res = recordGetpid.readInt32LE(2 * 8);
            // console.log('result', res);
            expect(res).toBe(pid);

            const exitRecord = Buffer.alloc(2 * 8);
            exitRecord.writeInt8(2, 1);
            // console.log(exitRecord);
            const exitAddr = getAddress(exitRecord);
            recordGetpid.writeInt32LE(exitAddr[0], 8);
            recordGetpid.writeInt32LE(exitAddr[1], 8 + 4);
            recordGetpid.writeInt8(1, 2);

            await new Promise(r => setTimeout(r, 5));

            const numExited = exitRecord.readInt8(3);
            // console.log('numExited', emptyRecord.readInt8(3), recordGetpid.readInt8(3), numExited);
        });
    }

    test('can execute a call', async () => {
        const addr = dlsym('getpid');
        const numThreads = 2;                       // Number of threads to spin up.
        const emptyRecord = Buffer.alloc(2 * 8);    // Create empty call record, where threads are pointed initially.
        emptyRecord.writeInt8(-1, 0);               // Tell threads that this record is already in use.

        async(emptyRecord, numThreads);
        

        const recordGetpid = Buffer.alloc(4 * 8, 0);
        recordGetpid.writeInt8(1, 1); // set TYPE_CALL
        recordGetpid.writeInt32LE(addr[0], 2 * 8);
        recordGetpid.writeInt32LE(addr[1], 2 * 8 + 4);

        const nextAddr = getAddress(recordGetpid);
        emptyRecord.writeInt32LE(nextAddr[0], 8);
        emptyRecord.writeInt32LE(nextAddr[1], 8 + 4);
        emptyRecord.writeInt8(1, 2);
        
        await new Promise(r => setTimeout(r, 10));
        const res = recordGetpid.readInt32LE(2 * 8);
        expect(res).toBe(pid);
    });
});
