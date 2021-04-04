// Load the inferencing WebAssembly module
const Module = require('./edge-impulse-standalone');
const fs = require('fs');

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

if (!process.argv[2]) {
    return console.error('Requires one parameter (a comma-separated list of raw features, or a file pointing at raw features)');
}

let features = process.argv[2];
if (fs.existsSync(features)) {
    features = fs.readFileSync(features, 'utf-8');
}

// Initialize the classifier, and invoke with the argument passed in
let classifier = new EdgeImpulseClassifier();
classifier.init().then(async () => {
    let result = classifier.classify(features.trim().split(',').map(n => Number(n)));

    console.log(result);
}).catch(err => {
    console.error('Failed to initialize classifier', err);
});
