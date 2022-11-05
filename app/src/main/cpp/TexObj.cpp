#include <sstream>
#include <android/log.h>
#include "TexObj.h"
#include "GlObj.h"

/* 画像形式判定 */
std::tuple<bool, int, int, std::vector<char>> TexObj::LoadTexture(std::vector<char> &texbindata) {
    int retImgw = 0, retImgh = 0;
    std::vector<char> retRgbaBuf;

    std::istringstream texbinstream(std::string(texbindata.begin(), texbindata.end()));

    /* フォーマットチェック(BMP) */
    char bm[2] = {0};
    texbinstream.read(bm, sizeof(bm));
    if(bm[0]=='B' && bm[1]=='M') {
        __android_log_print(ANDROID_LOG_INFO, "aaaaa", "BMP形式(%c %c) %s %s(%d)", bm[0], bm[1], __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
        /* BMP読込み */
        auto[w, h, wkbuf] = LoadTextureFromBmp(texbindata);
        retImgw = w;
        retImgh = h;
        retRgbaBuf = std::move(wkbuf);
        return {true, retImgw, retImgh, retRgbaBuf};
    }
    else {
        __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "BMP形式ではない(%c %c) %s %s(%d)", bm[0], bm[1], __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
//        return {false, -1}; /* TGAの可能性がある */
    }

    /* フォーマットチェック(TGA) */
    texbinstream.seekg(-18, std::ios::end );
    char TRUEVISION_TARGA[18] = {0};
    texbinstream.read(TRUEVISION_TARGA, sizeof(TRUEVISION_TARGA));
    if(std::string(TRUEVISION_TARGA).find("TRUEVISION-") == 0) {
        __android_log_print(ANDROID_LOG_INFO, "aaaaa", "TGA形式(%s) %s %s(%d)", TRUEVISION_TARGA, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
        /* TGA読込み */
        auto[w, h, wkbuf] = LoadTextureFromTga(texbindata);
        retImgw = w;
        retImgh = h;
        retRgbaBuf = std::move(wkbuf);
        return {true, retImgw, retImgh, retRgbaBuf};
    }
    else {
        __android_log_print(ANDROID_LOG_INFO, "aaaaa", "TGA形式ではない(%s) %s %s(%d)", TRUEVISION_TARGA, __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
//        return {false, -1}; /* 他の形式の可能性がある */
    }

    if(retRgbaBuf.empty()) {
        __android_log_print(ANDROID_LOG_INFO, "aaaaa", "Unsupported file format. %s %s(%d)", __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
        return {false, 0, 0, {}}; /* 未サポート形式 */
    }

    return {false, 0, 0, {}}; /* 未サポート形式 */
}

/* BMPフォーマット読込み */
std::tuple<int/*幅*/, int/*高さ*/, std::vector<char>/*RGBA*/> TexObj::LoadTextureFromBmp(std::vector<char> &texbindata) {
    std::vector<char> retRgbaBuf;
    int retw = 0, reth = 0;

    std::istringstream texbinstream(std::string(texbindata.begin(), texbindata.end()));
    texbinstream.seekg(0, std::ios::beg );
    BmpFileHeader fileHeader = {0};
    texbinstream.read((char*)&fileHeader, sizeof(BmpFileHeader));
    /* 'B','M'はじまりのチェックは完了している */

    /* ヘッダサイズ(40:Windows Bitmapのサイズ or 12:OS/2 Bitmapのサイズ) */
    int headersize = 0;
    texbinstream.read((char*)&headersize, sizeof(int));
    if(headersize != 40 && headersize != 12) {
        /* フォーマット不正 : 40:Windows Bitmapのサイズ,12:OS/2 Bitmapのサイズのどちらでもない */
        __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "Failed format!! offset2bin=%d(not equal 40 or 12)\n", headersize);
        return {0, 0, retRgbaBuf};
    }

    /* Windows Bitmap(40byte) */
    if(headersize == 40) {
        BitmapInfoHeader bmpinfoheader = {.headersize=headersize};
        texbinstream.read((char*)&(bmpinfoheader.width), sizeof(BitmapInfoHeader)-sizeof(int));
        if(bmpinfoheader.comptype != 0 || bmpinfoheader.bitdepth !=24) {
            /* 無圧縮しかサポートしない, 24bitカラーしか対応しない */
            __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "Not Supported comptype=%d bitdepth=%d\n", bmpinfoheader.comptype, bmpinfoheader.bitdepth);
            return {0, 0, retRgbaBuf};
        }

        retw = bmpinfoheader.width;
        reth = bmpinfoheader.height;
    }
    /* OS/2 Bitmap(12byte) */
    else if(headersize == 12) {
        BitmapCoreHeader bitmapcoreheader = {.headersize=headersize};
        texbinstream.read((char*)&(bitmapcoreheader.width), sizeof(BitmapCoreHeader)-sizeof(int));
        if(bitmapcoreheader.bitdepth !=24) {
            /* 24bitカラーしか対応しない */
            __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "Not Supported bitdepth=%d", bitmapcoreheader.bitdepth);
            return {0, 0, retRgbaBuf};
        }

        retw = bitmapcoreheader.width;
        reth = bitmapcoreheader.height;
    }

    /* BGRデータまで移動 */
    texbinstream.seekg(fileHeader.offset2bin, std::ios::beg);
    /* BGRデータ一括取得 */
    int imagesize3 = retw * reth * 3;
    char* buf = (char*)malloc(imagesize3);
    texbinstream.read(buf, imagesize3);

    /* RGBAデータに変換 */
    int imagesize4 = retw * reth * 4;
    retRgbaBuf.resize(imagesize4);
    for(int rgba = 0, bgr = imagesize3-3;  rgba < imagesize4; rgba+=4,bgr-=3) {
        retRgbaBuf[rgba + 0] = buf[bgr + 2];
        retRgbaBuf[rgba + 1] = buf[bgr + 1];
        retRgbaBuf[rgba + 2] = buf[bgr + 0];
        retRgbaBuf[rgba + 3] = 0xff;
    }
    free(buf);

    return {retw, reth, retRgbaBuf};
}

/* TGAフォーマット読込み */
std::tuple<int/*幅*/, int/*高さ*/, std::vector<char>/*RGBA*/> TexObj::LoadTextureFromTga(std::vector<char> &texbindata) {
    std::vector<char> retRgbaBuf;
    int retw = 0, reth = 0;

    std::istringstream texbinstream(std::string(texbindata.begin(), texbindata.end()));
    texbinstream.seekg(0, std::ios::beg );

    /* ヘッダ読込 */
    TgaHeader tgaheader;
    texbinstream.read((char*)&tgaheader, sizeof(TgaHeader));

    int width = 0, height = 0, bitdepth = 0;

    /* 画像幅/高さ取得 */
    retw = tgaheader.is_width;
    reth = tgaheader.is_height;

    /* RGBA領域確保 */
    retRgbaBuf.resize(retw * reth * 4);

    /* カラーパレット読込み */
    std::unique_ptr<RgbTriple[]> colorpalette = nullptr;
    if(tgaheader.color_map_type) {
        /* 24ビットイメージと32ビットイメージはパレット化されない */
        colorpalette = std::make_unique<RgbTriple[]>(tgaheader.cm_length);
        texbinstream.read((char*)&(colorpalette[0]), sizeof(RgbTriple) * tgaheader.cm_length);
    }

    bool flipvert = true;
    unsigned char *ptr = nullptr;
    switch(tgaheader.image_type) {
        case 0: __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "Not Supported ImageType=%d", tgaheader.image_type); break;  /* not supported */
        case 1: /* COLOR-MAPPED BGR 8 BITS GREYSCALE case 3の処理と同じ */
//              break;
        case 3: /* COLOR-MAPPED BGR 8 BITS */ {
            for(int rowidx = tgaheader.is_height - 1; rowidx >= 0; rowidx-- ) {
                if( flipvert )
                    ptr = (unsigned char*)&(retRgbaBuf[rowidx * tgaheader.is_width * 4 ]);

                for(int col = 0; col < tgaheader.is_width; col++, ptr += 4 ) {
                    // read the current pixel
                    unsigned char coloridx;
                    texbinstream.read((char*)&coloridx, sizeof(unsigned char));

                    // convert indexed pixel (8 bits) into rgba (32 bits) pixel
                    ptr[0] = colorpalette[ coloridx ].rgbtRed;  // b->r
                    ptr[1] = colorpalette[ coloridx ].rgbtGreen;// g->g
                    ptr[2] = colorpalette[ coloridx ].rgbtBlue; // r->b
                    ptr[3] = 0xff;                          // alpha
                }
            }
            break;
        }
        case 2:{
            for(int rowidx = tgaheader.is_height - 1; rowidx >= 0; rowidx-- ) {
                if( flipvert )
                    ptr = (unsigned char*)&(retRgbaBuf[rowidx * tgaheader.is_width * 4]);

                for(int colidx = 0; colidx < tgaheader.is_width; colidx++, ptr += 4 ) {
                    switch( tgaheader.is_pixel_depth ) {
                        case 16: {  // TRUE-COLOR BGR 16 BITS
                            // read the current pixel
                            unsigned char color;
                            texbinstream.read((char*)&color, sizeof(unsigned short));

                            // convert bgr (16 bits) pixel into rgba (32 bits) pixel
                            ptr[0] = ((color & 0x7C00) >> 10) << 3;	// b->r
                            ptr[1] = ((color & 0x03E0) >>  5) << 3;	// g->g
                            ptr[2] = ((color & 0x001F) >>  0) << 3;	// r->b
                            ptr[3] = 255;							// alpha

                            break;
                        }
                        case 24: {  // TRUE-COLOR BGR 24 BITS
                            // convert bgr (24 bits) pixel into rgba (32 bits) pixel
                            RgbTriple pix;
                            texbinstream.read((char*)&pix, sizeof(RgbTriple));

                            ptr[0] = pix.rgbtRed;
                            ptr[1] = pix.rgbtGreen;
                            ptr[2] = pix.rgbtBlue;
                            ptr[3] = 255;

                            break;
                        }
                        case 32: {  // TRUE-COLOR BGR 32 BITS
                            // convert bgr (32 bits) pixel into rgba (32 bits) pixel
                            BgraQuad pix;
                            texbinstream.read((char*)&pix, sizeof(BgraQuad));

                            ptr[0] = pix.bgraRed;
                            ptr[1] = pix.bgraGreen;
                            ptr[2] = pix.bgraBlue;
                            ptr[3] = pix.bgraAlpha;

                            break;
                        }
                    }
                }
            }
            break;
        }
        case 9: // RLE COLOR-MAPPED BGR 8 BITS
        case 11:// RLE COLOR-MAPPED BGR 8 BITS GREYSCALE
        {
            unsigned char packetHeader, packetSize;
            for(int rowidx = tgaheader.is_height - 1; rowidx >= 0; rowidx-- ) {
                if( flipvert )
                    ptr = (unsigned char*)&(retRgbaBuf[rowidx * tgaheader.is_width * 4]);

                for(int col = 0; col < tgaheader.is_width; /* rien */ ) {
                    texbinstream.read((char*)&packetHeader, sizeof(unsigned char));
                    packetSize = 1 + (packetHeader & 0x7f);
                    if( packetHeader & 0x80 ) {
                        // run-length packet
                        // read the current pixel
                        unsigned char coloridx;
                        texbinstream.read((char*)&coloridx, sizeof(unsigned char));

                        // convert indexed pixel (8 bits) pixel into rgba (32 bits) pixel
                        for(int lpi = 0; lpi < packetSize; lpi++, ptr += 4, col++ ) {
                            ptr[0] = colorpalette[ coloridx ].rgbtRed;     // b->r
                            ptr[1] = colorpalette[ coloridx ].rgbtGreen;   // g->g
                            ptr[2] = colorpalette[ coloridx ].rgbtBlue;    // r->b
                            ptr[3] = 255;                           // alpha
                        }
                    }
                    else {
                        // non run-length packet
                        for(int lpi = 0; lpi < packetSize; lpi++, ptr += 4, col++ ) {
                            // read the current pixel
                            unsigned char coloridx;
                            texbinstream.read((char*)&coloridx, sizeof(unsigned char));

                            // convert indexed pixel (8 bits) pixel into rgba (32 bits) pixel
                            ptr[0] = colorpalette[ coloridx ].rgbtRed;		// b->r
                            ptr[1] = colorpalette[ coloridx ].rgbtGreen;	// g->g
                            ptr[2] = colorpalette[ coloridx ].rgbtBlue;		// r->b
                            ptr[3] = 255;							// alpha
                        }
                    }
                }
            }
            break;
        }
        case 10: {
            unsigned char packetHeader, packetSize;
            for(int rowidx = tgaheader.is_height - 1; rowidx >= 0; rowidx-- ) {
                if( flipvert )
                    ptr = (unsigned char*)&(retRgbaBuf[rowidx * tgaheader.is_width * 4]);

                for(int colidx = 0; colidx < tgaheader.is_width; /* rien */ ) {
                    texbinstream.read((char*)&packetHeader, sizeof(unsigned char));
                    packetSize		= 1 + (packetHeader & 0x7f);
                    if( packetHeader & 0x80 ) {
                        // run-length packet
                        switch( tgaheader.is_pixel_depth ) {
                            case 16: {  // RLE TRUE-COLOR BGR 16 BITS
                                // read the current pixel
                                unsigned short color;
                                texbinstream.read((char*)&color, sizeof(unsigned short));

                                // convert bgr (16 bits) pixel into rgba (32 bits) pixel
                                for(int lpi = 0; lpi < packetSize; lpi++, ptr += 4, colidx++ ) {
                                    ptr[0] = ((color & 0x7C00) >> 10) << 3;	// b->r
                                    ptr[1] = ((color & 0x03E0) >>  5) << 3;	// g->g
                                    ptr[2] = ((color & 0x001F) >>  0) << 3;	// r->b
                                    ptr[3] = 255;
                                }
                                break;
                            }

                            case 24: {  // RLE TRUE-COLOR BGR 24 BITS
                                // convert bgr (24 bits) pixel into rgba (32 bits) pixel
                                RgbTriple pix;
                                texbinstream.read((char*)&pix, sizeof(RgbTriple));

                                for(int lpi = 0; lpi < packetSize; lpi++, ptr += 4, colidx++ ) {
                                    ptr[0] = pix.rgbtRed;
                                    ptr[1] = pix.rgbtGreen;
                                    ptr[2] = pix.rgbtBlue;
                                    ptr[3] = 255;
                                }
                                break;
                            }

                            case 32: {  // RLE TRUE-COLOR BGR 32 BITS
                                // convert bgr (32 bits) pixel into rgba (32 bits) pixel
                                BgraQuad pix;
                                texbinstream.read((char*)&pix, sizeof(BgraQuad));

                                for(int lpi = 0; lpi < packetSize; lpi++, ptr += 4, colidx++ ) {
                                    ptr[0] = pix.bgraRed;
                                    ptr[1] = pix.bgraGreen;
                                    ptr[2] = pix.bgraBlue;
                                    ptr[3] = pix.bgraAlpha;
                                }
                                break;
                            }
                        }
                    }
                    else {
                        // non run-length packet
                        for(int lpi = 0; lpi < packetSize; lpi++, ptr += 4, colidx++ ) {
                            switch( tgaheader.is_pixel_depth ) {
                                case 16: {  // RLE TRUE-COLOR BGR 16 BITS
                                    // read the current pixel
                                    unsigned short color;
                                    texbinstream.read((char*)&color, sizeof(unsigned short));

                                    // convert bgr (16 bits) pixel into rgba (32 bits) pixel
                                    ptr[0] = ((color & 0x7C00) >> 10) << 3;	// b->r
                                    ptr[1] = ((color & 0x03E0) >>  5) << 3;	// g->g
                                    ptr[2] = ((color & 0x001F) >>  0) << 3;	// r->b
                                    ptr[3] = 255;							// alpha

                                    break;
                                }

                                case 24: {  // RLE TRUE-COLOR BGR 24 BITS
                                    // convert bgr (24 bits) pixel into rgba (32 bits) pixel
                                    RgbTriple pix;
                                    texbinstream.read((char*)&pix, sizeof(RgbTriple));

                                    ptr[0] = pix.rgbtRed;
                                    ptr[1] = pix.rgbtGreen;
                                    ptr[2] = pix.rgbtBlue;
                                    ptr[3] = 255;

                                    break;
                                }

                                case 32: {  // RLE TRUE-COLOR BGR 32 BITS
                                    // convert bgr (32 bits) pixel into rgba (32 bits) pixel
                                    BgraQuad pix;
                                    texbinstream.read((char*)&pix, sizeof(BgraQuad));

                                    ptr[0] = pix.bgraRed;
                                    ptr[1] = pix.bgraGreen;
                                    ptr[2] = pix.bgraBlue;
                                    ptr[3] = pix.bgraAlpha;

                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        default: {
            __android_log_print(ANDROID_LOG_ERROR, "aaaaa", "Unknown format!! ImageType=%d", tgaheader.image_type);
            return {0, 0, retRgbaBuf};
        }
    }

    colorpalette.reset();

    return {retw, reth, retRgbaBuf};
}
