var webpackConfig = require("./webpack.config.js");

webpackConfig.mode = "production";
webpackConfig.devtool = undefined;
module.exports = webpackConfig;
