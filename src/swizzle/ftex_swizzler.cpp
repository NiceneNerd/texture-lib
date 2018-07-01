/**
* @copyright 2018 - Max Bebök
* @author Max Bebök
* @license GNU-GPLv3 - see the "LICENSE" file in the root directory
*
* convertet from this python script into JS, and then into C++:
* @see http://mk8.tockdom.com/wiki/GTX#Swizzling
*
* Original python Authors:
* @author AddrLib: actual code
* @author Exzap: modifying code to apply to Wii U textures
* @author AboodXD: porting, code improvements and cleaning up
*/

#include "../../include/main_header.h"
#undef max
#undef min

#include <algorithm>

const u8 bankSwapOrder[] = {0, 1, 3, 2, 6, 7, 5, 4, 0, 0};

u16 BCn_formats[] = {0x31, 0x431, 0x32, 0x432, 0x33, 0x433, 0x34, 0x234, 0x35, 0x235};
size_t BCn_formats_size = sizeof(BCn_formats) / sizeof(u16);

const u8 formatHwInfo[] = {
    0x00,0x00,0x00,0x01,0x08,0x03,0x00,0x01,0x08,0x01,0x00,0x01,0x00,0x00,0x00,0x01,
    0x00,0x00,0x00,0x01,0x10,0x07,0x00,0x00,0x10,0x03,0x00,0x01,0x10,0x03,0x00,0x01,
    0x10,0x0B,0x00,0x01,0x10,0x01,0x00,0x01,0x10,0x03,0x00,0x01,0x10,0x03,0x00,0x01,
    0x10,0x03,0x00,0x01,0x20,0x03,0x00,0x00,0x20,0x07,0x00,0x00,0x20,0x03,0x00,0x00,
    0x20,0x03,0x00,0x01,0x20,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x03,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x20,0x03,0x00,0x01,0x00,0x00,0x00,0x01,
    0x00,0x00,0x00,0x01,0x20,0x0B,0x00,0x01,0x20,0x0B,0x00,0x01,0x20,0x0B,0x00,0x01,
    0x40,0x05,0x00,0x00,0x40,0x03,0x00,0x00,0x40,0x03,0x00,0x00,0x40,0x03,0x00,0x00,
    0x40,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x80,0x03,0x00,0x00,0x80,0x03,0x00,0x00,
    0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x10,0x01,0x00,0x00,
    0x10,0x01,0x00,0x00,0x20,0x01,0x00,0x00,0x20,0x01,0x00,0x00,0x20,0x01,0x00,0x00,
    0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x60,0x01,0x00,0x00,
    0x60,0x01,0x00,0x00,0x40,0x01,0x00,0x01,0x80,0x01,0x00,0x01,0x80,0x01,0x00,0x01,
    0x40,0x01,0x00,0x01,0x80,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

void FTEX_Swizzler::deswizzle(u32 sizeX, u32 sizeY, u32 sizeZ, u32 index, u16 format_, u32 tileMode, u32 swizzle, u32 pitch, u8 bpp, u8* dataIn, u8* dataOut, u32 dataSize)
{
    u32 singleBufferSize = dataSize / sizeZ;
    u32 bufferStart = index * singleBufferSize;
    u8* data = dataIn + bufferStart;

    u16* BCn_formats_end = BCn_formats + BCn_formats_size;
    if(std::find(BCn_formats, BCn_formats_end, format_) != BCn_formats_end) //!!
    {
        sizeX = (sizeX + 3) / 4;
        sizeY = (sizeY + 3) / 4;
    }
    
    u32 pipeSwizzle = (swizzle >> 8) & 1;
    u32 bankSwizzle = (swizzle >> 9) & 3;

    if(sizeZ > 0)
    {
        // every 4 textures, the only correct texture is the first, fix for the rest here:
        bankSwizzle = (index % 4);
    }

    for(u32 y=0; y<sizeY; ++y)
    {
        for(u32 x=0; x<sizeX; ++x)
        {
            u32 pos;
            if(tileMode == 0 || tileMode == 1)
            {
                pos = AddrLib_computeSurfaceAddrFromCoordLinear(x, y, bpp, pitch);
            }else if(tileMode == 2 || tileMode == 3){
                pos = AddrLib_computeSurfaceAddrFromCoordMicroTiled(x, y, bpp, pitch, tileMode);
            }else{
                pos = AddrLib_computeSurfaceAddrFromCoordMacroTiled(x, y, bpp, pitch, sizeZ, tileMode, pipeSwizzle, bankSwizzle);
            }

            u8 bpp2 = bpp;
            bpp2 = bpp2 / 8;

            u32 pos_ = (y * sizeX + x) * bpp2;

            if((pos_ < dataSize) && (pos < dataSize))
            {
                for(u32 p=0; p<bpp2; ++p)
                    dataOut[pos_ + p] = data[pos + p];
            }
        }
    }
}

u8 FTEX_Swizzler::surfaceGetBitsPerPixel(u32 surfaceFormat)
{
    u32 hwFormat = surfaceFormat & 0x3F;
    return formatHwInfo[hwFormat * 4 + 0];
}


u8 FTEX_Swizzler::computeSurfaceThickness(u32 tileMode)
{
    if(tileMode == 3 || tileMode == 7 || tileMode == 11 || tileMode == 13 || tileMode == 15)
        return 4;
    else if(tileMode == 16 || tileMode == 17)
        return 8;

    return 1;
}

u32 FTEX_Swizzler::computePixelIndexWithinMicroTile(u32 x, u32 y, u8 bpp, u32 tileMode, u32 z)
{
    u8 thickness = computeSurfaceThickness(tileMode);

    u8 pixelBit0 = x & 1;
    u8 pixelBit1 = (x & 2) >> 1;
    u8 pixelBit2 = y & 1;
    u8 pixelBit3 = (x & 4) >> 2;
    u8 pixelBit4 = (y & 2) >> 1;
    u8 pixelBit5 = (y & 4) >> 2;
    u8 pixelBit6 = 0;
    u8 pixelBit7 = 0;
    u8 pixelBit8 = 0;

    if(bpp == 0x08)
    {
        pixelBit0 = x & 1;
        pixelBit1 = (x & 2) >> 1;
        pixelBit2 = (x & 4) >> 2;
        pixelBit3 = (y & 2) >> 1;
        pixelBit4 = y & 1;
        pixelBit5 = (y & 4) >> 2;

    }else if(bpp == 0x10)
    {
        pixelBit0 = x & 1;
        pixelBit1 = (x & 2) >> 1;
        pixelBit2 = (x & 4) >> 2;
        pixelBit3 = y & 1;
        pixelBit4 = (y & 2) >> 1;
        pixelBit5 = (y & 4) >> 2;

    }else if(bpp == 0x20 || bpp == 0x60){
        pixelBit0 = x & 1;
        pixelBit1 = (x & 2) >> 1;
        pixelBit2 = y & 1;
        pixelBit3 = (x & 4) >> 2;
        pixelBit4 = (y & 2) >> 1;
        pixelBit5 = (y & 4) >> 2;

    }else if(bpp == 0x40){
        pixelBit0 = x & 1;
        pixelBit1 = y & 1;
        pixelBit2 = (x & 2) >> 1;
        pixelBit3 = (x & 4) >> 2;
        pixelBit4 = (y & 2) >> 1;
        pixelBit5 = (y & 4) >> 2;

    }else if(bpp == 0x80){
        pixelBit0 = y & 1;
        pixelBit1 = x & 1;
        pixelBit2 = (x & 2) >> 1;
        pixelBit3 = (x & 4) >> 2;
        pixelBit4 = (y & 2) >> 1;
        pixelBit5 = (y & 4) >> 2;
    }

    if(thickness > 1)
    {
        pixelBit6 = z & 1;
        pixelBit7 = (z & 2) >> 1;
    }

    if(thickness == 8)
        pixelBit8 = (z & 4) >> 2;

    return (
        (pixelBit8 << 8) | (pixelBit7 << 7) | (pixelBit6 << 6)
        | 32 * pixelBit5 | 16 * pixelBit4   | 8 * pixelBit3
        |  4 * pixelBit2 | pixelBit0        | 2 * pixelBit1
    );
}

u32 FTEX_Swizzler::computePipeFromCoordWoRotation(u32 x, u32 y)
{
    return ((y >> 3) ^ (x >> 3)) & 1; // hardcoded to assume 2 pipes
}

u32 FTEX_Swizzler::computeBankFromCoordWoRotation(u32 x, u32 y)
{
    u32 numPipes = m_pipes;
    u32 numBanks = m_banks;
    u32 bank = 0;

    if(numBanks == 4)
    {
        u8 bankBit0 = ((y / (16 * numPipes)) ^ (x >> 3) ) & 1;
        bank = bankBit0 | 2 * (( (y / (8 * numPipes)) ^ (x >> 4)) & 1);
    }
    else if(numBanks == 8)
    {
        u8 bankBit0a = ( (y / (32 * numPipes)) ^ (x >> 3)) & 1;
        bank = (bankBit0a | 2 * (( (y / (32 * numPipes)) ^ (y / (16 * numPipes) ^ (x >> 4))) & 1) |
            4 * (( (y / (8 * numPipes)) ^ (x >> 5)) & 1));
    }

    return bank;
}

bool FTEX_Swizzler::isThickMacroTiled(u32 tileMode)
{
    return (tileMode == 7 || tileMode == 11 || tileMode == 13 || tileMode == 15);
}

bool FTEX_Swizzler::isBankSwappedTileMode(u32 tileMode)
{
    return (tileMode == 8 || tileMode == 9 || tileMode == 10 || tileMode == 11 || tileMode == 14 || tileMode == 15);
}

u8 FTEX_Swizzler::computeMacroTileAspectRatio(u32 tileMode)
{
    if(tileMode == 8 || tileMode == 12 || tileMode == 14)
        return 1;
    else if(tileMode == 5 || tileMode == 9)
        return 2;
    else if(tileMode == 6 || tileMode == 10)
        return 4;

    return 1;
}

u32 FTEX_Swizzler::computeSurfaceBankSwappedWidth(u32 tileMode, u8 bpp, u32 pitch, u32 numSamples)
{
    if(!isBankSwappedTileMode(tileMode))
        return 0;

    u32 numBanks  = m_banks;
    u32 numPipes  = m_pipes;
    u32 swapSize  = m_swapSize;
    u32 rowSize   = m_rowSize;
    u32 splitSize = m_splitSize;
    u32 groupSize = m_pipeInterleaveBytes;
    u32 bytesPerSample = 8 * bpp;

    u32 samplesPerTile = splitSize / bytesPerSample;
    u32 slicesPerTile  = samplesPerTile == 0 ? 1 : std::max((u32)1, numSamples / samplesPerTile);

    if(isThickMacroTiled(tileMode))
        numSamples = 4;

    u32 bytesPerTileSlice = numSamples * (bytesPerSample / slicesPerTile);

    u8 factor = computeMacroTileAspectRatio(tileMode);
    u32 swapTiles = std::max((u32)1, (swapSize >> 1) / bpp);

    u32 swapWidth   = swapTiles * 8 * numBanks;
    u32 heightBytes = numSamples * factor * numPipes * (bpp / slicesPerTile);
    u32 swapMax     = numPipes * numBanks * (rowSize / heightBytes);
    u32 swapMin     = groupSize * 8 * (numBanks / bytesPerTileSlice);

    u32 bankSwapWidth = std::min(swapMax, std::max(swapMin, swapWidth));
    u32 doublePitch = 2 * pitch;

    while(!(bankSwapWidth < doublePitch))
        bankSwapWidth >>= 1;

    return bankSwapWidth;
}

u32 FTEX_Swizzler::AddrLib_computeSurfaceAddrFromCoordLinear(u32 x, u32 y, u8 bpp, u32 pitch)
{
    u32 rowOffset = y * pitch;
    u32 pixOffset = x;
    u32 addr = (rowOffset + pixOffset) * bpp;

    return addr / 8;
}

u32 FTEX_Swizzler::AddrLib_computeSurfaceAddrFromCoordMicroTiled(u32 x, u32 y, u8 bpp, u32 pitch, u32 tileMode)
{
    u8 microTileThickness = 1;

    if(tileMode == 3)
        microTileThickness = 4;

    u32 microTileBytes = (m_microTilePixels * microTileThickness * bpp + 7) / 8;
    u32 microTilesPerRow = pitch >> 3;
    u32 microTileIndexX = x >> 3;
    u32 microTileIndexY = y >> 3;

    u32 microTileOffset = microTileBytes * (microTileIndexX + microTileIndexY * microTilesPerRow);

    auto pixelIndex = computePixelIndexWithinMicroTile(x, y, bpp, tileMode);

    u32 pixelOffset = bpp * pixelIndex;
    pixelOffset >>= 3;

    return pixelOffset + microTileOffset;
}

u32 FTEX_Swizzler::AddrLib_computeSurfaceAddrFromCoordMacroTiled(u32 x, u32 y, u8 bpp, u32 pitch, u32 sizeZ, u32 tileMode, u8 pipeSwizzle, u8 bankSwizzle)
{
    u32 numPipes = m_pipes;
    u32 numBanks = m_banks;
    u32 numGroupBits = m_pipeInterleaveBytesBitcount;
    u32 numPipeBits = m_pipesBitcount;
    u32 numBankBits = m_banksBitcount;

    auto microTileThickness = computeSurfaceThickness(tileMode);

    u32 microTileBits = bpp * (microTileThickness * m_microTilePixels);
    u32 microTileBytes = (microTileBits + 7) / 8;

    auto pixelIndex  = computePixelIndexWithinMicroTile(x, y, bpp, tileMode);
    u32 pixelOffset = bpp * pixelIndex;
    u32 elemOffset  = pixelOffset;

    u32 bytesPerSample = microTileBytes;
    u32 numSamples = 1;
    u32 sampleSlice = 0;

    if(microTileBytes > m_splitSize)
    {
        u32 samplesPerSlice = m_splitSize / bytesPerSample;
        u32 numSampleSplits = std::max((u32)1, 1 / samplesPerSlice);
        numSamples = samplesPerSlice;
        sampleSlice = elemOffset / (microTileBits / numSampleSplits);
        elemOffset %= microTileBits / numSampleSplits;
    }

    elemOffset += 7;
    elemOffset = elemOffset / 8;

    auto pipe = computePipeFromCoordWoRotation(x, y);
    auto bank = computeBankFromCoordWoRotation(x, y);

    u32 bankPipe = pipe + numPipes * bank;
    u32 swizzle_ = pipeSwizzle + numPipes * bankSwizzle;

    bankPipe ^= numPipes * sampleSlice * ((numBanks >> 1) + 1) ^ swizzle_;
    bankPipe %= numPipes * numBanks;
    pipe = bankPipe % numPipes;
    bank = bankPipe / numPipes;

    u32 sliceBytes = (sizeZ * pitch * microTileThickness * bpp * numSamples + 7) / 8;
    u32 sliceOffset = sliceBytes * (sampleSlice / microTileThickness);

    u32 macroTilePitch = 8 * m_banks;
    u32 macroTileHeight = 8 * m_pipes;

    if(tileMode == 5 || tileMode == 9) // GX2_TILE_MODE_2D_TILED_THIN4 and GX2_TILE_MODE_2B_TILED_THIN2
    {
        macroTilePitch >>= 1;
        macroTileHeight *= 2;
    }
    else if(tileMode == 6 || tileMode == 10) // GX2_TILE_MODE_2D_TILED_THIN4 and GX2_TILE_MODE_2B_TILED_THIN4
    {
        macroTilePitch >>= 2;
        macroTileHeight *= 4;
    }

    u32 macroTilesPerRow = pitch / macroTilePitch;
    u32 macroTileBytes = (numSamples * microTileThickness * bpp * macroTileHeight * macroTilePitch + 7) / 8;
    u32 macroTileIndexX = x / macroTilePitch;
    u32 macroTileIndexY = y / macroTileHeight;
    u32 macroTileOffset = (macroTileIndexX + macroTilesPerRow * macroTileIndexY) * macroTileBytes;

    if(tileMode == 8 || tileMode == 9 || tileMode == 10 || tileMode == 11 || tileMode == 14 || tileMode == 15)
    {
        auto bankSwapWidth = computeSurfaceBankSwappedWidth(tileMode, bpp, pitch);
        u32 swapIndex = macroTilePitch * (macroTileIndexX / bankSwapWidth);
        bank ^= bankSwapOrder[swapIndex & (m_banks - 1)];
    }

    u32 groupMask = ((1 << numGroupBits) - 1);

    u32 numSwizzleBits = (numBankBits + numPipeBits);

    u32 totalOffset = (elemOffset + ((macroTileOffset + sliceOffset) >> numSwizzleBits));

    u32 offsetHigh = (totalOffset & ~groupMask) << numSwizzleBits;
    u32 offsetLow = groupMask & totalOffset;

    u32 pipeBits = pipe << numGroupBits;
    u32 bankBits = bank << (numPipeBits + numGroupBits);

    return bankBits | pipeBits | offsetLow | offsetHigh;
}
