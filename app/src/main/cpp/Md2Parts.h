#ifndef CPPMD2VIEWER_MD2PARTS_H
#define CPPMD2VIEWER_MD2PARTS_H

/* md2 header */
struct md2header {
    int magicnumber;/* Magic Number. "IPD2"固定 */
    int version;    /* md2 version. 現在は8. */

    int skinwidth;  /* textureサイズ(width) */
    int skinheight; /* textureサイズ(height) */

    int framesize;  /* 1フレームのサイズ[bytes] */

    int num_skins;      /* texture数 */
    int num_vertexs;    /* 頂点数(3角形とかn角形とか) */
    int num_st;         /* UV座標数 */
    int num_polys;      /* ポリゴン数 */
    int num_glcmds;     /* openglコマンド数 */
    int num_totalframes;/* 総フレーム数 */

    int offset_skins;   /* テクスチャ名までのoffset(64 bytes) */
    int offset_st;      /* uv座標までのoffset(s-t) */
    int offset_meshs;   /* mesh情報までのoffset */
    int offset_frames;  /* フレームデータまでのoffset */
    int offset_glcmds;  /* openglコマンドまでのoffset */
    int offset_end;     /* ファイルの終端 */
};

struct vertex {
    float v[3];
};

struct texstcoord {
    float s;
    float t;
};

struct texindex {
    short s;
    short t;
};

struct mesh {
    unsigned short meshIndex[3];
    unsigned short stIndex[3];
};

struct framePoint_t {
    unsigned char v[3];
    unsigned char normalIndex;
};

struct frame {
    float scale[3];
    float translate[3];
    char name[16];  /* ← 何者か不明,けっこう文字化けする,どっちにしても未使用 */
    framePoint_t fp[1];
};

#endif //CPPMD2VIEWER_MD2PARTS_H
