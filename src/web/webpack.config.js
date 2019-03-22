var webpack = require('webpack');
var CopyWebpackPlugin = require('copy-webpack-plugin');
var WriteFileWebpackPlugin = require('write-file-webpack-plugin');
var path = require('path');

module.exports = {
    entry: './main.js',
    output: { 
        filename: "quickmake.js",
        path: path.join(__dirname, "../../build/web")
    },
    node: {
        fs: "empty"
    },
    module: {
        rules: [
            {
                test: /game\.js$/,
                loader: "exports-loader"
            },
            {
                test: /\.glsl$/,
                loader: "raw-loader"
            }
        ]
    },
    mode: "development",
    plugins: [
        new WriteFileWebpackPlugin(),
        new CopyWebpackPlugin([
            { from: 'index.html', to: '../../build/web/index.html' },
            { from: '../data', to: '../../build/web' },
            { from: 'wasm/game.wasm', to: '../../build/web/game.wasm' },
        ])
    ],
    resolve: {
        extensions: [ '.js' ],
        modules: [ 'node_modules' ],
    },
    devtool: 'source-map'
};
