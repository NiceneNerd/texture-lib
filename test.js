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
console.time("load");
const bufferIn = fs.readFileSync("./testBuffer.bin");
const bufferRef = fs.readFileSync("./testBuffer.out.bin");
console.timeEnd("load");

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

let bufferOut;

console.time("swizzle");
for(let i=0; i<500; ++i)
{
    bufferOut = Buffer.alloc(bufferIn.length);
    tex.deswizzle(bufferIn, bufferOut, params);
}

console.timeEnd("swizzle");

console.log(bufferOut);
console.log(bufferOut.compare(bufferRef) == 0 ? "SUCCESS" : "ERROR");
