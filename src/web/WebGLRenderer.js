"use strict";

var defaultVertexShaderSource = require("./shaders/defaultVertexShader.glsl");
var defaultFragmentShaderSource = require("./shaders/defaultFragmentShader.glsl");
var lineVertexShaderSource = require("./shaders/lineVertexShader.glsl");
var lineFragmentShaderSource = require("./shaders/lineFragmentShader.glsl");
var spriteVertexShaderSource = require("./shaders/spriteVertexShader.glsl");
var spriteFragmentShaderSource = require("./shaders/spriteFragmentShader.glsl");

var gl = null;

// NOTE(ebuchholz): these probably should be 0's instead of -1's
var WebGLMesh = function () {
    this.key = -1;
    this.positionBuffer = -1;
    this.texCoordBuffer = -1;
    this.normalBuffer= -1;
    this.indexBuffer= -1;
    this.numIndices = -1;
};

var WebGLTexture = function () {
    this.key = -1;
    this.textureID = -1;
};

var ShaderProgram = function () {
    this.type = -1;
    this.vertexShader = -1;
    this.fragmentShader = -1;
    this.program = -1;
};

var ShaderTypes = {
    DEFAULT: 0,
    LINES: 1,
    SPRITE: 2
};

var WebGLRenderer = function () {
    this.shaders = [];
    this.meshes = [];
    this.textures = [];

    this.viewMatrix = null;
    this.projMatrix = null;

    this.spritePositionBuffer = 0;
    this.spriteTextureBuffer = 0;
    this.spriteColorBuffer = 0;
    this.spriteIndexBuffer = 0;
    
    this.spriteVertexPositions;
    this.spriteTextureCoords;
    this.spriteColors;
    this.spriteIndices;

    this.debugPositionBuffer = 0;
};

WebGLRenderer.MAX_SPRITES_PER_BATCH = 1000;

WebGLRenderer.prototype = {

    initWebGL: function (canvas) {
        this.canvas = canvas;
        gl = canvas.getContext("webgl", { alpha: false });
        var success = gl.getExtension("OES_element_index_uint"); // TODO(ebuchholz): check
        this.compileAndLinkShader(defaultVertexShaderSource, defaultFragmentShaderSource, ShaderTypes.DEFAULT);
        this.compileAndLinkShader(lineVertexShaderSource, lineFragmentShaderSource, ShaderTypes.LINES);
        this.compileAndLinkShader(spriteVertexShaderSource, spriteFragmentShaderSource, ShaderTypes.SPRITE);

        // sprite drawing
        this.spritePositionBuffer = gl.createBuffer();
        this.spriteTextureBuffer = gl.createBuffer();
        this.spriteColorBuffer = gl.createBuffer();
        this.spriteIndexBuffer = gl.createBuffer();

        this.spriteVertexPositions = new Float32Array(WebGLRenderer.MAX_SPRITES_PER_BATCH * 8);
        this.spriteTextureCoords = new Float32Array(WebGLRenderer.MAX_SPRITES_PER_BATCH * 8);
        this.spriteColors = new Float32Array(WebGLRenderer.MAX_SPRITES_PER_BATCH * 16);
        this.spriteIndices = new Uint32Array(WebGLRenderer.MAX_SPRITES_PER_BATCH * 6);

        // index buffer doesn't change
        // TODO(ebuchholz): temporary memory, don't need to keep it in renderer
        var indexIndex = 0;
        for (var i = 0; i < WebGLRenderer.MAX_SPRITES_PER_BATCH; ++i) {
            var vertexNum = i * 4;
            this.spriteIndices[indexIndex++] = vertexNum;
            this.spriteIndices[indexIndex++] = vertexNum+3;
            this.spriteIndices[indexIndex++] = vertexNum+1;
            this.spriteIndices[indexIndex++] = vertexNum;
            this.spriteIndices[indexIndex++] = vertexNum+2;
            this.spriteIndices[indexIndex++] = vertexNum+3;
        }

        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.spriteIndexBuffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, this.spriteIndices, gl.STATIC_DRAW);

        this.debugPositionBuffer = gl.createBuffer();

        // TODO(ebuchholz): use the right blend mode depending on what's being drawn, this is just for default 2d drawing
        gl.enable(gl.BLEND);
        gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
    },

    compileAndLinkShader: function (vertexShaderText, fragmentShaderText, type) {
        var shader = new ShaderProgram();
        shader.type = type;

        // TODO: factor compile code, whole shader build process, shader object
        shader.vertexShader = gl.createShader(gl.VERTEX_SHADER);
        gl.shaderSource(shader.vertexShader, vertexShaderText);
        gl.compileShader(shader.vertexShader);
        if (!gl.getShaderParameter(shader.vertexShader, gl.COMPILE_STATUS)) {
            console.log(gl.getShaderInfoLog(shader.vertexShader));
            return;
        }

        shader.fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
        gl.shaderSource(shader.fragmentShader, fragmentShaderText);
        gl.compileShader(shader.fragmentShader);
        if (!gl.getShaderParameter(shader.fragmentShader, gl.COMPILE_STATUS)) {
            console.log(gl.getShaderInfoLog(shader.fragmentShader));
            return;
        }

        shader.program = gl.createProgram();
        gl.attachShader(shader.program, shader.vertexShader);
        gl.attachShader(shader.program, shader.fragmentShader);
        gl.linkProgram(shader.program);

        if (!gl.getProgramParameter(shader.program, gl.LINK_STATUS)) {
            console.log(gl.getProgramInfoLog(shader.program));
            return;
        }

        this.shaders.push(shader);
    },

    loadMesh: function (game, loadedMesh) {
        var mesh = new WebGLMesh();
        mesh.key = loadedMesh.key;
        this.meshes[mesh.key] = mesh;

        mesh.positionBuffer = gl.createBuffer();
        var floatBuffer = new Float32Array(game.buffer,
                                           loadedMesh.positions.values.ptr, 
                                           loadedMesh.positions.count);
        gl.bindBuffer(gl.ARRAY_BUFFER, mesh.positionBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, floatBuffer, gl.STATIC_DRAW);

        mesh.texCoordBuffer = gl.createBuffer();
        floatBuffer = new Float32Array(game.buffer,
                                       loadedMesh.texCoords.values.ptr, 
                                       loadedMesh.texCoords.count);
        gl.bindBuffer(gl.ARRAY_BUFFER, mesh.texCoordBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, floatBuffer, gl.STATIC_DRAW);

        mesh.normalBuffer = gl.createBuffer();
        floatBuffer = new Float32Array(game.buffer,
                                       loadedMesh.normals.values.ptr, 
                                       loadedMesh.normals.count);
        gl.bindBuffer(gl.ARRAY_BUFFER, mesh.normalBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, floatBuffer, gl.STATIC_DRAW);

        mesh.indexBuffer = gl.createBuffer();
        var uintBuffer = new Uint32Array(game.buffer,
                                         loadedMesh.indices.values.ptr, 
                                         loadedMesh.indices.count);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, uintBuffer, gl.STATIC_DRAW);

        mesh.numIndices = loadedMesh.indices.count;
    },

    loadTexture: function (game, loadedTexture) {
        var texture = new WebGLTexture();
        texture.key = loadedTexture.key;
        this.textures[texture.key] = texture;

        texture.textureID = gl.createTexture();
        var uintBuffer = new Uint8Array(game.buffer,
                                        loadedTexture.pixels.ptr,
                                        loadedTexture.width * loadedTexture.height * 4);
        gl.bindTexture(gl.TEXTURE_2D, texture.textureID);
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, loadedTexture.width, loadedTexture.height, 0, gl.RGBA, gl.UNSIGNED_BYTE, uintBuffer);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
    },

    setCamera: function (setCameraCommand) {
        this.viewMatrix = setCameraCommand.viewMatrix;
        this.projMatrix = setCameraCommand.projMatrix;
    },

    matrix4x4transpose: function (matrixBuffer) {
        var floatBuffer = new Float32Array(16)

        floatBuffer[0] = matrixBuffer[0];
        floatBuffer[1] = matrixBuffer[4];
        floatBuffer[2] = matrixBuffer[8];
        floatBuffer[3] = matrixBuffer[12];

        floatBuffer[4] = matrixBuffer[1];
        floatBuffer[5] = matrixBuffer[5];
        floatBuffer[6] = matrixBuffer[9];
        floatBuffer[7] = matrixBuffer[13];

        floatBuffer[8] = matrixBuffer[2];
        floatBuffer[9] = matrixBuffer[6];
        floatBuffer[10] = matrixBuffer[10];
        floatBuffer[11] = matrixBuffer[14];

        floatBuffer[12] = matrixBuffer[3];
        floatBuffer[13] = matrixBuffer[7];
        floatBuffer[14] = matrixBuffer[11];
        floatBuffer[15] = matrixBuffer[15]

        return floatBuffer;
    },

    drawModel: function (game, modelCommand, program) {
        var mesh = this.meshes[modelCommand.meshKey];

        gl.bindBuffer(gl.ARRAY_BUFFER, mesh.positionBuffer);
        var positionLocation = gl.getAttribLocation(program, "position");
        gl.enableVertexAttribArray(positionLocation);
        gl.vertexAttribPointer(positionLocation, 3, gl.FLOAT, false, 0, 0);

        gl.bindBuffer(gl.ARRAY_BUFFER, mesh.texCoordBuffer);
        var texCoordLocation = gl.getAttribLocation(program, "texCoord");
        gl.enableVertexAttribArray(texCoordLocation);
        gl.vertexAttribPointer(texCoordLocation, 2, gl.FLOAT, false, 0, 0);

        gl.bindBuffer(gl.ARRAY_BUFFER, mesh.normalBuffer);
        var normalLocation = gl.getAttribLocation(program, "normal");
        gl.enableVertexAttribArray(normalLocation);
        gl.vertexAttribPointer(normalLocation, 3, gl.FLOAT, false, 0, 0);

        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);

        // TODO(ebuchholz): figure out how to do this without making new array buffer view every frame
        var floatBuffer = new Float32Array(game.buffer,
                                           modelCommand.modelMatrix.ptr,
                                           16); //4x4 matrix
        var modelMatrixLocation = gl.getUniformLocation(program, "modelMatrix");
        gl.uniformMatrix4fv(modelMatrixLocation, false, this.matrix4x4transpose(floatBuffer));

        floatBuffer = new Float32Array(game.buffer,
                                           this.viewMatrix.ptr,
                                           16); //4x4 matrix
        var viewMatrixLocation = gl.getUniformLocation(program, "viewMatrix");
        gl.uniformMatrix4fv(viewMatrixLocation, false, this.matrix4x4transpose(floatBuffer));

        floatBuffer = new Float32Array(game.buffer,
                                           this.projMatrix.ptr,
                                           16); //4x4 matrix
        var projMatrixLocation = gl.getUniformLocation(program, "projMatrix");
        gl.uniformMatrix4fv(projMatrixLocation, false, this.matrix4x4transpose(floatBuffer));

        var texture = this.textures[modelCommand.textureKey];
        var textureLocation = gl.getUniformLocation(program, "texture");
        gl.uniform1i(textureLocation, 0);
        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, texture.textureID);

        gl.drawElements(gl.TRIANGLES, mesh.numIndices, gl.UNSIGNED_INT, 0);
    },

    drawSprite: function (game, program, spriteCommand, screenWidth, screenHeight) {
        this.spriteVertexPositions[0] = spriteCommand.x;
        this.spriteVertexPositions[1] = spriteCommand.y;
        this.spriteVertexPositions[2] = spriteCommand.x + spriteCommand.width;
        this.spriteVertexPositions[3] = spriteCommand.y;
        this.spriteVertexPositions[4] = spriteCommand.x;
        this.spriteVertexPositions[5] = spriteCommand.y + spriteCommand.height;
        this.spriteVertexPositions[6] = spriteCommand.x + spriteCommand.width;
        this.spriteVertexPositions[7] = spriteCommand.y + spriteCommand.height;

        gl.bindBuffer(gl.ARRAY_BUFFER, this.spritePositionBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, this.spriteVertexPositions, gl.DYNAMIC_DRAW);

        var positionLocation = gl.getAttribLocation(program, "position");
        gl.enableVertexAttribArray(positionLocation);
        gl.vertexAttribPointer(positionLocation, 2, gl.FLOAT, false, 0, 0);

        this.spriteTextureCoords[0] = 0.0;
        this.spriteTextureCoords[1] = 1.0;
        this.spriteTextureCoords[2] = 1.0;
        this.spriteTextureCoords[3] = 1.0;
        this.spriteTextureCoords[4] = 0.0;
        this.spriteTextureCoords[5] = 0.0;
        this.spriteTextureCoords[6] = 1.0;
        this.spriteTextureCoords[7] = 0.0;

        gl.bindBuffer(gl.ARRAY_BUFFER, this.spriteTextureBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, this.spriteTextureCoords, gl.DYNAMIC_DRAW);

        var texCoordLocation = gl.getAttribLocation(program, "texCoord");
        gl.enableVertexAttribArray(texCoordLocation);
        gl.vertexAttribPointer(texCoordLocation, 2, gl.FLOAT, false, 0, 0);

        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.spriteIndexBuffer);

        var screenWidthLocation = gl.getUniformLocation(program, "screenWidth");
        gl.uniform1f(screenWidthLocation, screenWidth);
        var screenHeightLocation = gl.getUniformLocation(program, "screenHeight");
        gl.uniform1f(screenHeightLocation, screenHeight);

        var texture = this.textures[spriteCommand.textureKey];
        var textureLocation = gl.getUniformLocation(program, "texture");
        gl.uniform1i(textureLocation, 0);
        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, texture.textureID);

        gl.drawElements(gl.TRIANGLES, 6, gl.UNSIGNED_INT, 0);
    },

    // TODO(ebuchholz): use buffersubdata instead? and reserve enough for max batch size
    flushSprites: function (game, program, numSpritesBatched, screenWidth, screenHeight, textureKey) {
        gl.bindBuffer(gl.ARRAY_BUFFER, this.spritePositionBuffer);
        var floatBuffer = Float32Array.from(this.spriteVertexPositions);
        gl.bufferData(gl.ARRAY_BUFFER, floatBuffer, gl.DYNAMIC_DRAW);

        var positionLocation = gl.getAttribLocation(program, "position");
        gl.enableVertexAttribArray(positionLocation);
        gl.vertexAttribPointer(positionLocation, 2, gl.FLOAT, false, 0, 0);

        gl.bindBuffer(gl.ARRAY_BUFFER, this.spriteTextureBuffer);
        floatBuffer = Float32Array.from(this.spriteTextureCoords);
        gl.bufferData(gl.ARRAY_BUFFER, floatBuffer, gl.DYNAMIC_DRAW);

        var texCoordLocation = gl.getAttribLocation(program, "texCoord");
        gl.enableVertexAttribArray(texCoordLocation);
        gl.vertexAttribPointer(texCoordLocation, 2, gl.FLOAT, false, 0, 0);

        gl.bindBuffer(gl.ARRAY_BUFFER, this.spriteColorBuffer);
        floatBuffer = Float32Array.from(this.spriteColors);
        gl.bufferData(gl.ARRAY_BUFFER, floatBuffer, gl.DYNAMIC_DRAW);

        var colorCoordLocation = gl.getAttribLocation(program, "color");
        gl.enableVertexAttribArray(colorCoordLocation);
        gl.vertexAttribPointer(colorCoordLocation, 4, gl.FLOAT, false, 0, 0);

        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.spriteIndexBuffer);

        var texture = this.textures[textureKey];

        var screenWidthLocation = gl.getUniformLocation(program, "screenWidth");
        gl.uniform1f(screenWidthLocation, screenWidth);
        var screenHeightLocation = gl.getUniformLocation(program, "screenHeight");
        gl.uniform1f(screenHeightLocation, screenHeight);

        gl.activeTexture(gl.TEXTURE0);
        var textureLocation = gl.getUniformLocation(program, "texture");
        gl.uniform1i(textureLocation, 0);
        gl.bindTexture(gl.TEXTURE_2D, texture.textureID);

        gl.drawElements(gl.TRIANGLES, numSpritesBatched * 6, gl.UNSIGNED_INT, 0);
    },

    drawSpriteList: function (game, program, spriteListCommand, screenWidth, screenHeight) {
        if (spriteListCommand.numSprites > 0) {
            var spritesPointer = game.getPointer(spriteListCommand.sprites);
            var currentTextureKey = game.wrapPointer(spritesPointer, game.render_sprite).textureKey;
            var numSpritesBatched = 0;

            for (var i = 0; i < spriteListCommand.numSprites; ++i) {
                var sprite = game.wrapPointer(spritesPointer + game.sizeof_render_sprite() * i, game.render_sprite);

                if (sprite.textureKey !== currentTextureKey || numSpritesBatched >= WebGLRenderer.MAX_SPRITES_PER_BATCH) {
                    this.flushSprites(game, program, numSpritesBatched, screenWidth, screenHeight, currentTextureKey);
                    numSpritesBatched = 0;
                    currentTextureKey = sprite.textureKey;
                }

                var numFloats = numSpritesBatched * 8;
                var numColors = numSpritesBatched * 16;

                // TODO(ebuchholz): maybe calculate these whole buffers on C side? to avoid copying, and using webidl getters/setters
                var spritePos = new Float32Array(game.buffer, sprite.pos.ptr, 8);
                this.spriteVertexPositions[numFloats]   = spritePos[0];
                this.spriteVertexPositions[numFloats+1] = spritePos[1];
                this.spriteVertexPositions[numFloats+2] = spritePos[2];
                this.spriteVertexPositions[numFloats+3] = spritePos[3];
                this.spriteVertexPositions[numFloats+4] = spritePos[4];
                this.spriteVertexPositions[numFloats+5] = spritePos[5];
                this.spriteVertexPositions[numFloats+6] = spritePos[6];
                this.spriteVertexPositions[numFloats+7] = spritePos[7];

                var spriteTex = new Float32Array(game.buffer, sprite.texCoord.ptr, 8);
                this.spriteTextureCoords[numFloats]   = spriteTex[0];
                this.spriteTextureCoords[numFloats+1] = spriteTex[1];
                this.spriteTextureCoords[numFloats+2] = spriteTex[2];
                this.spriteTextureCoords[numFloats+3] = spriteTex[3];
                this.spriteTextureCoords[numFloats+4] = spriteTex[4];
                this.spriteTextureCoords[numFloats+5] = spriteTex[5];
                this.spriteTextureCoords[numFloats+6] = spriteTex[6];
                this.spriteTextureCoords[numFloats+7] = spriteTex[7];

                var spriteColor = new Float32Array(game.buffer, sprite.color.ptr, 16);
                this.spriteColors[numColors]    = spriteColor[0];
                this.spriteColors[numColors+1]  = spriteColor[1];
                this.spriteColors[numColors+2]  = spriteColor[2];
                this.spriteColors[numColors+3]  = spriteColor[3];
                this.spriteColors[numColors+4]  = spriteColor[4];
                this.spriteColors[numColors+5]  = spriteColor[5];
                this.spriteColors[numColors+6]  = spriteColor[6];
                this.spriteColors[numColors+7]  = spriteColor[7];
                this.spriteColors[numColors+8]  = spriteColor[8];
                this.spriteColors[numColors+9]  = spriteColor[9];
                this.spriteColors[numColors+10] = spriteColor[10];
                this.spriteColors[numColors+11] = spriteColor[11];
                this.spriteColors[numColors+12] = spriteColor[12];
                this.spriteColors[numColors+13] = spriteColor[13];
                this.spriteColors[numColors+14] = spriteColor[14];
                this.spriteColors[numColors+15] = spriteColor[15];

                ++numSpritesBatched;
            }
            if (numSpritesBatched > 0) {
                this.flushSprites(game, program, numSpritesBatched, screenWidth, screenHeight, currentTextureKey);
            }
        }
    },

    drawLines: function (game, lineCommand, program) {
        gl.bindBuffer(gl.ARRAY_BUFFER, this.debugPositionBuffer);

        var floatBuffer = new Float32Array(game.buffer,
                                           lineCommand.lines.ptr, 
                                           lineCommand.numLines * game.sizeof_line());
        gl.bufferData(gl.ARRAY_BUFFER, floatBuffer, gl.DYNAMIC_DRAW);

        var positionLocation = gl.getAttribLocation(program, "position");
        gl.enableVertexAttribArray(positionLocation);
        gl.vertexAttribPointer(positionLocation, 3, gl.FLOAT, false, 0, 0);

        floatBuffer = new Float32Array(game.buffer, this.viewMatrix.ptr, 16); //4x4 matrix
        var viewMatrixLocation = gl.getUniformLocation(program, "viewMatrix");
        gl.uniformMatrix4fv(viewMatrixLocation, false, this.matrix4x4transpose(floatBuffer));

        floatBuffer = new Float32Array(game.buffer, this.projMatrix.ptr, 16); //4x4 matrix
        var projMatrixLocation = gl.getUniformLocation(program, "projMatrix");
        gl.uniformMatrix4fv(projMatrixLocation, false, this.matrix4x4transpose(floatBuffer));

        gl.drawArrays(gl.LINES, 0, lineCommand.numLines * 2);
    },

    renderFrame: function (game, renderCommands) {
        gl.viewport(0, 0, renderCommands.windowWidth, renderCommands.windowHeight);
        //gl.enable(gl.DEPTH_TEST);
        //gl.enable(gl.CULL_FACE);
        gl.clearColor(0.0, 0.7, 0.8, 0.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        var renderCommandOffset = 0;
        var renderCommandMemorySize = renderCommands.memory.size;
        var renderMemoryPointer = game.getPointer(renderCommands.memory.base);
        while (renderCommandOffset < renderCommandMemorySize) {
            var header = game.wrapPointer(renderMemoryPointer + renderCommandOffset, 
                                               game.render_command_header);
            renderCommandOffset += game.sizeof_render_command_header();
            switch (header.type) {
                default:
                    console.log("ERROR: invalid render command type");
                    break;
                case game.RENDER_COMMAND_SPRITE:
                {
                    var program = this.shaders[ShaderTypes.SPRITE].program;
                    gl.useProgram(program);

                    var spriteCommand = game.wrapPointer(renderMemoryPointer + renderCommandOffset, 
                                                         game.render_command_sprite);
                    renderCommandOffset += game.sizeof_render_command_sprite();
                    this.drawSprite(game, program, spriteCommand, renderCommands.windowWidth, renderCommands.windowHeight);
                } break;
                case game.RENDER_COMMAND_SPRITE_LIST: 
                {
                    // TODO(ebuchholz): have a way to not have to set the program for command
                    var program = this.shaders[ShaderTypes.SPRITE].program;
                    gl.useProgram(program);

                    var spriteListCommand = game.wrapPointer(renderMemoryPointer + renderCommandOffset, 
                                                             game.render_command_sprite_list);
                    renderCommandOffset += game.sizeof_render_command_sprite_list();
                    renderCommandOffset += spriteListCommand.numSprites * game.sizeof_render_sprite();
                    this.drawSpriteList(game, program, spriteListCommand, renderCommands.windowWidth, renderCommands.windowHeight);
                } break;
                case game.RENDER_COMMAND_MODEL:
                {
                    var program = this.shaders[ShaderTypes.DEFAULT].program;
                    gl.useProgram(program);

                    var modelCommand = game.wrapPointer(renderMemoryPointer + renderCommandOffset, 
                                                            game.render_command_model);
                    renderCommandOffset += game.sizeof_render_command_model();
                    this.drawModel(game, modelCommand, program);
                } break;
                case game.RENDER_COMMAND_LINES:
                {
                    var program = this.shaders[ShaderTypes.LINES].program;
                    gl.useProgram(program);

                    var lineCommand = game.wrapPointer(renderMemoryPointer + renderCommandOffset, 
                                                       game.render_command_lines);
                    renderCommandOffset += game.sizeof_render_command_lines();
                    renderCommandOffset += game.sizeof_line() * lineCommand.numLines;
                    this.drawLines(game, lineCommand, program);
                } break;
                case game.RENDER_COMMAND_SET_CAMERA:
                {
                    var setCameraCommand = game.wrapPointer(renderMemoryPointer + renderCommandOffset, 
                                                            game.render_command_set_camera);
                    renderCommandOffset += game.sizeof_render_command_set_camera();
                    this.setCamera(setCameraCommand, program);
                } break;
            }
        }
    }

};

module.exports = WebGLRenderer;
