//
//  image_conversion.cpp
//  YUV-Accelerator
//
//  Created by 朱芄蓉 on 17/12/2017.
//  Copyright © 2017 朱芄蓉. All rights reserved.
//

#include "image_conversion.hpp"

/* No optimazation */
void yuv2rgb_ori(YUVImage& yuv, RGBImage& rgb){
    if(yuv.width != rgb.width || yuv.height != rgb.height){
        cerr << "[Image Conversion]: Size of YUV image and RGB image does not match!" << endl;
        return;
    }
    rgb.init_image();
    
    for(int i = 0, pos = 0; i < rgb.height; ++i){
        for(int j = 0; j < rgb.width; ++j, ++pos){  // k = i * width + j
            int uv_pos = (i / 2) * rgb.width / 2 + (j / 2);  // index of u,v
            double r, g, b;
#ifdef THIS_YEAR
            r = (double)yuv.y[pos]
                + 1.140 * (double)yuv.v[uv_pos];
            g = (double)yuv.y[pos]
                - 0.394 * (double)yuv.u[uv_pos]
                - 0.581 * (double)yuv.v[uv_pos];
            b = (double)yuv.y[pos]
                + 2.032 * (double)yuv.u[uv_pos];
#endif
#ifndef THIS_YEAR
            r = 1.164383 * ((double)yuv.y[pos] - 16)
                + 1.596027 * ((double)yuv.v[uv_pos] - 128);
            g = 1.164383 * ((double)yuv.y[pos] - 16)
                - 0.391762 * ((double)yuv.u[uv_pos] - 128)
                - 0.812968 * ((double)yuv.v[uv_pos] - 128);
            b = 1.164383 * ((double)yuv.y[pos] - 16)
                + 2.017232 * ((double)yuv.u[uv_pos] - 128);
#endif
            rgb.r[pos] = (uint8_t)clip(0, 255, r);
            rgb.g[pos] = (uint8_t)clip(0, 255, g);
            rgb.b[pos] = (uint8_t)clip(0, 255, b);
        }
    }
}

/* MMX */
void yuv2rgb_mmx(YUVImage& yuv, RGBImage& rgb){
    cout << "yuv2rgb_mmx()" << endl;
    
    int8_t YUV_R[3] = {(int8_t)(0.164383 * (1 << 16)), (int8_t)(0.017232 * (1 << 16)), 0 * (1 << 16)};
    int8_t YUV_G[3] = {(int8_t)(0.164383 * (1 << 16)), (int8_t)(-0.391762 * (1 << 16)), (int8_t)(-0.312968 * (1 << 16))};
    int8_t YUV_B[3] = {(int8_t)(0.164383 * (1 << 16)),  0 * (1 << 16), (int8_t)(0.096027 * (1 << 16))};
    __m64 OFFSET_128 = _mm_set_pi8(128, 128, 128, 128, 128, 128, 128, 128);
    __m64 OFFSET_16 = _mm_set_pi8(16, 16, 16, 16, 16, 16, 16, 16);
    
    uint8_t tmp_u[yuv.width * yuv.height];
    uint8_t tmp_v[yuv.width * yuv.height];
    yuv.get_uv(tmp_u, tmp_v);
    
    __m64 tmp, tmp_data;
    
    _mm_empty();
    
    // YUV to R Channel
    __m64* dst = (__m64*)rgb.r;
    __m64* src_u = (__m64*)tmp_u;
    __m64* src_v = (__m64*)tmp_v;
    
    __m64 Y_R = _mm_set_pi8(YUV_R[0], YUV_R[0], YUV_R[0], YUV_R[0], YUV_R[0], YUV_R[0], YUV_R[0], YUV_R[0]);
    __m64 U_R = _mm_set_pi8(YUV_R[1], YUV_R[1], YUV_R[1], YUV_R[1], YUV_R[1], YUV_R[1], YUV_R[1], YUV_R[1]);
    __m64 V_R = _mm_set_pi8(YUV_R[2], YUV_R[2], YUV_R[2], YUV_R[2], YUV_R[2], YUV_R[2], YUV_R[2], YUV_R[2]);
    for (int i = 0; i < (rgb.width * rgb.height); i += 8) {
        // Y Channel to R Channel
        __m64 y = _mm_set_pi8(yuv.y[i], yuv.y[i + 1], yuv.y[i + 2], yuv.y[i + 3], yuv.y[i + 4], yuv.y[i + 5], yuv.y[i + 6], yuv.y[i + 7]);
        tmp_data = _m_psubw(y, OFFSET_16); // (Y - 16)
        tmp = _m_pmullw(tmp_data, Y_R);            // R = (Y - 16) * 0.164383
        *dst = _m_paddsw(tmp, tmp_data);        // R += Y - 16
        
        // U Channel to R Channel
        tmp_data = _m_psubw(*src_u, OFFSET_128);// (U - 128)
        tmp = _m_pmullw(tmp_data, U_R);            // (U - 128) * 0.017232
        *dst = _m_paddsw(*dst, tmp);            // R += (U - 128) * 0.017232
        tmp = _m_psllwi(tmp_data, 1);
        *dst = _m_paddsw(*dst, tmp);            // R += (U - 128) << 1;
        
        // V Channel to R Channel
        
        
        // increase iterators
        dst++;
        src_u++;
        src_v++;
    }
    // End of YUV to R Channel
    
    // YUV to G Channel
    dst = (__m64*)rgb.g;
    src_u = (__m64*)tmp_u;
    src_v = (__m64*)tmp_v;
    __m64 Y_G = _mm_set_pi8(YUV_G[0], YUV_G[0], YUV_G[0], YUV_G[0], YUV_G[0], YUV_G[0], YUV_G[0], YUV_G[0]);
    __m64 U_G = _mm_set_pi8(YUV_G[1], YUV_G[1], YUV_G[1], YUV_G[1], YUV_G[1], YUV_G[1], YUV_G[1], YUV_G[1]);
    __m64 V_G = _mm_set_pi8(YUV_G[2], YUV_G[2], YUV_G[2], YUV_G[2], YUV_G[2], YUV_G[2], YUV_G[2], YUV_G[2]);
    for (int i = 0; i < (rgb.width * rgb.height); i += 8) {
        // Y Channel to G Channel
        __m64 y = _mm_set_pi8(yuv.y[i], yuv.y[i + 1], yuv.y[i + 2], yuv.y[i + 3], yuv.y[i + 4], yuv.y[i + 5], yuv.y[i + 6], yuv.y[i + 7]);
        tmp_data = _m_psubw(y, OFFSET_16); // (Y - 16)
        tmp = _m_pmulhw(tmp_data, Y_G);            // G = (Y - 16) * 0.164383
        *dst = _m_paddsw(tmp, tmp_data);        // G += Y - 16
        
        // U Channel to G Channel
        tmp_data = _m_psubw(*src_u, OFFSET_128);// (U - 128)
        tmp = _m_pmulhw(tmp_data, U_G);            // (U - 128) * (-0.391762)
        *dst = _m_paddsw(*dst, tmp);            // G += (U - 128) * (-0.391762)
        
        // V Channel to R Channel
        tmp_data = _m_psubw(*src_v, OFFSET_128);// (V - 128)
        tmp = _m_pmulhw(tmp_data, V_G);            // (V - 128) * (-0.312968)
        *dst = _m_paddsw(*dst, tmp);            // G += (V - 128) * (-0.312968)
        tmp = _m_psrawi(tmp_data, 1);
        *dst = _m_psubsw(*dst, tmp);            // G -= (V - 128) >> 1;
        
        // increase iterators
        dst++;
        src_u++;
        src_v++;
    }
    // End of YUV to G Channel
    
    // YUV to B Channel
    dst = (__m64*)rgb.b;
    src_u = (__m64*)tmp_u;
    src_v = (__m64*)tmp_v;
    __m64 Y_B = _mm_set_pi8(YUV_B[0], YUV_B[0], YUV_B[0], YUV_B[0], YUV_B[0], YUV_B[0], YUV_B[0], YUV_B[0]);
    __m64 U_B = _mm_set_pi8(YUV_B[1], YUV_B[1], YUV_B[1], YUV_B[1], YUV_B[1], YUV_B[1], YUV_B[1], YUV_B[1]);
    __m64 V_B = _mm_set_pi8(YUV_B[2], YUV_B[2], YUV_B[2], YUV_B[2], YUV_B[2], YUV_B[2], YUV_B[2], YUV_B[2]);
    for (int i = 0; i < (rgb.width * rgb.height) >> 2; i++) {
        // Y Channel to B Channel
        __m64 y = _mm_set_pi8(yuv.y[i], yuv.y[i + 1], yuv.y[i + 2], yuv.y[i + 3], yuv.y[i + 4], yuv.y[i + 5], yuv.y[i + 6], yuv.y[i + 7]);
        tmp_data = _m_psubw(y, OFFSET_16); // (Y - 16)
        tmp = _m_pmulhw(tmp_data, Y_B);            // B = (Y - 16) * 0.164383
        *dst = _m_paddsw(tmp, tmp_data);        // B += Y - 16
        
        // U Channel to B Channel
        
        
        // V Channel to B Channel
        tmp_data = _m_psubw(*src_v, OFFSET_128);// (V - 128)
        tmp = _m_pmulhw(tmp_data, V_B);            // (V - 128) * 0.096027
        *dst = _m_paddsw(*dst, tmp);            // B += (V - 128) * 0.096027
        tmp = _m_psrawi(tmp_data, 1);
        *dst = _m_paddsw(*dst, tmp);            // B += (V - 128) >> 1;
        tmp = _m_psllwi(tmp_data, 1);
        *dst = _m_paddsw(*dst, tmp);            // B += (V - 128) << 1;
        
        // increase iterators
        dst++;
        src_u++;
        src_v++;
    }
    // End of YUV to B Channel
    
    _mm_empty();
}

/* SSE2 */
void yuv2rgb_sse2(YUVImage& yuv, RGBImage& rgb){
    
}

/* AVX */
void yuv2rgb_avx(YUVImage& yuv, RGBImage& rgb){
    
}

/* No optimazation */
void rgb2yuv_ori(RGBImage& rgb, YUVImage& yuv){
    if(yuv.width != rgb.width || yuv.height != rgb.height){
        cerr << "[Image Conversion]: Size of YUV image and RGB image does not match!" << endl;
        return;
    }
    yuv.init_image();
    
    double y, u, v;
    /* Y */
    for(int i = 0, pos = 0; i < rgb.height; ++i){
        for(int j = 0; j < rgb.width; ++j, ++pos){
#ifdef THIS_YEAR
            y = 0.299 * rgb.r[pos]
                + 0.587 * rgb.g[pos]
                + 0.144 * rgb.b[pos];
#endif
#ifndef THIS_YEAR
            y = 0.256788 * rgb.r[pos]
                + 0.504129 * rgb.g[pos]
                + 0.097906 * rgb.b[pos] + 16;
#endif
            yuv.y[pos] = (uint8_t)clip(0, 255, y);
        }
    }
    
    /* U, V */
    for(int i = 0, uv_pos = 0; i < rgb.height; i += 2){
        for(int j = 0; j < rgb.width; j += 2, ++uv_pos){
            int rgb_pos = i * rgb.width + j;
            double u, v;
#ifdef THIS_YEAR
            u = -0.147 * rgb.r[rgb_pos]
                - 0.289 * rgb.g[rgb_pos]
                + 0.436 * rgb.b[rgb_pos];
            v = 0.615 * rgb.r[rgb_pos]
                - 0.515 * rgb.g[rgb_pos]
                - 0.100 * rgb.b[rgb_pos];
#endif
#ifndef THIS_YEAR
            u = -0.148223*rgb.r[rgb_pos] - 0.290993*rgb.g[rgb_pos] + 0.439216*rgb.b[rgb_pos] + 128;
            v = 0.439216*rgb.r[rgb_pos] - 0.367788*rgb.g[rgb_pos] - 0.071427*rgb.b[rgb_pos] + 128;
#endif
            yuv.u[uv_pos] = (uint8_t)clip(0, 255, u);
            yuv.v[uv_pos] = (uint8_t)clip(0, 255, v);
        }
    }
}

/* MMX */
void rgb2yuv_mmx(YUVImage& yuv, RGBImage& rgb){
    
}

/* SSE2 */
void rgb2yuv_sse2(YUVImage& yuv, RGBImage& rgb){
    
}

/* AVX */
void rgb2yuv_avx(YUVImage& yuv, RGBImage& rgb){
    
}
