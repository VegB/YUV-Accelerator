//
//  rgb.cpp
//  YUV-Accelerator
//
//  Created by VegB on 17/12/2017.
//  Copyright © 2017 VegB. All rights reserved.
//

#include "rgb.hpp"
#include "bmp.h"

/* Set alpha and clear rgb */
void RGBImage::init_image(){
    memset(r, 0, width * height);
    memset(g, 0, width * height);
    memset(b, 0, width * height);
}

/*
 Set this object to src * alpha.
 
 * Parameters
     - src: a RGBImage object
     - alpha: used to set this object to src * alpha
 */
void RGBImage::alpha_blend_ori(RGBImage& src, int alpha){
#ifdef LOG
    cout << "[" << name << "]: Alpha blending without SIMD." << endl;
#endif
    for(int i = 0, pos = 0; i < src.height; ++i){
        for(int j = 0; j < src.width; ++j, ++pos){
//            r[pos] = uint8_t((uint16_t)src.r[pos] * alpha >> 8);
//            g[pos] = uint8_t((uint16_t)src.g[pos] * alpha >> 8);
//            b[pos] = uint8_t((uint16_t)src.b[pos] * alpha >> 8);
            r[pos] = uint8_t((double)src.r[pos] * alpha / 256.0001);
            g[pos] = uint8_t((double)src.g[pos] * alpha / 256.0001);
            b[pos] = uint8_t((double)src.b[pos] * alpha / 256.0001);
        }
    }
}

inline void RGBImage::single_color_blend_mmx(uint8_t*src, uint8_t* dst, int pos, __m64 alpha){
    __m64 color = _mm_set_pi16((int16_t)src[pos], (int16_t)src[pos + 1], (int16_t)src[pos + 2], (int16_t)src[pos + 3]);
    __m64 rst = _m_pmullw(color, alpha);  // r/g/b * alpha
    __m64 rst2 = _mm_srli_pi16(rst, 8);  // r/g/b / 256
    write_back_mmx(rst2, dst, pos);
#ifdef DEBUG
//    cout << "alpha: ";
//    print_64(alpha);
//    cout << "color: ";
//    print_64(color);
//    cout << "alpha * color: ";
//    print_64(rst);
//    cout << "div 256: ";
//    print_64(rst2);
#endif
}

void RGBImage::alpha_blend_mmx(RGBImage& src, int alpha_){
    __m64 alpha = _mm_set_pi16((int16_t)alpha_, (int16_t)alpha_, (int16_t)alpha_, (int16_t)alpha_);
    for(int i = 0; i < src.width * src.height; i += 4){
        single_color_blend_mmx(src.r, r, i, alpha);
        single_color_blend_mmx(src.g, g, i, alpha);
        single_color_blend_mmx(src.b, b, i, alpha);
    }
}

inline void RGBImage::single_color_blend_sse2(uint8_t*src, uint8_t* dst, int pos, __m128 alpha){
    __m128 color = _mm_set_ps((float)src[pos + 3], (float)src[pos + 2], (float)src[pos + 1], (float)src[pos]);
    __m128 CONST_256 = _mm_set_ps((float)256, (float)256, (float)256, (float)256);
    __m128 rst = _mm_mul_ps(color, alpha);  // r/g/b * alpha
    __m128 rst2 = _mm_div_ps(rst, CONST_256);  // r/g/b / 256
    write_back_sse2(rst2, dst, pos);
}

void RGBImage::alpha_blend_sse2(RGBImage& src, int alpha_){
    __m128 alpha = _mm_set_ps((float)alpha_, (float)alpha_, (float)alpha_, (float)alpha_);
    for(int i = 0; i < src.width * src.height; i += 4){
        single_color_blend_sse2(src.r, r, i, alpha);
        single_color_blend_sse2(src.g, g, i, alpha);
        single_color_blend_sse2(src.b, b, i, alpha);
    }
}

inline void RGBImage::single_color_blend_avx(uint8_t*src, uint8_t* dst, int pos, __m256 alpha){
    __m256 color = _mm256_set_ps((float)src[pos], (float)src[pos + 1], (float)src[pos + 2], (float)src[pos + 3], (float)src[pos + 4], (float)src[pos + 5], (float)src[pos + 6], (float)src[pos + 7]);
    __m256 CONST_256 = _mm256_set_ps((float)256, (float)256, (float)256, (float)256, (float)256, (float)256, (float)256, (float)256);
    __m256 rst = _mm256_mul_ps(color, alpha);  // r/g/b * alpha
    __m256 rst2 = _mm256_div_ps(rst, CONST_256);  // r/g/b / 256
    write_back_avx(rst2, dst, pos);
}

void RGBImage::alpha_blend_avx(RGBImage& src, int alpha_){
    __m256 alpha = _mm256_set_ps((float)alpha_, (float)alpha_, (float)alpha_, (float)alpha_, (float)alpha_, (float)alpha_, (float)alpha_, (float)alpha_);
    for(int i = 0; i < src.width * src.height; i += 8){
        single_color_blend_avx(src.r, r, i, alpha);
        single_color_blend_avx(src.g, g, i, alpha);
        single_color_blend_avx(src.b, b, i, alpha);
    }
}

/*
 Set this object to 'alpha * img1 + (256 - alpha) * img2'.
 
 * Parameters
     - img1, img2: two RGBImage objects
     - alpha: during superimposing, img1 has 'alpha' and img2 has '256 - alpha'.
 */
void RGBImage::superimpose_ori(RGBImage& img1, RGBImage &img2, int alpha){
#ifdef LOG
    cout << "[" << name << "]: Superimposing without SIMD." << endl;
#endif
    RGBImage tmp_img1(img1.width, img1.height, "tmp_img1"), tmp_img2(img2.width, img2.height, "tmp_img2");
    tmp_img1.alpha_blend_ori(img1, alpha);
    tmp_img2.alpha_blend_ori(img2, 256 - alpha);
    for(int pos = 0; pos < img1.width * img1.height; ++pos){
        r[pos] = uint8_t((double)tmp_img1.r[pos] + (double)tmp_img2.r[pos]);
        g[pos] = uint8_t((double)tmp_img1.g[pos] + (double)tmp_img2.g[pos]);
        b[pos] = uint8_t((double)tmp_img1.b[pos] + (double)tmp_img2.b[pos]);
    }
}

inline void RGBImage::single_color_impose_mmx(uint8_t* src1, uint8_t* src2, uint8_t* dst, int pos){
    __m64 num1 = _mm_set_pi16((int16_t)src1[pos], (int16_t)src1[pos + 1], (int16_t)src1[pos + 2],(int16_t)src1[pos + 3]);
    __m64 num2 = _mm_set_pi16((int16_t)src2[pos], (int16_t)src2[pos + 1], (int16_t)src2[pos + 2],(int16_t)src2[pos + 3]);
    __m64 rst = _m_paddw(num1, num2);
    write_back_mmx(rst, dst, pos);
}

void RGBImage::superimpose_mmx(RGBImage& img1, RGBImage& img2, int alpha){
    RGBImage tmp_img1(img1.width, img1.height, "tmp_img1"), tmp_img2(img2.width, img2.height, "tmp_img2");
    tmp_img1.alpha_blend_mmx(img1, alpha);
    tmp_img2.alpha_blend_mmx(img2, 256 - alpha);
    for(int pos = 0; pos < img1.width * img1.height; pos += 4){
        single_color_impose_mmx(tmp_img1.r, tmp_img2.r, r, pos);
        single_color_impose_mmx(tmp_img1.g, tmp_img2.g, g, pos);
        single_color_impose_mmx(tmp_img1.b, tmp_img2.b, b, pos);
    }
}

inline void RGBImage::single_color_impose_sse2(uint8_t* src1, uint8_t* src2, uint8_t* dst, int pos){
    __m128 num1 = _mm_set_ps((float)src1[pos], (float)src1[pos + 1], (float)src1[pos + 2], (float)src1[pos + 3]);
    __m128 num2 = _mm_set_ps((float)src2[pos], (float)src2[pos + 1], (float)src2[pos + 2], (float)src2[pos + 3]);
    __m128 rst = _mm_add_ps(num1, num2);
    write_back_sse2(rst, dst, pos);
}

void RGBImage::superimpose_sse2(RGBImage& img1, RGBImage& img2, int alpha){
    RGBImage tmp_img1(img1.width, img1.height, "tmp_img1"), tmp_img2(img2.width, img2.height, "tmp_img2");
    tmp_img1.alpha_blend_sse2(img1, alpha);
    tmp_img2.alpha_blend_sse2(img2, 256 - alpha);
    for(int pos = 0; pos < img1.width * img1.height; pos += 4){
        single_color_impose_sse2(tmp_img1.r, tmp_img2.r, r, pos);
        single_color_impose_sse2(tmp_img1.g, tmp_img2.g, g, pos);
        single_color_impose_sse2(tmp_img1.b, tmp_img2.b, b, pos);
    }
}

inline void RGBImage::single_color_impose_avx(uint8_t* src1, uint8_t* src2, uint8_t* dst, int pos){
    __m256 num1 = _mm256_set_ps((float)src1[pos], (float)src1[pos + 1], (float)src1[pos + 2], (float)src1[pos + 3], (float)src1[pos + 4], (float)src1[pos + 5], (float)src1[pos + 6], (float)src1[pos + 7]);
    __m256 num2 = _mm256_set_ps((float)src2[pos], (float)src2[pos + 1], (float)src2[pos + 2], (float)src2[pos + 3], (float)src2[pos + 4], (float)src2[pos + 5], (float)src2[pos + 6], (float)src2[pos + 7]);
    __m256 rst = _mm256_add_ps(num1, num2);
    write_back_avx(rst, dst, pos);
}

void RGBImage::superimpose_avx(RGBImage& img1, RGBImage& img2, int alpha){
    RGBImage tmp_img1(img1.width, img1.height, "tmp_img1"), tmp_img2(img2.width, img2.height, "tmp_img2");
    tmp_img1.alpha_blend_avx(img1, alpha);
    tmp_img2.alpha_blend_avx(img2, 256 - alpha);
    for(int pos = 0; pos < img1.width * img1.height; pos += 4){
        single_color_impose_avx(tmp_img1.r, tmp_img2.r, r, pos);
        single_color_impose_avx(tmp_img1.g, tmp_img2.g, g, pos);
        single_color_impose_avx(tmp_img1.b, tmp_img2.b, b, pos);
    }
}

void RGBImage::write_bmp(FILE* fout) const {
    t_BMPfileheader file_header;
    t_BMPinfoheader info_header;
    uint8_t buf[3];
    int i, j;
    fwrite(&file_header, sizeof(t_BMPfileheader), 1, fout);
    fwrite(&info_header, sizeof(t_BMPinfoheader), 1, fout);
    for (i = height - 1; i >= 0; i--) {
        for (j = 0; j<width; j++) {
            buf[0] = r[i*width + j];
            buf[1] = g[i*width + j];
            buf[2] = b[i*width + j];
            fwrite(buf, sizeof(uint8_t), 3, fout);
        }
    }
}
