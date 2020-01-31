"use strict";

var MobileDetect = require("mobile-detect");

var Game = require("./wasm/game.js");
var WebAudioSounds = require("./WebAudioSounds");
var WebGLRenderer = require("./WebGLRenderer");

var Input = function () {
    this.keyDown = {};
    this.keyJustPressed = {};
    this.mouseX = -1;
    this.mouseY = -1;
};

var WebPlatform = function () {
    this.viewport = null;
    this.canvas = null;
    this.updateCallback = null;

    this.game = null;
    this.renderer = null;
    this.totalAssetsLoaded = 0;
    this.webAudioSounds = null;
    this.lastTime = 0;
};


WebPlatform.prototype = {

    init: function () {
        this.game = Game();
        this.game.onRuntimeInitialized = this.run.bind(this);
    },

    run: function () {
        // TODO(ebuchholz): remove
        //var testElement = document.getElementById("megatest");
        //var console = (function (oldConsole) {
        //    return {
        //        log: function (text) {
        //            oldConsole.log(text);
        //            testElement.innerHTML += text + "<br>";
        //        },
        //        info: function (text) {
        //            oldConsole.info(text);
        //            testElement.innerHTML += text + "<br>";
        //        },
        //        warn: function (text) {
        //            oldConsole.warn(text);
        //            testElement.innerHTML += text + "<br>";
        //        },
        //        error:function (text){
        //            oldConsole.error(text);
        //            testElement.innerHTML += text + "<br>";
        //        }
        //    };
        //})(window.console);
        //window.console = console;

        this.viewport = document.getElementById("quickmake");
        this.viewport.style["touch-action"] = "none";
        this.viewport.style["overflow"] = "hidden";
        this.canvas = document.createElement("canvas");
        this.viewport.insertAdjacentElement("afterbegin", this.canvas);
        this.viewport.style.display = "block";

        this.canvas.width = 1152;
        this.canvas.height = 648;
        this.canvas.style["touch-action"] = "none";
        this.canvas.style["user-select"] = "none";
        this.canvas.style["-moz-user-select"] = "none";
        this.canvas.style["-ms-user-select"] = "none";
        this.canvas.style["-khtml-user-select"] = "none";
        this.canvas.style["-webkit-user-select"] = "none";
        this.canvas.style["outline"] = "none";
        this.canvas.style["overflow"] = "hidden";
        this.canvas.draggable = false;
        this.viewport.draggable = false;

        // for interactive file loading/saving
        this.fileInput = document.createElement("input");
        this.fileInput.setAttribute("type", "file");
        this.fileInput.onchange = this.onFileToLoadSelected.bind(this);
        this.loadedFile = null;
        this.loadedFileSize = 0;
        this.fileSelectQueued = false;

        window.addEventListener("resize", this.resize.bind(this));
        this.updateCallback = this.update.bind(this);

        this.renderer = new WebGLRenderer();
        this.renderer.initWebGL(this.canvas);

        this.gameMemory = this.game.wrapPointer(this.game._malloc(this.game.sizeof_game_memory()), 
                                                this.game.game_memory);
        this.gameMemory.memoryCapacity = 50 * 1024 * 1024;
        this.gameMemory.memory = this.game._malloc(this.gameMemory.memoryCapacity);
        this.gameMemory.tempMemoryCapacity = 10 * 1024 * 1024;
        this.gameMemory.tempMemory = this.game._malloc(this.gameMemory.tempMemoryCapacity);

        // sounds
        this.webAudioSounds = new WebAudioSounds();
        this.webAudioSounds.audioBufferSize = 512; // TODO(ebuchholz): tune buffers (windows and web)

        // NOTE(ebuchholz): need to init on first input to work in browsers
        //this.webAudioSounds.init(this.game);

        // TODO(ebuchholz): clear memory
        this.platformOptions = this.game.wrapPointer(this.game._malloc(this.game.sizeof_platform_options()), 
                                                     this.game.platform_options);
        this.platformOptions.audioSampleRate = this.webAudioSounds.getSampleRate();

        this.platformTriggers = this.game.wrapPointer(this.game._malloc(this.game.sizeof_platform_triggers()), 
                                                     this.game.platform_triggers);
        this.platformTriggers.triggerFileWindow = false;
        this.platformTriggers.triggerFileSave = false;

        this.assetList = this.game.wrapPointer(this.game._malloc(this.game.sizeof_asset_list()), 
                                                this.game.asset_list);
        this.assetList.numAssetsToLoad = 0;
        this.assetList.maxAssetsToLoad = 100;
        this.assetList.assetsToLoad = 
            this.game._malloc(this.assetList.maxAssetsToLoad * this.game.sizeof_asset_to_load());

        this.game.ccall("getGameAssetList", 
            "null", 
            ["number"], 
            [this.game.getPointer(this.assetList)]
        );

        this.workingAssetMemory = this.game.wrapPointer(this.game._malloc(this.game.sizeof_memory_arena()), 
                                                       this.game.memory_arena);
        this.workingAssetMemory.capacity = 30 * 1024 * 1024;
        this.workingAssetMemory.base = this.game._malloc(this.workingAssetMemory.capacity);

        var assetsToLoadPointer = this.game.getPointer(this.assetList.assetsToLoad);
        for (var i = 0; i < this.assetList.numAssetsToLoad; ++i) {
            var assetToLoad = this.game.wrapPointer(assetsToLoadPointer + this.game.sizeof_asset_to_load() * i, this.game.asset_to_load);
            switch (assetToLoad.type) {
                default:
                    console.log("invalid asset to load type");
                    break;
                case this.game.ASSET_TYPE_ANIMATION_DATA: {
                    this.loadAnimationDataFile(assetToLoad.path, assetToLoad.type, assetToLoad.key);
                } break;
                case this.game.ASSET_TYPE_OBJ: {
                    this.loadOBJFile(assetToLoad.path, assetToLoad.type, assetToLoad.key);
                } break;
                case this.game.ASSET_TYPE_QMM: {
                    this.loadQMMFile(assetToLoad.path, assetToLoad.type, assetToLoad.key);
                } break;
                case this.game.ASSET_TYPE_BMP: {
                    this.loadBMPFile(assetToLoad.path, assetToLoad.type, assetToLoad.key);
                } break;
                case this.game.ASSET_TYPE_WAV: {
                    this.loadWAVFile(assetToLoad.path, assetToLoad.type, assetToLoad.key);
                } break;
                case this.game.ASSET_TYPE_ATLAS: {
                    this.loadTextureAtlas(assetToLoad.path, assetToLoad.type, assetToLoad.key, assetToLoad.secondKey);
                } break;
                case this.game.ASSET_TYPE_DATA: {
                    this.loadDataAsset(assetToLoad.path, assetToLoad.type, assetToLoad.key);
                } break;
            }
        }

    },

    loadAnimationDataFile: function (path, assetType, assetKey, assetSecondKey) {
        //fetch(path).then(
        //    function (file) {
        //        file.text().then(
        //            function (data) {
                       // var strLength = this.game.lengthBytesUTF8(data);
                       // var objFileData = this.game._malloc(strLength + 1);
                       // this.game.writeAsciiToMemory(data, objFileData); 

                        this.workingAssetMemory.size = 0;

                        var fileData = 0;
                        this.game.ccall("parseGameAsset", 
                            "null", 
                            ["number", "number", "number", "number", 
                             "number", "number", "number", "number", "number"], 
                            [
                                fileData, 
                                0,
                                assetType,
                                assetKey,
                                assetSecondKey,
                                this.game.getPointer(this.gameMemory),
                                this.game.getPointer(this.workingAssetMemory),
                                this.game.getPointer(this.platformOptions),
                                0
                            ]
                        );

                        this.onAssetLoaded();
        //            }.bind(this),
        //            function () {
        //                console.log("fetch .text() failed for " + path);
        //            }.bind(this)
        //        );
        //    }.bind(this),
        //    function () {
        //        console.log("Failed to fetch " + path);
        //    }.bind(this)
        //);
    },

    loadOBJFile: function (path, assetType, assetKey, assetSecondKey) {
        fetch(path).then(
            function (file) {
                file.text().then(
                    function (data) {
                        var strLength = this.game.lengthBytesUTF8(data);
                        var objFileData = this.game._malloc(strLength + 1);
                        this.game.writeAsciiToMemory(data, objFileData); 

                        this.workingAssetMemory.size = 0;

                        this.game.ccall("parseGameAsset", 
                            "null", 
                            ["number", "number", "number", "number", 
                             "number", "number", "number", "number", "number"], 
                            [
                                objFileData, 
                                0,
                                assetType,
                                assetKey,
                                assetSecondKey,
                                this.game.getPointer(this.gameMemory),
                                this.game.getPointer(this.workingAssetMemory),
                                this.game.getPointer(this.platformOptions),
                                0
                            ]
                        );

                        var loadedMesh = 
                            this.game.wrapPointer(
                                this.game.getPointer(this.workingAssetMemory.base), 
                                this.game.loaded_mesh_asset
                            );

                        this.renderer.loadMesh(this.game, loadedMesh);
                        this.onAssetLoaded();
                    }.bind(this),
                    function () {
                        console.log("fetch .text() failed for " + path);
                    }.bind(this)
                );
            }.bind(this),
            function () {
                console.log("Failed to fetch " + path);
            }.bind(this)
        );
    },

    loadQMMFile: function (path, assetType, assetKey, assetSecondKey) {
        //fetch(path).then(
        //    function (file) {
        //        file.text().then(
        //            function (data) {
                       // var strLength = this.game.lengthBytesUTF8(data);
                       // var objFileData = this.game._malloc(strLength + 1);
                       // this.game.writeAsciiToMemory(data, objFileData); 

                        this.workingAssetMemory.size = 0;

                        var fileData = 0;
                        this.game.ccall("parseGameAsset", 
                            "null", 
                            ["number", "number", "number", "number", 
                             "number", "number", "number", "number", "number"], 
                            [
                                fileData, 
                                0,
                                assetType,
                                assetKey,
                                assetSecondKey,
                                this.game.getPointer(this.gameMemory),
                                this.game.getPointer(this.workingAssetMemory),
                                this.game.getPointer(this.platformOptions),
                                0
                            ]
                        );

                        var loadedMesh = 
                            this.game.wrapPointer(
                                this.game.getPointer(this.workingAssetMemory.base), 
                                this.game.loaded_animated_mesh_asset
                            );

                        this.renderer.loadAnimatedMesh(this.game, loadedMesh);
                        this.onAssetLoaded();
        //            }.bind(this),
        //            function () {
        //                console.log("fetch .text() failed for " + path);
        //            }.bind(this)
        //        );
        //    }.bind(this),
        //    function () {
        //        console.log("Failed to fetch " + path);
        //    }.bind(this)
        //);
    },

    loadBMPFile: function (path, assetType, assetKey, assetSecondKey) {
        fetch(path).then(
            function (file) {
                file.arrayBuffer().then(
                    function (data) {
                        var fileDataView = new Uint8Array(data);
                        //var strLength = this.game.lengthBytesUTF8(data);
                        var numBytes = data.byteLength;
                        var bmpFileData = this.game._malloc(numBytes);
                        var bmpDataView = new Uint8Array(this.game.HEAPU8.buffer, 
                                                         bmpFileData,
                                                         numBytes);
                        bmpDataView.set(fileDataView, 0);

                        this.workingAssetMemory.size = 0;

                        this.game.ccall("parseGameAsset", 
                            "null", 
                            ["number", "number", "number", "number", 
                             "number", "number", "number", "number", "number"], 
                            [
                                bmpFileData, 
                                0,
                                assetType,
                                assetKey,
                                assetSecondKey,
                                this.game.getPointer(this.gameMemory),
                                this.game.getPointer(this.workingAssetMemory),
                                this.game.getPointer(this.platformOptions),
                                0
                            ]
                        );

                        var loadedTexture = 
                            this.game.wrapPointer(
                                this.game.getPointer(this.workingAssetMemory.base), 
                                this.game.loaded_texture_asset
                            );

                        this.renderer.loadTexture(this.game, loadedTexture);
                        this.onAssetLoaded();
                    }.bind(this),
                    function () {
                        console.log("fetch .arrayBuffer() failed for " + path);
                    }.bind(this)
                );
            }.bind(this),
            function () {
                console.log("Failed to fetch " + path);
            }.bind(this)
        );
    },

    loadWAVFile: function (path, assetType, assetKey, assetSecondKey) {
        fetch(path).then(
            function (file) {
                file.arrayBuffer().then(
                    function (data) {
                        var fileDataView = new Uint8Array(data);
                        //var strLength = this.game.lengthBytesUTF8(data);
                        var numBytes = data.byteLength;
                        var wavFileData = this.game._malloc(numBytes);
                        var wavDataView = new Uint8Array(this.game.HEAPU8.buffer, 
                                                         wavFileData,
                                                         numBytes);
                        wavDataView.set(fileDataView, 0);

                        this.workingAssetMemory.size = 0;

                        this.game.ccall("parseGameAsset", 
                            "null", 
                            ["number", "number", "number", "number", 
                             "number", "number", "number", "number", "number"], 
                            [
                                wavFileData, 
                                0,
                                assetType,
                                assetKey,
                                assetSecondKey,
                                this.game.getPointer(this.gameMemory),
                                this.game.getPointer(this.workingAssetMemory),
                                this.game.getPointer(this.platformOptions),
                                0
                            ]
                        );

                        //var loadedTexture = 
                        //    this.game.wrapPointer(
                        //        this.game.getPointer(this.workingAssetMemory.base), 
                        //        this.game.loaded_texture_asset
                        //    );

                        //this.renderer.loadTexture(this.game, loadedTexture);
                        this.onAssetLoaded();
                    }.bind(this),
                    function () {
                        console.log("fetch .arrayBuffer() failed for " + path);
                    }.bind(this)
                );
            }.bind(this),
            function () {
                console.log("Failed to fetch " + path);
            }.bind(this)
        );
    },

    loadDataAsset: function (path, assetType, assetKey, assetSecondKey) {
        fetch(path).then(
            function (file) {
                file.arrayBuffer().then(
                    function (data) {
                        var fileDataView = new Uint8Array(data);
                        //var strLength = this.game.lengthBytesUTF8(data);
                        var numBytes = data.byteLength;
                        var fileData = this.game._malloc(numBytes);
                        var dataView = new Uint8Array(this.game.HEAPU8.buffer, 
                                                         fileData,
                                                         numBytes);
                        dataView.set(fileDataView, 0);

                        this.workingAssetMemory.size = 0;

                        this.game.ccall("parseGameAsset", 
                            "null", 
                            ["number", "number", "number", "number", 
                             "number", "number", "number", "number", "number"], 
                            [
                                fileData, 
                                0,
                                assetType,
                                assetKey,
                                assetSecondKey,
                                this.game.getPointer(this.gameMemory),
                                this.game.getPointer(this.workingAssetMemory),
                                this.game.getPointer(this.platformOptions),
                                numBytes
                            ]
                        );

                        //var loadedTexture = 
                        //    this.game.wrapPointer(
                        //        this.game.getPointer(this.workingAssetMemory.base), 
                        //        this.game.loaded_texture_asset
                        //    );

                        //this.renderer.loadTexture(this.game, loadedTexture);
                        this.onAssetLoaded();
                    }.bind(this),
                    function () {
                        console.log("fetch .arrayBuffer() failed for " + path);
                    }.bind(this)
                );
            }.bind(this),
            function () {
                console.log("Failed to fetch " + path);
            }.bind(this)
        );
    },

    loadTextureAtlas: function (path, assetType, assetKey, assetSecondKey) {
        var bmpPath = path.slice();
        bmpPath = bmpPath.replace(".txt", ".bmp");
        Promise.all([this.fetchTextFile(path), this.fetchBinaryFile(bmpPath)]).then(
            (results) => {
                var txtData = results[0];
                var bmpData = results[1];

                var strLength = this.game.lengthBytesUTF8(txtData);
                var txtFileData = this.game._malloc(strLength + 1);
                this.game.writeAsciiToMemory(txtData, txtFileData); 

                var fileDataView = new Uint8Array(bmpData);
                //var strLength = this.game.lengthBytesUTF8(bmpData);
                var numBytes = bmpData.byteLength;
                var bmpFileData = this.game._malloc(numBytes);
                var bmpDataView = new Uint8Array(this.game.HEAPU8.buffer, 
                                                 bmpFileData,
                                                 numBytes);
                bmpDataView.set(fileDataView, 0);

                this.workingAssetMemory.size = 0;

                this.game.ccall("parseGameAsset", 
                    "null", 
                    ["number", "number", "number", "number", 
                     "number", "number", "number", "number", "number"], 
                    [
                        txtFileData, 
                        bmpFileData, 
                        assetType,
                        assetKey,
                        assetSecondKey,
                        this.game.getPointer(this.gameMemory),
                        this.game.getPointer(this.workingAssetMemory),
                        this.game.getPointer(this.platformOptions),
                        0
                    ]
                );

                var loadedTexture = 
                    this.game.wrapPointer(
                        this.game.getPointer(this.workingAssetMemory.base), 
                        this.game.loaded_texture_asset
                    );

                this.renderer.loadTexture(this.game, loadedTexture);
                this.onAssetLoaded();
            },
            () => {
                console.log("failed to load texture atlas");
            }
        );
    },

    fetchTextFile: function (path) {
        return new Promise((resolve, reject) => {
            fetch(path).then(
                function (file) {
                    file.text().then(
                        function (data) {
                            resolve(data);
                        }.bind(this),
                        function () {
                            console.log("fetch .text() failed for " + path);
                            reject();
                        }.bind(this)
                    );
                }.bind(this),
                function () {
                    console.log("Failed to fetch " + path);
                    reject();
                }.bind(this)
            );
        });
    },

    fetchBinaryFile: function (path) {
        return new Promise((resolve, reject) => {
            fetch(path).then(
                function (file) {
                    file.arrayBuffer().then(
                        function (data) {
                            resolve(data);
                        }.bind(this),
                        function () {
                            console.log("fetch .arrayBuffer() failed for " + path);
                            reject();
                        }.bind(this)
                    );
                }.bind(this),
                function () {
                    console.log("Failed to fetch " + path);
                    reject();
                }.bind(this)
            );
        });
    },

    onAssetLoaded: function () {
        this.totalAssetsLoaded++;
        if (this.totalAssetsLoaded == this.assetList.numAssetsToLoad) {
            this.onAssetsLoaded();
        }
    },

    onAssetsLoaded: function () {
        this.game._free(this.game.getPointer(this.workingAssetMemory.base));
        this.renderCommands = this.game.wrapPointer(this.game._malloc(this.game.sizeof_render_command_list()),          
                                                    this.game.render_command_list);
        var renderCommandMemory = 1 * 1024 * 1024;
        this.renderCommands.memory.base = this.game._malloc(renderCommandMemory);
        this.renderCommands.memory.size = 0;
        this.renderCommands.memory.capacity = renderCommandMemory;

        // input
        this.input = new Input();
        this.gameInput = this.game.wrapPointer(this.game._malloc(this.game.sizeof_game_input()), this.game.game_input);

        document.addEventListener("keydown", this.onKeyDown.bind(this), false);
        document.addEventListener("keyup", this.onKeyUp.bind(this), false);

        var mobileDetect = new MobileDetect(window.navigator.userAgent);
        if (mobileDetect.mobile() || mobileDetect.tablet()) {
            this.canvas.addEventListener("touchstart", this.onTouchStart.bind(this), false);
            this.canvas.addEventListener("touchmove", this.onTouchMove.bind(this), false);
            this.canvas.addEventListener("touchend", this.onTouchEnd.bind(this), false);
        }
        else {
            this.canvas.addEventListener("mousedown", this.onMouseDown.bind(this));
            this.canvas.addEventListener("mouseup", this.onMouseUp.bind(this));
            this.canvas.addEventListener("mousemove", this.onMouseMove.bind(this));
        }

        this.controllers = {};
        window.addEventListener("gamepadconnected", this.onGamepadConnected.bind(this), false);
        window.addEventListener("gamepaddisconnected", this.onGamepadDisconnected.bind(this), false);

        this.resize();
        this.lastTime = window.performance.now();
        this.update();
    },

    update: function () {
        this.gameInput.upKey.down = this.input.keyDown["arrowup"];
        this.gameInput.upKey.justPressed = this.input.keyJustPressed["arrowup"];
        this.gameInput.downKey.down = this.input.keyDown["arrowdown"];
        this.gameInput.downKey.justPressed = this.input.keyJustPressed["arrowdown"];
        this.gameInput.leftKey.down = this.input.keyDown["arrowleft"];
        this.gameInput.leftKey.justPressed = this.input.keyJustPressed["arrowleft"];
        this.gameInput.rightKey.down = this.input.keyDown["arrowright"];
        this.gameInput.rightKey.justPressed = this.input.keyJustPressed["arrowright"];

        this.gameInput.aKey.down = this.input.keyDown["a"];
        this.gameInput.aKey.justPressed = this.input.keyJustPressed["a"];
        this.gameInput.sKey.down = this.input.keyDown["s"];
        this.gameInput.sKey.justPressed = this.input.keyJustPressed["s"];
        this.gameInput.dKey.down = this.input.keyDown["d"];
        this.gameInput.dKey.justPressed = this.input.keyJustPressed["d"];
        this.gameInput.fKey.down = this.input.keyDown["f"];
        this.gameInput.fKey.justPressed = this.input.keyJustPressed["f"];
        this.gameInput.gKey.down = this.input.keyDown["g"];
        this.gameInput.gKey.justPressed = this.input.keyJustPressed["g"];
        this.gameInput.hKey.down = this.input.keyDown["h"];
        this.gameInput.hKey.justPressed = this.input.keyJustPressed["h"];
        this.gameInput.jKey.down = this.input.keyDown["j"];
        this.gameInput.jKey.justPressed = this.input.keyJustPressed["j"];
        this.gameInput.kKey.down = this.input.keyDown["k"];
        this.gameInput.kKey.justPressed = this.input.keyJustPressed["k"];
        this.gameInput.wKey.down = this.input.keyDown["w"];
        this.gameInput.wKey.justPressed = this.input.keyJustPressed["w"];
        this.gameInput.eKey.down = this.input.keyDown["e"];
        this.gameInput.eKey.justPressed = this.input.keyJustPressed["e"];
        this.gameInput.tKey.down = this.input.keyDown["t"];
        this.gameInput.tKey.justPressed = this.input.keyJustPressed["t"];
        this.gameInput.yKey.down = this.input.keyDown["y"];
        this.gameInput.yKey.justPressed = this.input.keyJustPressed["y"];
        this.gameInput.uKey.down = this.input.keyDown["u"];
        this.gameInput.uKey.justPressed = this.input.keyJustPressed["u"];

        this.gameInput.pointerDown = this.input.pointerDown;
        this.gameInput.pointerJustDown = this.input.pointerJustDown;
        this.gameInput.pointerX = this.input.pointerX;
        this.gameInput.pointerY = this.input.pointerY;

        this.gameInput.pointer2Down = this.input.pointer2Down;
        this.gameInput.pointer2JustDown = this.input.pointer2JustDown;
        this.gameInput.pointer2X = this.input.pointer2X;
        this.gameInput.pointer2Y = this.input.pointer2Y;

        // get updated gamepad state
        var gamepads = navigator.getGamepads();
        for (var i = 0; i < gamepads.length; i++) {
            if (gamepads[i]) {
                this.controllers[gamepads[i].index] = gamepads[i];
            }
        }

        var controllerPointer = this.game.getPointer(this.gameInput.controllers);
        for (var i = 0; i < 4; ++i) {
            var gameController = this.game.wrapPointer(controllerPointer + this.game.sizeof_game_controller_input() * i, this.game.game_controller_input);
            if (this.controllers[i]) {
                gameController.connected = true;
                this.updateController(gameController, this.controllers[i]);
            }
            else {
                gameController.connected = false;
            }
        }

        this.renderCommands.windowWidth = this.canvas.width;
        this.renderCommands.windowHeight = this.canvas.height;
        this.renderCommands.memory.size = 0;
        // zero render command memory
        // TODO(ebuchholz): use 32 bit array, it's faster
        // TODO(ebuchholz): fill is slow in some browsers (firefox) so maybe don't zero it, or zero it on webassembly side
        //var uintBuffer = new Uint8Array(this.game.HEAPU8.buffer,
        //                                this.game.getPointer(this.renderCommands.memory.base),
        //                                this.renderCommands.memory.capacity);
        //uintBuffer.fill(0);
        //// zero temp memory
        //uintBuffer = new Uint8Array(this.game.HEAPU8.buffer,
        //                                this.game.getPointer(this.gameMemory.tempMemory),
        //                                this.gameMemory.tempMemoryCapacity);
        //uintBuffer.fill(0);

        // reset platform triggers
        this.platformTriggers.triggerFileWindow = false;
        this.platformTriggers.triggerFileSave = false;
        this.platformTriggers.fileToSaveData = 0;
        this.platformTriggers.fileToSaveSize = 0;

        if (this.loadedFile) {
            this.game.ccall("loadFile", 
                "null", 
                ["number", "number", "number"],
                [
                    this.game.getPointer(this.gameMemory), 
                    this.loadedFile, 
                    this.loadedFileSize
                ]
            );
        }

        this.game.ccall("updateGame", 
            "null", 
            ["number", "number", "number", "number", "number"], 
            [
                this.game.getPointer(this.gameInput), 
                this.game.getPointer(this.gameMemory), 
                this.game.getPointer(this.renderCommands),
                this.game.getPointer(this.platformOptions),
                this.game.getPointer(this.platformTriggers)
            ]
        );

        if (this.loadedFile) {
            this.game._free(this.loadedFile);
            this.loadedFile = null;
        }

        // TODO(ebuchholz): handle interruptions due to resizing, switching tabs, losing focues, etc.
        if (this.webAudioSounds.started) {
            this.webAudioSounds.updateAudio(this.game, this.gameMemory, this.gameSoundOutput, this.soundSamples);
        }

        this.renderer.renderFrame(this.game, this.renderCommands);

        for (var key in this.input.keyJustPressed) {
            if (this.input.keyJustPressed.hasOwnProperty(key)) {
                this.input.keyJustPressed[key] = false;
            }
        }
        this.input.pointerJustDown = false;
        this.input.pointer2JustDown = false;

        if (this.platformTriggers.triggerFileWindow) {
            // web file api
            this.fileInput.click();
            this.fileSelectQueued = true;
        }
        if (this.platformTriggers.triggerFileSave) {
            // web file api
            var uintBuffer = new Uint8Array(this.game.HEAPU8.buffer,
                                            this.platformTriggers.fileToSaveData.ptr,
                                            this.platformTriggers.fileToSaveSize);
            var data = new Blob([uintBuffer]);
            var downloadLinkURL = window.URL.createObjectURL(data);
            var downloadLink = document.createElement("a");
            downloadLink.download = "framedata.txt";
            downloadLink.href = downloadLinkURL;
            downloadLink.click();

            //this.fileInput.click();
        }

        var time = window.performance.now();
        //console.log("frame time: ", time - this.lastTime);
        this.lastTime = time;

        window.requestAnimationFrame(this.updateCallback);
    },

    onFileToLoadSelected: function (event) {
        var fileToLoad = event.target.files[0];
        this.fileSelectQueued = false;

        if (fileToLoad) {
            var reader = new FileReader();
            reader.onload = function (fileLoadedEvent) {
                var data = fileLoadedEvent.target.result;

                var fileDataView = new Uint8Array(data);
                //var strLength = this.game.lengthBytesUTF8(data);
                var numBytes = data.byteLength;
                var fileData = this.game._malloc(numBytes);
                var gameFileDataView = new Uint8Array(this.game.HEAPU8.buffer, 
                                                  fileData,
                                                  numBytes);
                gameFileDataView.set(fileDataView, 0);
                this.loadedFile = fileData;
                this.loadedFileSize = numBytes;
            }.bind(this);
            reader.readAsArrayBuffer(fileToLoad);
        }
    },

    hexColorToString: function (hexColor) {
        var a = ((hexColor >> 24) & 0xff).toString(16);
        var r = ((hexColor >> 16) & 0xff).toString(16);
        if (r.length == 1) { r = "0" + r; }
        var g = ((hexColor >> 8) & 0xff).toString(16);
        if (g.length == 1) { g = "0" + g; }
        var b = ((hexColor) & 0xff).toString(16);
        if (b.length == 1) { b = "0" + b; }
        return "#" + r + g + b;
    },

    resize: function () {
        this.canvas.width = this.viewport.clientWidth;
        this.canvas.height = this.viewport.clientHeight;
        this.canvas.style.width = this.viewport.clientWidth + "px";
        this.canvas.style.height = this.viewport.clientHeight + "px";
    },

    onKeyDown: function (key) {
        var keyName = key.key.toLowerCase();
        if (!this.input.keyDown[keyName]) { 
            this.input.keyJustPressed[keyName] = true; 
        }
        this.input.keyDown[keyName] = true;
    },

    onKeyUp :function (key) {
        var keyName = key.key.toLowerCase();
        this.input.keyDown[keyName] = false;
    },

    onTouchStart: function (e) {
        if (!this.input.pointerDown) {
            this.input.pointerJustDown = true;
        }
        this.input.pointerDown = true;
        this.setMouseXY(e.touches[0].clientX, e.touches[0].clientY);
        if (e.touches.length > 1) {
            if (!this.input.pointer2Down) {
                this.input.pointer2JustDown = true;
            }
            this.input.pointer2Down = true;
            this.setMouseXY2(e.touches[1].clientX, e.touches[1].clientY);
        }
        this.tryStartAudio();
        e.preventDefault();
    },

    tryStartAudio: function () {
        if (!this.webAudioSounds.started) {
            this.webAudioSounds.init(this.game);
            this.webAudioSounds.started = true;

            this.soundSamples = 
                this.game._malloc(this.game.sizeof_sound_sample() * this.webAudioSounds.samplesPerSecond);
            this.gameSoundOutput = 
                this.game.wrapPointer(this.game._malloc(this.game.sizeof_game_sound_output()), 
                                      this.game.game_sound_output);
        }
        // for ios/chrome/etc, whatever browers need touch input to start audio
        this.webAudioSounds.tryUnlockAudioContext();
    },

    onTouchMove: function (e) {
        this.setMouseXY(e.touches[0].clientX, e.touches[0].clientY);
        if (e.touches.length > 1) {
            this.setMouseXY2(e.touches[1].clientX, e.touches[1].clientY);
        }
        e.preventDefault();
    },

    // TODO(ebuchholz): make this work correctly, this works well enough for now
    onTouchEnd: function (e) {
        if (e.changedTouches[0].identifier == 0) {
            this.input.pointerDown = false;
        }
        else {
            this.input.pointer2Down = false;
        }
        if (e.touches.length == 0) {
            this.input.pointerDown = false;
            this.input.pointer2Down = false;
        }
        e.preventDefault();
    },

    onMouseDown: function (e) {
        if (this.fileSelectQueued) {
            this.fileInput.click();
            this.fileSelectQueued = false;
        }
        if (!this.input.pointerDown) {
            this.input.pointerJustDown = true;
        }
        this.input.pointerDown = true;

        this.tryStartAudio();
    },

    onMouseUp: function (e) {
        this.input.pointerDown = false;
    },

    onMouseMove: function (e) {
        this.setMouseXY(e.clientX, e.clientY);
    },

    setMouseXY: function (x, y) {
        let scaleX = this.canvas.width / this.canvas.clientWidth;
        let scaleY = this.canvas.height / this.canvas.clientHeight;
        var mouseX = (x - this.canvas.clientLeft) * scaleX;
        var mouseY = (y - this.canvas.clientTop) * scaleY;
        this.input.pointerX = mouseX;
        this.input.pointerY = mouseY;
    },

    setMouseXY2: function (x, y) {
        let scaleX = this.canvas.width / this.canvas.clientWidth;
        let scaleY = this.canvas.height / this.canvas.clientHeight;
        var mouseX = (x - this.canvas.clientLeft) * scaleX;
        var mouseY = (y - this.canvas.clientTop) * scaleY;
        this.input.pointer2X = mouseX;
        this.input.pointer2Y = mouseY;
    },

    onGamepadConnected: function (e) {
        this.controllers[e.gamepad.index] = e.gamepad;
    },

    onGamepadDisconnected: function (e) {
        delete this.controllers[e.gamepad.index];
    },

    updateController: function (gameController, webController) {
        this.updateControllerButton(webController, 12, gameController, "dPadUp");
        this.updateControllerButton(webController, 13, gameController, "dPadDown");
        this.updateControllerButton(webController, 14, gameController, "dPadLeft");
        this.updateControllerButton(webController, 15, gameController, "dPadRight");

        this.updateControllerButton(webController, 9, gameController, "start");
        this.updateControllerButton(webController, 8, gameController, "back");
        this.updateControllerButton(webController, 10, gameController, "leftStick");
        this.updateControllerButton(webController, 11, gameController, "rightStick");
        this.updateControllerButton(webController, 4, gameController, "leftBumper");
        this.updateControllerButton(webController, 5, gameController, "rightBumper");
        this.updateControllerButton(webController, 0, gameController, "aButton");
        this.updateControllerButton(webController, 1, gameController, "bButton");
        this.updateControllerButton(webController, 2, gameController, "xButton");
        this.updateControllerButton(webController, 3, gameController, "yButton");

        this.updateControllerTrigger(gameController, "leftTrigger", "leftTriggerButton", webController.buttons[6].value);
        this.updateControllerTrigger(gameController, "rightTrigger", "rightTriggerButton", webController.buttons[7].value);

        this.updateControllerStick(webController.axes[0], webController.axes[1], gameController, "leftStickX", "leftStickY", 0.23953978087710196234015930661946);
        this.updateControllerStick(webController.axes[2], webController.axes[3], gameController, "rightStickX", "rightStickY", 0.26517532883693960386974700155644);

        this.updateControllerStickDirection(gameController, "leftStickUp", "leftStickDown", webController.axes[1], 0.23953978087710196234015930661946);
        this.updateControllerStickDirection(gameController, "leftStickLeft", "leftStickRight", webController.axes[0], 0.23953978087710196234015930661946);
        this.updateControllerStickDirection(gameController, "rightStickUp", "rightStickDown", webController.axes[3], 0.26517532883693960386974700155644);
        this.updateControllerStickDirection(gameController, "rightStickLeft", "rightStickRight", webController.axes[2], 0.26517532883693960386974700155644);
    },

    updateControllerButton: function (webController, buttonID, gameController, buttonProperty) {
        gameController[buttonProperty].justPressed = false;
        if (webController.buttons[buttonID].pressed) {
            if (!gameController[buttonProperty].down) {
                gameController[buttonProperty].justPressed = true;
            }
            gameController[buttonProperty].down = true;
        }
        else {
            gameController[buttonProperty].down = false;
        }
    },

    updateControllerTrigger: function (gameController, triggerProperty, triggerButtonProperty, value) {
        gameController[triggerButtonProperty].justPressed = false;
        var deadZone = 0.11764705882352941176470588235294; // attempt to match xinput default deadzone
        if (value >= deadZone) {
            value -= deadZone;
            gameController[triggerProperty] = value / (1.0 - deadZone);
            if (!gameController[triggerButtonProperty].down) {
                gameController[triggerButtonProperty].justPressed = true;
            }
            gameController[triggerButtonProperty].down = true;
        }
        else {
            gameController[triggerProperty] = 0.0;
            gameController[triggerButtonProperty].down = false;
        }
    },

    updateControllerStick: function (xValue, yValue, gameController, xProperty, yProperty, deadZone) {
        var magnitude = Math.sqrt(xValue * xValue + yValue * yValue);
        var normalizedX = xValue / magnitude;
        var normalizedY = yValue / magnitude;

        if (magnitude >= deadZone) {
            if (magnitude > 1.0) {
                magnitude = 1.0;
            }
            magnitude -= deadZone;
            var normalizedMagnitude = magnitude / (1.0 - deadZone);
            gameController[xProperty] = normalizedX * normalizedMagnitude;
            gameController[yProperty] = normalizedY * normalizedMagnitude;
        }
        else {
            gameController[xProperty] = 0.0;
            gameController[yProperty] = 0.0;
        }
    },

    updateControllerStickDirection: function (gameController, dir0Prop, dir1Prop, value, deadZone) {
        gameController[dir0Prop].justPressed = false;
        if (value <= -deadZone) {
            if (!gameController[dir0Prop].down) {
                gameController[dir0Prop].justPressed = true;
            }
            gameController[dir0Prop].down = true;
        }
        else {
            gameController[dir0Prop].down = false;
        }
        gameController[dir1Prop].justPressed = false;
        if (value >= deadZone) {
            if (!gameController[dir1Prop].down) {
                gameController[dir1Prop].justPressed = true;
            }
            gameController[dir1Prop].down = true;
        }
        else {
            gameController[dir1Prop].down = false;
        }
    }

};

var platform = new WebPlatform();
platform.init();
