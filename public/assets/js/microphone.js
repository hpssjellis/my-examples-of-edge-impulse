(function() {

    class MicrophoneSensor {
        constructor() {
            this._constraints = {
                audio: true,
                video: false
            };

            if (this.hasSensor()) {
                this._audioContext = new (window.AudioContext || window.webkitAudioContext)();
            }
        }

        hasSensor() {
            return typeof window.AudioContext !== 'undefined' || typeof window.webkitAudioContext !== 'undefined';
        }

        async checkPermissions(fromButton) {
            if (!this.hasSensor()) {
                throw new Error('Microphone not present on this device');
            }

            if (this._recorder) {
                return true;
            }

            if (!fromButton) {
                return false;
            }

            if (this._audioContext?.state === "suspended") {
                // Resume after user interaction
                // https://developers.google.com/web/updates/2017/09/autoplay-policy-changes#webaudio
                await this._audioContext.resume();
            }

            this._stream = await navigator.mediaDevices.getUserMedia(this._constraints);

            return true;
        }

        takeSample(length, frequency, processing) {
            return new Promise((resolve, reject) => {
                if (!this._stream) {
                    return reject('No audio stream');
                }

                if (frequency !== 16000) {
                    return reject('Microphone only supports sampling at 16000Hz');
                }

                if (!this._audioContext) {
                    return reject('No audio context');
                }

                // use the stream
                let input = this._audioContext.createMediaStreamSource(this._stream);

                // Create the Recorder object and configure to record mono sound (1 channel)
                // Recording 2 channels will double the file size
                if (!this._recorder) {
                    this._recorder = new Recorder(input, {
                        numChannels: 1
                    });
                    this._recorder.record();
                }
                else {
                    this._recorder.clear();
                }

                setTimeout(() => {
                    if (!this._stream) return;

                    // tell the recorder to stop the recording
                    // this._stream.getAudioTracks()[0].stop();

                    processing();

                    if (!this._recorder) return;

                    // create the wav blob and pass it on to createDownloadLink
                    this._recorder.exportWAV(async (blob) => {
                        let buffer = await new Response(blob).arrayBuffer();
                        console.log('done recording', buffer.byteLength);
                        let wavFileItems = new Int16Array(buffer, 44);
                        let eiData = [];
                        for (let w of wavFileItems) {
                            eiData.push(w);
                        }

                        // this._stream = undefined;

                        resolve({
                            values: eiData.slice(0, length * 16),
                            intervalMs: 1000 / 16000,
                            sensors: [{
                                    name: "audio",
                                    units: "wav"
                                }
                            ],
                        });
                    });
                }, length + 50);
            });
        };
    }
    window.MicrophoneSensor = MicrophoneSensor;
})();
