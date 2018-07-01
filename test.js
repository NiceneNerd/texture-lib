/**
* @copyright 2018 - Max Bebök
* @author Max Bebök
* @license GNU-GPLv3 - see the "LICENSE" file in the root directory
*/

const tex = require("./bindings.js");
//const tex = require('./build/Debug/texture.node');

const fs = require("fs");
/*
const bufferIn = Buffer.from([
    1,2,3,4,5,6,7,8,
    9,1,2,3,4,5,6,7,
    1,2,3,4,5,6,7,8,
    9,1,2,3,4,5,6,7
]);
*/
const bufferIn = fs.readFileSync("./testBuffer.bin");

console.log(bufferIn);

const params = {
    "sizeX": 128,
    "sizeY": 128,
    "sizeZ": 1,
    "index": 0,
    "format": 1073,
    "tileMode": 4,
    "swizzle": 65536,
    "pitch": 32,
    "bpp": 64
};

const bufferOut = tex.deswizzle(bufferIn, params);

console.log("OUT");
console.log(bufferOut);