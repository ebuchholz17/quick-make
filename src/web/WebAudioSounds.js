var WebAudioSounds = function () {
    this.samplesPerSecond = 0;

    this.audioContext = null;
    this.audioBufferSize = 0;

    this.started = false;
    this.nextAudioBufferPlayTime = 0.0;
};

WebAudioSounds.prototype = {

    init: function () {
        var audioContextClass = window.AudioContext || window.webkitAudioContext;
        this.audioContext = new audioContextClass();
        this.samplesPerSecond = this.audioContext.sampleRate;

        this.playTime = this.audioContext.currentTime;
    },

    getSampleRate: function () {
        // make a dummy audio context to figure out the sample rate
        var audioContextClass = window.AudioContext || window.webkitAudioContext;
        var audioContext = new audioContextClass(); 
        return audioContext.sampleRate;
    },

    updateAudio: function (game, gameMemory, gameSoundOutput, soundSamples) {
        var currentAudioTime = this.audioContext.currentTime;
        var bufferingDelay = 50/1000;
        var bufferTime = this.audioBufferSize / this.samplesPerSecond;
        var numSamples = this.audioBufferSize;
        var maxNumQueuedBuffers = 5;

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

                var floatBuffer = new Float32Array(game.HEAPU8.buffer, soundSamples, numSamples);
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
    },

    tryUnlockAudioContext: function () {
        if (this.audioContext.state == "suspended") {
            this.audioContext.resume();
        }
    }

};

module.exports = WebAudioSounds;
