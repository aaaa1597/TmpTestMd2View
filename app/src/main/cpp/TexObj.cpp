#include <sstream>
#include <android/log.h>
#include "TexObj.h"
#include "GlObj.h"

/* 画像形式判定 */
std::tuple<bool, int, int, std::vector<char>> TexObj::LoadTexture(std::vector<char> &texbindata) {
    int retImgw = 0, retImgh = 0;
    std::vector<char> retRgbaBuf;

    std::istringstream texbinstream(std::string(texbindata.begin(), texbindata.end()));

    /* フォーマットチェック(JPG) */
    /* future plan */

    /* フォーマットチェック(PNG) */
    /* future plan */

    /* フォーマットチェック(GIF) */
    /* future plan */

    /* フォーマットチェック(PSD) */
    /* future plan */

    /* フォーマットチェック(PIC) */
    /* future plan */

    /* フォーマットチェック(PNM) */
    /* future plan */

    /* フォーマットチェック(HDR) */
    /* future plan */

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
        auto[w, h, wkbuf] = LoadTextureFromTga(texbindata, EComponentType::rgb_alpha);
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
std::tuple<int/*幅*/, int/*高さ*/, std::vector<char>/*RGBA*/> TexObj::LoadTextureFromTga(std::vector<char> &texbindata, EComponentType reqType) {
    std::vector<char> retRgbaBuf;
    int retw = 0, reth = 0;

    std::istringstream texbinstream(std::string(texbindata.begin(), texbindata.end()));
    texbinstream.seekg(0, std::ios::beg );

    /* ヘッダ読込 */
    TgaHeader tgaheader;
    texbinstream.read((char*)&tgaheader, sizeof(TgaHeader));

    /* do a tiny bit of precessing */
    bool isRLE = false;
    if ( tgaheader.image_type >= 8 ) {
        tgaheader.image_type -= 8;
        isRLE = true;
    }

    /* int tga_alpha_bits = tga_inverted & 15; */
    tgaheader.is_image_descriptor =  1 - ((tgaheader.is_image_descriptor >> 5) & 1);

    if( (tgaheader.is_width < 1) || (tgaheader.is_height < 1) ||
        (tgaheader.image_type < 1) || (tgaheader.image_type > 3) ||
        ((tgaheader.is_bits_per_pixel != 8) && (tgaheader.is_bits_per_pixel != 16) &&
         (tgaheader.is_bits_per_pixel != 24) && (tgaheader.is_bits_per_pixel != 32))    ) {
        __android_log_print(ANDROID_LOG_INFO, "aaaaa", "未サポート TGAファイル!! %s %s(%d)", __PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);
        retw = 0, reth = 0;
        std::vector<char>().swap(retRgbaBuf);
        return {retw, reth, retRgbaBuf};
    }

    /* パレットを使用している場合の、パレットのビット数 */
    int retComponent = tgaheader.is_bits_per_pixel / 8;
    if ( tgaheader.has_color_map ) {
        retComponent = tgaheader.cm_palette_bits / 8;
    }

    /* 画像幅/高さ取得 */
    retw = tgaheader.is_width;
    reth = tgaheader.is_height;

    /* RGBA領域確保 */
    retRgbaBuf.resize(retw * reth * retComponent);

    /* IDフィールド長分読み飛ばし(たいていは0) */
    texbinstream.seekg(tgaheader.id_length, std::ios::cur );

    /* カラーパレット読込み */
    std::unique_ptr<unsigned char[]> colorpalette = nullptr;
    if(!tgaheader.has_color_map && !isRLE) {
        for(int lpct=0; lpct < tgaheader.is_height; ++lpct) {
            int y = tgaheader.is_image_descriptor ? tgaheader.is_height - lpct - 1 : lpct;
            char *tga_row = &retRgbaBuf[y*tgaheader.is_width*retComponent];
            texbinstream.read(tga_row, tgaheader.is_width * retComponent);
        }
    }
    else {
        if(tgaheader.has_color_map) {
            /* colormap開始位置まで読み飛ばし */
            texbinstream.seekg(tgaheader.cm_palette_start, std::ios::cur );

            /* colorパレットの一括読込み */
            colorpalette = std::make_unique<unsigned char[]>(tgaheader.cm_palette_len * tgaheader.cm_palette_bits / 8);
            texbinstream.read((char*)&(colorpalette[0]), tgaheader.cm_palette_len * tgaheader.cm_palette_bits / 8);
        }

        /* 実データ読込み */
        int RLE_count = 0;
        int RLE_repeating = 0;
        bool read_next_pixel = true;
        for(int lpi = 0; lpi < tgaheader.is_width * tgaheader.is_height; ++lpi) {
            if ( isRLE ) {
                if ( RLE_count == 0 ) {
                    //   yep, get the next byte as a RLE command
                    char charRLE_cmd = -1;
                    int RLE_cmd;
                    texbinstream.read((char*)&(charRLE_cmd), 1);
                    RLE_cmd = charRLE_cmd;
                    RLE_count = 1 + (RLE_cmd & 127);

                    RLE_repeating = RLE_cmd >> 7;
                    read_next_pixel = true;
                }
                else if ( !RLE_repeating ) {
                    read_next_pixel = true;
                }
            }
            else {
                read_next_pixel = true;
            }

            /* pixel読込み */
            unsigned char wkrawdata[4] = {0};
            if( read_next_pixel) {
                if(tgaheader.has_color_map) {
                    /* 1バイト読込み → チェック */
                    char char_pal_idx;
                    texbinstream.read((char*)&(char_pal_idx), 1);
                    int pal_idx = char_pal_idx;
                    if( pal_idx >= tgaheader.cm_palette_len ) {
                        /* invalid index */
                        pal_idx = 0;
                    }
                    pal_idx *= (tgaheader.is_bits_per_pixel / 8);
                    for(int lpj = 0; lpj*8 < tgaheader.is_bits_per_pixel; ++lpj) {
                        wkrawdata[lpj] = colorpalette[pal_idx + lpj];
                    }
                }
                else {
                    /* データ読込み */
                    for(int lpj = 0; lpj*8 < tgaheader.is_bits_per_pixel; ++lpj) {
                        texbinstream.read((char*)&(wkrawdata[lpj]), 1);
                    }
                }
                /*  次pixel読込みフラグの初期化 */
                read_next_pixel = 0;
            }   /* pixel読込み */

            /* 取得データを設定 */
            for(int lpj = 0; lpj < retComponent; ++lpj)
                retRgbaBuf[lpi*retComponent+lpj] = wkrawdata[lpj];

            /* in case we're in RLE mode, keep counting down */
            --RLE_count;
        }

        //   do I need to invert the image?
        if( tgaheader.is_image_descriptor ) {
            for (int lpj = 0; lpj*2 < tgaheader.is_height; ++lpj) {
                int index1 = lpj * tgaheader.is_width * retComponent;
                int index2 = (tgaheader.is_height - 1 - lpj) * tgaheader.is_width * retComponent;
                for(int lpi = tgaheader.is_width * retComponent; lpi > 0; --lpi) {
                    unsigned char temp = retRgbaBuf[index1];
                    retRgbaBuf[index1] = retRgbaBuf[index2];
                    retRgbaBuf[index2] = temp;
                    ++index1;
                    ++index2;
                }
            }
        }
    }

    /* clear color palette */
    colorpalette.reset();

    /* swap RGB */
    if(retComponent >= 3) {
        unsigned char *tga_pixel = (unsigned char *)&(retRgbaBuf[0]);
        for(int lpi=0; lpi < tgaheader.is_width * tgaheader.is_height; ++lpi) {
            unsigned char temp = tga_pixel[0];
            tga_pixel[0] = tga_pixel[2];
            tga_pixel[2] = temp;
            tga_pixel += retComponent;
        }
    }

    // convert to target component count
    if(static_cast<int>(reqType) && (static_cast<int>(reqType) != retComponent))
        retRgbaBuf = TexObj::convertFormat(retRgbaBuf, retComponent, static_cast<int>(reqType), tgaheader.is_width, tgaheader.is_height);

    return {retw, reth, retRgbaBuf};
}

std::vector<char> TexObj::convertFormat(const std::vector<char> &rgbadata, int nowcomp, int reqcomp, unsigned int width, unsigned int height) {
    std::vector<char> retRbgaBuf;

    /* 引数チェック */
    if(reqcomp < 1 || 4 < reqcomp)
        return retRbgaBuf;
    if (reqcomp == nowcomp) {
        retRbgaBuf = rgbadata;
        return retRbgaBuf;
    }

    retRbgaBuf.resize(reqcomp * width * height);

    for(int lpj=0; lpj < (int)height; ++lpj) {
        const unsigned char *src  = reinterpret_cast<const unsigned char*>(&(rgbadata[lpj * width * nowcomp]));
        unsigned char *dest = reinterpret_cast<unsigned char*>(&(retRbgaBuf[lpj * width * reqcomp]));

#define COMBO(a,b)  ((a)*8+(b))
#define CASE(a,b)   case COMBO(a,b): for(int lpi=width-1; lpi >= 0; --lpi, src += a, dest += b)
        // convert source image with nowcomp components to one with reqcomp components;
        // avoid switch per pixel, so use switch per scanline and massive macros
        switch (COMBO(nowcomp, reqcomp)) {
            CASE(1,2) dest[0]=src[0], dest[1]=255; break;
            CASE(1,3) dest[0]=dest[1]=dest[2]=src[0]; break;
            CASE(1,4) dest[0]=dest[1]=dest[2]=src[0], dest[3]=255; break;
            CASE(2,1) dest[0]=src[0]; break;
            CASE(2,3) dest[0]=dest[1]=dest[2]=src[0]; break;
            CASE(2,4) dest[0]=dest[1]=dest[2]=src[0], dest[3]=src[1]; break;
            CASE(3,4) dest[0]=src[0],dest[1]=src[1],dest[2]=src[2],dest[3]=255; break;
            CASE(3,1) dest[0]=TexObj::Compute_y(src[0],src[1],src[2]); break;
            CASE(3,2) dest[0]=TexObj::Compute_y(src[0],src[1],src[2]), dest[1] = 255; break;
            CASE(4,1) dest[0]=TexObj::Compute_y(src[0],src[1],src[2]); break;
            CASE(4,2) dest[0]=TexObj::Compute_y(src[0],src[1],src[2]), dest[1] = src[3]; break;
            CASE(4,3) dest[0]=src[0],dest[1]=src[1],dest[2]=src[2]; break;
            default: return retRbgaBuf;
        }
#undef CASE
    }

    return retRbgaBuf;
}

unsigned char TexObj::Compute_y(int r, int g, int b) {
    return (unsigned char)(((r*77) + (g*150) +  (29*b)) >> 8);
}

