(function() {
    // Classifier module
    let classifierInitialized = false;
    Module.onRuntimeInitialized = function() {
        classifierInitialized = true;
    };

    let ptrs = {};
    Module.onMalloc = function onMalloc(ptr, size) {
        ptrs[ptr] = size;
    };
    Module.onFree = function onFree(ptr) {
        delete ptrs[ptr];
    };

    class EdgeImpulseClassifier {
        constructor() {
            this._initialized = false;
        }

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
            let ret = Module.run_classifier(obj.bytes.byteOffset, rawData.length, debug);
            Module._free(obj.ptr);

            if (ret.result !== 0) {
                throw new Error('Classification failed (err code: ' + ret.result + ')');
            }

            let jsResult = {
                anomaly: ret.anomaly,
                results: []
            };

            for (let cx = 0; cx < ret.classification.size(); cx++) {
                let c = ret.classification.get(cx);
                jsResult.results.push({ label: c.label, value: c.value });
            }

            return jsResult;
        }

        _arrayToHeap(data) {
            let typedArray = new Float32Array(data);
            let numBytes = typedArray.length * typedArray.BYTES_PER_ELEMENT;
            let ptr = Module._malloc(numBytes);
            let heapBytes = new Uint8Array(Module.HEAPU8.buffer, ptr, numBytes);
            heapBytes.set(new Uint8Array(typedArray.buffer));
            return { ptr: ptr, bytes: heapBytes };
        }
    }

    window.EdgeImpulseClassifier = EdgeImpulseClassifier;
})();
