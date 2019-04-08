var WebAudioSounds = function () {
    this.samples = null;
    this.samplesPerSecond = 0;
    this.writeCursor = 0;
    this.dataCursor = 0;

    this.audioContext = null;

    this.audioBufferSize = 0;
    this.sampleBufferSize = 0;

    this.started = false;
};

WebAudioSounds.prototype = {

    init: function (game) {
        var audioContextClass = window.AudioContext || window.webkitAudioContext;
        audioContext = new audioContextClass();
        var audioProcessor = audioContext.createScriptProcessor(this.audioBufferSize, 1, 1);
        audioProcessor.onaudioprocess = (e) => this.bufferAudio(e);
        this.samplesPerSecond = audioContext.sampleRate;
        this.sampleBufferSize = this.samplesPerSecond;

        this.samples = new Float32Array(this.sampleBufferSize);
        this.writeCursor = 0;
        this.dataCursor = (1.0/60.0) * this.samplesPerSecond;

        audioProcessor.connect(audioContext.destination);
    },

    getNumSamplesToUpdate: function () {
        var samplesPerFrame = (1.0/60.0) * this.samplesPerSecond;

        var unwrappedDataCursor = this.dataCursor < this.writeCursor ? 
                                  this.dataCursor + this.sampleBufferSize : this.dataCursor;
        var targetIndex = this.writeCursor + this.audioBufferSize*3;
        var numSamples = targetIndex - unwrappedDataCursor;
        return numSamples;
    },

    updateAudio: function (game, soundSamples, numSamples) {
        var floatBuffer = new Float32Array(game.buffer, soundSamples, numSamples);
        for (var i = 0; i < numSamples; ++i) {
            this.samples[this.dataCursor] = floatBuffer[i];
            this.dataCursor = (this.dataCursor+1) % this.sampleBufferSize;

            //if (this.writeCursor == this.dataCursor) {
            //    console.log("running overlaps write");
            //}
        }
    },

    bufferAudio: function (e)  {
        var output = e.outputBuffer.getChannelData(0);
        var runningStart = this.dataCursor;
        var writeStart = this.writeCursor;
        for (var i = 0; i < this.audioBufferSize; ++i) {
            output[i] = this.samples[this.writeCursor];
            this.writeCursor = (this.writeCursor + 1) % this.sampleBufferSize;
            //if (this.writeCursor == this.dataCursor) {
            //    console.log("write overlaps running");
            //}
        }
    }

};

module.exports = WebAudioSounds;
