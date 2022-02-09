var win_rw_helper = {},
  fs = require("fs"),
  child_process = require("child_process"),
  os = require("os"),
  path = require("path"),
  binding_path = path.resolve(__dirname, "./win_rw.node"),
  win_rw_helper;

if (fs.existsSync(binding_path)) {
  win_rw_helper = require(binding_path);
} else {
  win_rw_helper = require("./win_rw_" +
    process.platform +
    "_" +
    process.arch +
    ".node");
}

module.exports.readPath = readPath;
module.exports.writePath = writePath;

function readPath(path) {
  return win_rw_helper.readPath(path);
}

function writePath(path, buffer) {
  return win_rw_helper.writePath(path, buffer);
}