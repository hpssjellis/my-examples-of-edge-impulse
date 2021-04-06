// Classifier module
let classifierInitialized = false;
Module.onRuntimeInitialized = function() {
    classifierInitialized = true;
};

class EdgeImpulseClassifier {
    _initialized = false;

    init() {
        if (classifierInitialized === true) return Promise.resolve();

        return new Promise((resolve) => {
            Module.onRuntimeInitialized = () => {
                resolve();
                classifierInitialized = true;
            };
        });
    }

    classify(rawData, debug = false) {
        if (!classifierInitialized) throw new Error('Module is not initialized');

        const obj = this._arrayToHeap(rawData);
        let ret = Module.run_classifier(obj.buffer.byteOffset, rawData.length, debug);
        Module._free(obj.ptr);

        if (ret.result !== 0) {
            throw new Error('Classification failed (err code: ' + ret.result + ')');
        }


        let jsResult = {
            anomaly: ret.anomaly,
            results: []
        };

        // if ret.size is a function, then this is a new module. Use this API call to prevent leaks.
        // the old API (calling via ret.classification) is still there for backwards compatibility, but leaks some memory
        if (typeof ret.size === 'function') {
            for (let cx = 0; cx < ret.size(); cx++) {
                let c = ret.get(cx);
                jsResult.results.push({ label: c.label, value: c.value });
                c.delete();
            }
        }
        else {
            for (let cx = 0; cx < ret.classification.size(); cx++) {
                let c = ret.classification.get(cx);
                jsResult.results.push({ label: c.label, value: c.value });
                c.delete();
            }
        }

        ret.delete();

        return jsResult;
    }

    _arrayToHeap(data) {
        let typedArray = new Float32Array(data);
        let numBytes = typedArray.length * typedArray.BYTES_PER_ELEMENT;
        let ptr = Module._malloc(numBytes);
        let heapBytes = new Uint8Array(Module.HEAPU8.buffer, ptr, numBytes);
        heapBytes.set(new Uint8Array(typedArray.buffer));
        return { ptr: ptr, buffer: heapBytes };
    }
}
