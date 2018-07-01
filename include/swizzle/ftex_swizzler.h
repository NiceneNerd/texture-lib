/**
* @copyright 2018 - Max Bebök
* @author Max Bebök
* @license GNU-GPLv3 - see the "LICENSE" file in the root directory
*
* convertet from python script:
* @see http://mk8.tockdom.com/wiki/GTX#Swizzling
*
* Original python Authors:
* @author AddrLib: actual code
* @author Exzap: modifying code to apply to Wii U textures
* @author AboodXD: porting, code improvements and cleaning up
*/

#pragma once

#include "main_header.h"

class FTEX_Swizzler
{
    private:
        // immutable
        u32 m_banks         = 4;
        u32 m_banksBitcount = 2;

        u32 m_pipes         = 2;
        u32 m_pipesBitcount = 1;
        u32 m_pipeInterleaveBytes = 256;
        u32 m_pipeInterleaveBytesBitcount = 8;

        u32 m_rowSize   = 2048;
        u32 m_swapSize  = 256;
        u32 m_splitSize = 2048;

        u32 m_chipFamily      = 2;
        u32 m_microTilePixels = 8 * 8;

        // calculated
        u8 tileThickness;

        u32 macroTileBits;
        u32 macroBytesPerSample;
        u32 macroSamplesPerSlice;
        u32 macroNumSampleSplits;

        inline u8 surfaceGetBitsPerPixel(u32 surfaceFormat);
        inline u8 computeSurfaceThickness(u32 tileMode);

        inline u32 computePixelIndexWithinMicroTile(u32 x, u32 y, u8 bpp, u32 tileMode, u32 z = 0);

        inline u32 computePipeFromCoordWoRotation(u32 x, u32 y);
        inline u32 computeBankFromCoordWoRotation(u32 x, u32 y);

        inline bool isThickMacroTiled(u32 tileMode);
        inline bool isBankSwappedTileMode(u32 tileMode);

        inline u8 computeMacroTileAspectRatio(u32 tileMode);
        inline u32 computeSurfaceBankSwappedWidth(u32 tileMode, u8 bpp, u32 pitch, u32 numSamples = 1);

        inline u32 AddrLib_computeSurfaceAddrFromCoordLinear(u32 x, u32 y, u8 bpp, u32 pitch);
        inline u32 AddrLib_computeSurfaceAddrFromCoordMicroTiled(u32 x, u32 y, u8 bpp, u32 pitch, u32 tileMode);
        inline u32 AddrLib_computeSurfaceAddrFromCoordMacroTiled(u32 x, u32 y, u8 bpp, u32 pitch, u32 sizeZ, u32 tileMode, u8 pipeSwizzle, u8 bankSwizzle);

    public:
        void deswizzle(u32 sizeX, u32 sizeY, u32 sizeZ, u32 index, u16 format_, u32 tileMode, u32 swizzle_, u32 pitch, u8 bpp, 
                       u8* dataIn, u8* dataOut, u32 dataSize);
};
