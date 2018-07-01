/**
* @copyright 2018 - Max Bebök
* @author Max Bebök
* @license GNU-GPLv3 - see the "LICENSE" file in the root directory
*/

const tex = require("./bindings.js");
const fs = require("fs");

const bufferIn = Buffer.from([
    1,2,3,4,5,6,7,8,
    9,1,2,3,4,5,6,7,
    1,2,3,4,5,6,7,8,
    9,1,2,3,4,5,6,7
]);
console.log(bufferIn);

const bufferOut = tex.deswizzle(bufferIn, {
    sizeX: 8,
    sizeY: 8,
    sizeZ: 1,

    index: 4,
    format: 5,
    tileMode: 6,
    swizzle: 7,
    pitch: 8,
    bpp: 9
});

console.log(bufferOut);