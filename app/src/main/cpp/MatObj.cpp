#include <functional>
#include <array>
#include <cmath>
#include "MatObj.h"

const std::array<float, 16> Mat44::IDENTITY = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
};

/* 投影行列を生成 */
std::array<float, 16> Mat44::getPerspectivef(float fovy, float aspect, float near, float far) {
    std::array<float, 16> retMat = {};

    float n2 = near + near;
    float bottom= (float)tan(fovy / 2.0f * PI / 180.0) * near;
    float top   = -bottom;
    float right = aspect * bottom;
    float left  = -right;
    float xx = 1.0f / (left - right);
    float yy = 1.0f / (bottom - top);
    float zz = 1.0f / (near - far);

#define M(row, col) retMat [col * 4 + row]

    M(0, 0) = n2 * xx;
    M(0, 1) = 0.0f;
    M(0, 2) = (right + left) * xx;
    M(0, 3) = 0.0f;

    M(1, 0) = 0.0f;
    M(1, 1) = n2 * yy;
    M(1, 2) = (top + bottom) * yy;
    M(1, 3) = 0.0f;

    M(2, 0) = 0.0f;
    M(2, 1) = 0.0f;
    M(2, 2) = -(far + near) * zz;
    M(2, 3) = -(n2 * far) * zz;

    M(3, 0) = 0.0f;
    M(3, 1) = 0.0f;
    M(3, 2) = -1.0f;
    M(3, 3) = 0.0f;

#undef M

    return retMat;
}

/* 行列の掛け算(外積とは違う) */
std::array<float, 16> Mat44::multMatrixf(const std::array<float, 16> &a, const std::array<float, 16> &m) {
    std::array<float, 16> retmat = {};

#define A(row, col) a[(col << 2) + row]
#define M(row, col) m[(col << 2) + row]
#define MAT(row, col) retmat[(col << 2) + row]

    for (int i = 0; i < 4; i++) {
        MAT(i, 0) = A(i, 0) * M(0, 0) + A(i, 1) * M(1, 0) + A(i, 2) * M(2, 0) + A(i, 3) * M(3, 0);
        MAT(i, 1) = A(i, 0) * M(0, 1) + A(i, 1) * M(1, 1) + A(i, 2) * M(2, 1) + A(i, 3) * M(3, 1);
        MAT(i, 2) = A(i, 0) * M(0, 2) + A(i, 1) * M(1, 2) + A(i, 2) * M(2, 2) + A(i, 3) * M(3, 2);
        MAT(i, 3) = A(i, 0) * M(0, 3) + A(i, 1) * M(1, 3) + A(i, 2) * M(2, 3) + A(i, 3) * M(3, 3);
    }

#undef A
#undef M
#undef MAT
    return retmat;
}

/* setLookAtf */
std::array<float, 16> Mat44::getLookAtf(const std::array<float, 3> &camerapos,
                                        const std::array<float, 3> &targetpos,
                                        const std::array<float, 3> &uppos) {
    std::array<float, 16> retMat = {};
    std::array<float, 3> view0 = {targetpos[0]-camerapos[0],
                                 targetpos[1]-camerapos[1],
                                 targetpos[2]-camerapos[2]};
    std::array<float, 3> up0 = uppos;

    std::array<float, 3> view1= Vec3::normalize(view0);
    std::array<float, 3> up1  = Vec3::normalize(up0);

    std::array<float, 3> side0 = Vec3::cross(view1, up1);
    std::array<float, 3> side1 = Vec3::normalize(side0);

    std::array<float, 3> up2 = Vec3::cross(side1, view1);

#define M(row, col) retMat[(col << 2) + row]

    M(0, 0) = side1[0];
    M(0, 1) = side1[1];
    M(0, 2) = side1[2];
    M(0, 3) = 0.0f;

    M(1, 0) = up2[0];
    M(1, 1) = up2[1];
    M(1, 2) = up2[2];
    M(1, 3) = 0.0f;

    M(2, 0) = -view1[0];
    M(2, 1) = -view1[1];
    M(2, 2) = -view1[2];
    M(2, 3) = 0.0f;

    M(3, 0) = 0.0f;
    M(3, 1) = 0.0f;
    M(3, 2) = 0.0f;
    M(3, 3) = 1.0f;

#undef M

    retMat = Mat44::translatef(retMat, {-camerapos[0], -camerapos[1], -camerapos[2]});

    return retMat;
}

/* 行列を移動 */
std::array<float, 16> Mat44::translatef(const std::array<float, 16> &mat, const std::array<float, 3> &vec) {
    std::array<float, 16> retmat = mat;
    retmat[12] = mat[0] * vec[0] + mat[4] * vec[1] + mat[ 8] * vec[2] + mat[12];
    retmat[13] = mat[1] * vec[0] + mat[5] * vec[1] + mat[ 9] * vec[2] + mat[13];
    retmat[14] = mat[2] * vec[0] + mat[6] * vec[1] + mat[10] * vec[2] + mat[14];
    retmat[15] = mat[3] * vec[0] + mat[7] * vec[1] + mat[11] * vec[2] + mat[15];
    return retmat;
}

/* 行列を拡縮 */
std::array<float, 16> Mat44::scalef(const std::array<float, 16> &mat, const std::array<float, 3> &vec) {
    std::array<float, 16> retmat = mat;
    for (int i=0 ; i<4 ; i++) {
        retmat[	    i] = mat[	  i] * vec[0];	/* x */
        retmat[ 4 + i] = mat[ 4 + i] * vec[1];	/* y */
        retmat[ 8 + i] = mat[ 8 + i] * vec[2];	/* z */
    }
    return retmat;
}

/* 回転行列を取得 */
/* @param rm returns the result */
/* @param a angle to rotate in degrees */
/* @param x X axis component */
/* @param y Y axis component */
/* @param z Z axis component */
std::array<float, 16> Mat44::getRotatef(float angle, float x, float y, float z) {
    std::array<float, 16> rm = {};
    rm[3] = 0;
    rm[7] = 0;
    rm[11]= 0;
    rm[12]= 0;
    rm[13]= 0;
    rm[14]= 0;
    rm[15]= 1;
    angle *= (float) (PI / 180.0f);
    float s = (float) std::sin(angle);
    float c = (float) std::cos(angle);
    if (1.0f == x && 0.0f == y && 0.0f == z) {
        rm[5] = c;   rm[10]= c;
        rm[6] = s;   rm[9] = -s;
        rm[1] = 0;   rm[2] = 0;
        rm[4] = 0;   rm[8] = 0;
        rm[0] = 1;
    } else if (0.0f == x && 1.0f == y && 0.0f == z) {
        rm[0] = c;   rm[10]= c;
        rm[8] = s;   rm[2] = -s;
        rm[1] = 0;   rm[4] = 0;
        rm[6] = 0;   rm[9] = 0;
        rm[5] = 1;
    } else if (0.0f == x && 0.0f == y && 1.0f == z) {
        rm[0] = c;   rm[5] = c;
        rm[1] = s;   rm[4] = -s;
        rm[2] = 0;   rm[6] = 0;
        rm[8] = 0;   rm[9] = 0;
        rm[10]= 1;
    } else {
        float len = std::sqrt(x*x + y*y + z*z);
        if (1.0f != len) {
            float recipLen = 1.0f / len;
            x *= recipLen;
            y *= recipLen;
            z *= recipLen;
        }
        float nc = 1.0f - c;
        float xy = x * y;
        float yz = y * z;
        float zx = z * x;
        float xs = x * s;
        float ys = y * s;
        float zs = z * s;
        rm[ 0] = x*x*nc +  c;
        rm[ 4] =  xy*nc - zs;
        rm[ 8] =  zx*nc + ys;
        rm[ 1] =  xy*nc + zs;
        rm[ 5] = y*y*nc +  c;
        rm[ 9] =  yz*nc - xs;
        rm[ 2] =  zx*nc - ys;
        rm[ 6] =  yz*nc + xs;
        rm[10] = z*z*nc +  c;
    }

    return rm;
}

/* 逆行列を求める */
std::array<float, 16> Mat44::invertf(const std::array<float, 16> &matrix) {
    std::array<float, 16> retmat = {};

    /* 転置行列 */
    float src0  = matrix[ 0];
    float src4  = matrix[ 1];
    float src8  = matrix[ 2];
    float src12 = matrix[ 3];

    float src1  = matrix[ 4];
    float src5  = matrix[ 5];
    float src9  = matrix[ 6];
    float src13 = matrix[ 7];

    float src2  = matrix[ 8];
    float src6  = matrix[ 9];
    float src10 = matrix[10];
    float src14 = matrix[11];

    float src3  = matrix[12];
    float src7  = matrix[13];
    float src11 = matrix[14];
    float src15 = matrix[15];

    /* calculate pairs for first 8 elements (cofactors) */
    float atmp0  = src10 * src15;
    float atmp1  = src11 * src14;
    float atmp2  = src9  * src15;
    float atmp3  = src11 * src13;
    float atmp4  = src9  * src14;
    float atmp5  = src10 * src13;
    float atmp6  = src8  * src15;
    float atmp7  = src11 * src12;
    float atmp8  = src8  * src14;
    float atmp9  = src10 * src12;
    float atmp10 = src8  * src13;
    float atmp11 = src9  * src12;

    /* calculate first 8 elements (cofactors) */
    float dst0  = (atmp0 * src5 + atmp3 * src6 + atmp4  * src7)
                  - (atmp1 * src5 + atmp2 * src6 + atmp5  * src7);
    float dst1  = (atmp1 * src4 + atmp6 * src6 + atmp9  * src7)
                  - (atmp0 * src4 + atmp7 * src6 + atmp8  * src7);
    float dst2  = (atmp2 * src4 + atmp7 * src5 + atmp10 * src7)
                  - (atmp3 * src4 + atmp6 * src5 + atmp11 * src7);
    float dst3  = (atmp5 * src4 + atmp8 * src5 + atmp11 * src6)
                  - (atmp4 * src4 + atmp9 * src5 + atmp10 * src6);
    float dst4  = (atmp1 * src1 + atmp2 * src2 + atmp5  * src3)
                  - (atmp0 * src1 + atmp3 * src2 + atmp4  * src3);
    float dst5  = (atmp0 * src0 + atmp7 * src2 + atmp8  * src3)
                  - (atmp1 * src0 + atmp6 * src2 + atmp9  * src3);
    float dst6  = (atmp3 * src0 + atmp6 * src1 + atmp11 * src3)
                  - (atmp2 * src0 + atmp7 * src1 + atmp10 * src3);
    float dst7  = (atmp4 * src0 + atmp9 * src1 + atmp10 * src2)
                  - (atmp5 * src0 + atmp8 * src1 + atmp11 * src2);

    /* calculate pairs for second 8 elements (cofactors) */
    float btmp0  = src2 * src7;
    float btmp1  = src3 * src6;
    float btmp2  = src1 * src7;
    float btmp3  = src3 * src5;
    float btmp4  = src1 * src6;
    float btmp5  = src2 * src5;
    float btmp6  = src0 * src7;
    float btmp7  = src3 * src4;
    float btmp8  = src0 * src6;
    float btmp9  = src2 * src4;
    float btmp10 = src0 * src5;
    float btmp11 = src1 * src4;

    /* calculate second 8 elements (cofactors) */
    float dst8  = (btmp0  * src13 + btmp3  * src14 + btmp4  * src15)
                  - (btmp1  * src13 + btmp2  * src14 + btmp5  * src15);
    float dst9  = (btmp1  * src12 + btmp6  * src14 + btmp9  * src15)
                  - (btmp0  * src12 + btmp7  * src14 + btmp8  * src15);
    float dst10 = (btmp2  * src12 + btmp7  * src13 + btmp10 * src15)
                  - (btmp3  * src12 + btmp6  * src13 + btmp11 * src15);
    float dst11 = (btmp5  * src12 + btmp8  * src13 + btmp11 * src14)
                  - (btmp4  * src12 + btmp9  * src13 + btmp10 * src14);
    float dst12 = (btmp2  * src10 + btmp5  * src11 + btmp1  * src9 )
                  - (btmp4  * src11 + btmp0  * src9  + btmp3  * src10);
    float dst13 = (btmp8  * src11 + btmp0  * src8  + btmp7  * src10)
                  - (btmp6  * src10 + btmp9  * src11 + btmp1  * src8 );
    float dst14 = (btmp6  * src9  + btmp11 * src11 + btmp3  * src8 )
                  - (btmp10 * src11 + btmp2  * src8  + btmp7  * src9 );
    float dst15 = (btmp10 * src10 + btmp4  * src8  + btmp9  * src9 )
                  - (btmp8  * src9  + btmp11 * src10 + btmp5  * src8 );

    /* calculate determinant */
    float det = src0 * dst0 + src1 * dst1 + src2 * dst2 + src3 * dst3;

    if(fabsf(det - 0.0f) < FLT_EPSILON)
        return Mat44::IDENTITY;

    /* calculate matrix inverse */
    float invdet = 1.0f / det;
    retmat[ 0] = dst0  * invdet;
    retmat[ 1] = dst1  * invdet;
    retmat[ 2] = dst2  * invdet;
    retmat[ 3] = dst3  * invdet;

    retmat[ 4] = dst4  * invdet;
    retmat[ 5] = dst5  * invdet;
    retmat[ 6] = dst6  * invdet;
    retmat[ 7] = dst7  * invdet;

    retmat[ 8] = dst8  * invdet;
    retmat[ 9] = dst9  * invdet;
    retmat[10] = dst10 * invdet;
    retmat[11] = dst11 * invdet;

    retmat[12] = dst12 * invdet;
    retmat[13] = dst13 * invdet;
    retmat[14] = dst14 * invdet;
    retmat[15] = dst15 * invdet;

    return retmat;
}

/* 転置行列を取得 */
std::array<float, 16> Mat44::transposef(const std::array<float, 16> &matrix) {
    std::array<float, 16> retmat = {};
    for (int i = 0; i < 4; i++) {
        int mBase = i * 4;
        retmat[i	 ] = matrix[mBase	];
        retmat[i + 4 ] = matrix[mBase + 1];
        retmat[i + 8 ] = matrix[mBase + 2];
        retmat[i + 12] = matrix[mBase + 3];
    }

    return retmat;
}

/* ベクトルを正規化 */
std::array<float, 3> Vec3::normalize(const std::array<float, 3> &vec) {
    std::array<float, 3> retvec = vec;

    float l = (float)sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    if (l == 0.0f)
        return retvec;

    retvec[0] /= l;
    retvec[1] /= l;
    retvec[2] /= l;

    return retvec;
}

/* ベクトル同士を外積(ベクトルのみ) */
std::array<float, 3> Vec3::cross(const std::array<float, 3> &v1, const std::array<float, 3> &v2) {
    std::array<float, 3> retvec = {};
    retvec[0] = v1[1] * v2[2] - v1[2] * v2[1];
    retvec[1] = v1[2] * v2[0] - v1[0] * v2[2];
    retvec[2] = v1[0] * v2[1] - v1[1] * v2[0];
    return retvec;
}
