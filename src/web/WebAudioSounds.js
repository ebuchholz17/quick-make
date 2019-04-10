var WebAudioSounds = function () {
    this.samples = null;
    this.samplesPerSecond = 0;
    this.writeCursor = 0;
    this.dataCursor = 0;

    this.audioContext = null;

    this.audioBufferSize = 0;
    this.sampleBufferSize = 0;

    this.started = false;
    this.frameStartAudioTime = 0.0;
    this.nextAudioBufferPlayTime = 0.0;
};

WebAudioSounds.prototype = {

    init: function () {
        var audioContextClass = window.AudioContext || window.webkitAudioContext;
        this.audioContext = new audioContextClass();
        //var audioProcessor = audioContext.createScriptProcessor(this.audioBufferSize, 1, 1);
        //audioProcessor.onaudioprocess = (e) => this.bufferAudio(e);
        this.samplesPerSecond = this.audioContext.sampleRate;
        this.sampleBufferSize = this.samplesPerSecond;

        this.samples = new Float32Array(this.sampleBufferSize);
        this.writeCursor = 0;
        this.dataCursor = (1.0/60.0) * this.samplesPerSecond;
        this.playTime = this.audioContext.currentTime;

        //audioProcessor.connect(audioContext.destination);
    },

    updateAudio: function (game, gameMemory, gameSoundOutput, soundSamples) {
        var currentAudioTime = this.audioContext.currentTime;
        var bufferingDelay = 50/1000;
        var bufferTime = this.audioBufferSize / this.samplesPerSecond;
        var numSamples = this.audioBufferSize;
        var maxNumQueuedBuffers = 4;

        for (var i = 0; i < maxNumQueuedBuffers; ++i) {
            var secsLeftTilNextAudio = this.nextAudioBufferPlayTime - currentAudioTime;
            if (secsLeftTilNextAudio < bufferingDelay + bufferTime * maxNumQueuedBuffers) {

                gameSoundOutput.samples = soundSamples;
                gameSoundOutput.samplesPerSecond = this.samplesPerSecond;
                gameSoundOutput.sampleCount = numSamples;

                game.ccall("getGameSoundSamples", 
                    "null", 
                    ["number", "number"], 
                    [
                        game.getPointer(gameMemory), 
                        game.getPointer(gameSoundOutput)
                    ]
                );

                var floatBuffer = new Float32Array(game.buffer, soundSamples, numSamples);
                var buffer = this.audioContext.createBuffer(1, numSamples, this.samplesPerSecond);
                var output = buffer.getChannelData(0);
                for (var sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex) {
                    output[sampleIndex] = floatBuffer[sampleIndex];
                }

                var bufferSourceNode = this.audioContext.createBufferSource();
                bufferSourceNode.connect(this.audioContext.destination);
                bufferSourceNode.buffer = buffer;

                var playTime = Math.max(currentAudioTime + bufferingDelay, this.nextAudioBufferPlayTime);
                bufferSourceNode.start(playTime);

                this.nextAudioBufferPlayTime = playTime + bufferTime;
            }
            else { 
                break;
            }
        }
    }

};

module.exports = WebAudioSounds;
