"use strict";

var MobileDetect = require("mobile-detect");

var Game = require("./wasm/game.js");
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
};


WebPlatform.prototype = {

    init: function () {
        this.game = Game();
        this.game.onRuntimeInitialized = this.run.bind(this);
    },

    run: function () {
        this.viewport = document.getElementById("run-around-game");
        this.viewport.style["touch-action"] = "none";
        this.viewport.style["overflow"] = "hidden";
        this.canvas = document.createElement("canvas");
        this.viewport.insertAdjacentElement("afterbegin", this.canvas);
        this.viewport.style.display = "block";

        this.canvas.width = 960;
        this.canvas.height = 540;
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
                case this.game.ASSET_TYPE_OBJ: 
                {
                    this.loadOBJFile(assetToLoad.path, assetToLoad.type, assetToLoad.key);
                } break;
                case this.game.ASSET_TYPE_BMP: 
                {
                    this.loadBMPFile(assetToLoad.path, assetToLoad.type, assetToLoad.key);
                } break;
            }
        }

    },

    loadOBJFile: function (path, assetType, assetKey) {
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
                            ["number", "number", "number", "number", "number"], 
                            [
                                objFileData, 
                                assetType,
                                assetKey,
                                this.game.getPointer(this.gameMemory),
                                this.game.getPointer(this.workingAssetMemory)
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

    loadBMPFile: function (path, assetType, assetKey) {
        fetch(path).then(
            function (file) {
                file.arrayBuffer().then(
                    function (data) {
                        console.log();
                        var fileDataView = new Uint8Array(data);
                        //var strLength = this.game.lengthBytesUTF8(data);
                        var numBytes = data.byteLength;
                        var bmpFileData = this.game._malloc(numBytes);
                        var bmpDataView = new Uint8Array(this.game.buffer, 
                                                         bmpFileData,
                                                         numBytes);
                        bmpDataView.set(fileDataView, 0);

                        this.workingAssetMemory.size = 0;

                        this.game.ccall("parseGameAsset", 
                            "null", 
                            ["number", "number", "number", "number", "number"], 
                            [
                                bmpFileData, 
                                assetType,
                                assetKey,
                                this.game.getPointer(this.gameMemory),
                                this.game.getPointer(this.workingAssetMemory)
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

        this.resize();
        this.update();
    },

    update: function () {

        this.gameInput.forwardButton = this.input.keyDown["w"];
        this.gameInput.backButton = this.input.keyDown["s"];
        this.gameInput.leftButton = this.input.keyDown["a"];
        this.gameInput.rightButton = this.input.keyDown["d"];
        this.gameInput.upButton = this.input.keyDown[" "];
        this.gameInput.downButton = this.input.keyDown["shift"];
        this.gameInput.turnUpButton = this.input.keyDown["arrowup"];
        this.gameInput.turnDownButton = this.input.keyDown["arrowdown"];
        this.gameInput.turnLeftButton = this.input.keyDown["arrowleft"];
        this.gameInput.turnRightButton = this.input.keyDown["arrowright"];

        this.gameInput.pointerDown = this.input.pointerDown;
        this.gameInput.pointerJustDown = this.input.pointerJustDown;
        this.gameInput.pointerX = this.input.pointerX;
        this.gameInput.pointerY = this.input.pointerY;

        this.gameInput.pointer2Down = this.input.pointer2Down;
        this.gameInput.pointer2JustDown = this.input.pointer2JustDown;
        this.gameInput.pointer2X = this.input.pointer2X;
        this.gameInput.pointer2Y = this.input.pointer2Y;

        this.renderCommands.memory.size = 0;
        // zero render command memory
        var uintBuffer = new Uint8Array(this.game.buffer,
                                        this.game.getPointer(this.renderCommands.memory.base),
                                        this.renderCommands.memory.capacity);
        uintBuffer.fill(0);
        this.game.ccall("updateGame", 
            "null", 
            ["number", "number", "number"], 
            [
                this.game.getPointer(this.gameInput), 
                this.game.getPointer(this.gameMemory), 
                this.game.getPointer(this.renderCommands)
            ]
        );
        this.renderer.renderFrame(this.game, this.renderCommands);

        for (var key in this.input.keyJustPressed) {
            if (this.input.hasOwnProperty(key)) {
                this.input.keyJustPressed[key] = false;
            }
        }
        this.input.pointerJustDown = false;
        this.input.pointer2JustDown = false;

        window.requestAnimationFrame(this.updateCallback);
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
        e.preventDefault();
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
        if (!this.input.pointerDown) {
            this.input.pointerJustDown = true;
        }
        this.input.pointerDown = true;
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
    }

};

var platform = new WebPlatform();
platform.init();
