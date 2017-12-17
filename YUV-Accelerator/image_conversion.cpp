//
//  image_conversion.cpp
//  YUV-Accelerator
//
//  Created by 朱芄蓉 on 17/12/2017.
//  Copyright © 2017 朱芄蓉. All rights reserved.
//

#include "image_conversion.hpp"

float yuv2r[3] = {1.164383, 0, 1.596027};
float yuv2g[3] = {1.164383, -0.391762, -0.812968};
float yuv2b[3] = {1.164383, 2.017232, 0};

float rgb2y[3] = {0.256788, 0.504129, 0.097906};
float rgb2u[3] = {-0.148223, -0.290993, 0.439216};
float rgb2v[3] = {0.439216, -0.367788, -0.071427};

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
    _mm_empty();
    uint8_t tmp_u[yuv.width * yuv.height];
    uint8_t tmp_v[yuv.width * yuv.height];
    yuv.get_uv(tmp_u, tmp_v);

    __m64 MINUS_16 = _mm_set_pi16((int16_t)(-16), (int16_t)(-16), (int16_t)(-16), (int16_t)(-16));
    __m64 MINUS_128 = _mm_set_pi16((int16_t)(-128), (int16_t)(-128), (int16_t)(-128), (int16_t)(-128));
    
    /* YUV2R */
    for(int i = 0; i < yuv.width * yuv.height; i += 4){
        __m64 y = _mm_set_pi16((int16_t)yuv.y[i], (int16_t)yuv.y[i + 1], (int16_t)yuv.y[i + 2], (int16_t)yuv.y[i + 3]);
        __m64 y_minus_16 = _mm_add_pi16(y, MINUS_16);  // y - 16
        __m64 y_param = _mm_set_pi16((int16_t)(yuv2r[0] * 256), (int16_t)(yuv2r[0] * 256), (int16_t)(yuv2r[0] * 256), (int16_t)(yuv2r[0] * 256));
        __m64 y_rst = _m_pmulhw(y_param, y_minus_16);
        
        __m64 v = _mm_set_pi16((int16_t)tmp_v[i], (int16_t)tmp_v[i + 1], (int16_t)tmp_v[i + 2], (int16_t)tmp_v[i + 3]);
        __m64 v_minus_128 = _mm_add_pi16(v, MINUS_128);
        __m64 v_param = _mm_set_pi16((int16_t)(yuv2r[2] * 256), (int16_t)(yuv2r[2] * 256), (int16_t)(yuv2r[2] * 256), (int16_t)(yuv2r[2] * 256));
        __m64 v_rst = _m_pmulhw(v_param, v_minus_128);
        
        __m64 rst = _mm_add_pi16(y_rst, v_rst);
        write_back_from_64(rst, rgb.r, i);
    }
    
    /* YUV2G */
    for(int i = 0; i < yuv.width * yuv.height; i += 4){
        __m64 y = _mm_set_pi16((int16_t)yuv.y[i], (int16_t)yuv.y[i + 1], (int16_t)yuv.y[i + 2], (int16_t)yuv.y[i + 3]);
        __m64 y_minus_16 = _mm_add_pi16(y, MINUS_16);  // y - 16
        __m64 y_param = _mm_set_pi16((int16_t)(yuv2g[0] * 256), (int16_t)(yuv2g[0] * 256), (int16_t)(yuv2g[0] * 256), (int16_t)(yuv2g[0] * 256));
        __m64 y_rst = _m_pmulhw(y_param, y_minus_16);
        
        __m64 u = _mm_set_pi16((int16_t)tmp_u[i], (int16_t)tmp_u[i + 1], (int16_t)tmp_u[i + 2], (int16_t)tmp_u[i + 3]);
        __m64 u_minus_128 = _mm_add_pi16(u, MINUS_128);
        __m64 u_param = _mm_set_pi16((int16_t)(yuv2g[1] * 256), (int16_t)(yuv2g[1] * 256), (int16_t)(yuv2g[1] * 256), (int16_t)(yuv2g[1] * 256));
        __m64 u_rst = _m_pmulhw(u_param, u_minus_128);
        
        __m64 v = _mm_set_pi16((int16_t)tmp_v[i], (int16_t)tmp_v[i + 1], (int16_t)tmp_v[i + 2], (int16_t)tmp_v[i + 3]);
        __m64 v_minus_128 = _mm_add_pi16(v, MINUS_128);
        __m64 v_param = _mm_set_pi16((int16_t)(yuv2g[2] * 256), (int16_t)(yuv2g[2] * 256), (int16_t)(yuv2g[2] * 256), (int16_t)(yuv2g[2] * 256));
        __m64 v_rst = _m_pmulhw(v_param, v_minus_128);
        
        __m64 rst1 = _mm_add_pi16(y_rst, u_rst);
        __m64 rst = _mm_add_pi16(rst1, v_rst);
        write_back_from_64(rst, rgb.g, i);
    }
    
    /* YUV2B */
    for(int i = 0; i < yuv.width * yuv.height; i += 4){
        __m64 y = _mm_set_pi16((int16_t)yuv.y[i], (int16_t)yuv.y[i + 1], (int16_t)yuv.y[i + 2], (int16_t)yuv.y[i + 3]);
        __m64 y_minus_16 = _mm_add_pi16(y, MINUS_16);  // y - 16
        __m64 y_param = _mm_set_pi16((int16_t)(yuv2b[0] * 256), (int16_t)(yuv2b[0] * 256), (int16_t)(yuv2b[0] * 256), (int16_t)(yuv2b[0] * 256));
        __m64 y_rst = _m_pmulhw(y_param, y_minus_16);
        
        __m64 u = _mm_set_pi16((int16_t)tmp_u[i], (int16_t)tmp_u[i + 1], (int16_t)tmp_u[i + 2], (int16_t)tmp_u[i + 3]);
        __m64 u_minus_128 = _mm_add_pi16(u, MINUS_128);
        __m64 u_param = _mm_set_pi16((int16_t)(yuv2b[1] * 256), (int16_t)(yuv2b[1] * 256), (int16_t)(yuv2b[1] * 256), (int16_t)(yuv2b[1] * 256));
        __m64 u_rst = _m_pmulhw(u_param, u_minus_128);
        
        __m64 rst = _mm_add_pi16(y_rst, u_rst);
        write_back_from_64(rst, rgb.b, i);
    }
    
    _mm_empty();
}

/* SSE2 */
void yuv2rgb_sse2(YUVImage& yuv, RGBImage& rgb){
    _mm_empty();
    
    uint8_t tmp_u[yuv.width * yuv.height];
    uint8_t tmp_v[yuv.width * yuv.height];
    yuv.get_uv(tmp_u, tmp_v);
    
    __m128 MINUS_16 = _mm_set_ps((float)(-16), (float)(-16), (float)(-16), (float)(-16));
    __m128 MINUS_128 = _mm_set_ps((float)(-128), (float)(-128), (float)(-128), (float)(-128));
    
    /* YUV2R */
    for(int i = 0; i < yuv.width * yuv.height; i += 4){
        __m128 y = _mm_set_ps((float)yuv.y[i], (float)yuv.y[i + 1], (float)yuv.y[i + 2], (float)yuv.y[i + 3]);
        __m128 y_minus_16 = _mm_add_ps(y, MINUS_16);  // y - 16
        __m128 y_param = _mm_set_ps((float)yuv2r[0], (float)yuv2r[0], (float)yuv2r[0], (float)yuv2r[0]);
        __m128 y_rst = _mm_mul_ps(y_param, y_minus_16);
        
        __m128 v = _mm_set_ps((float)tmp_v[i], (float)tmp_v[i + 1], (float)tmp_v[i + 2], (float)tmp_v[i + 3]);
        __m128 v_minus_128 = _mm_add_ps(v, MINUS_128);
        __m128 v_param = _mm_set_ps((float)yuv2r[2], (float)yuv2r[2], (float)yuv2r[2], (float)yuv2r[2]);
        __m128 v_rst = _mm_mul_ps(v_param, v_minus_128);
        
        __m128 rst = _mm_add_ps(y_rst, v_rst);
        write_back_from_128(rst, rgb.r, i);
    }
    
    /* YUV2G */
    for(int i = 0; i < yuv.width * yuv.height; i += 4){
        __m128 y = _mm_set_ps((float)yuv.y[i], (float)yuv.y[i + 1], (float)yuv.y[i + 2], (float)yuv.y[i + 3]);
        __m128 y_minus_16 = _mm_add_ps(y, MINUS_16);  // y - 16
        __m128 y_param = _mm_set_ps((float)yuv2g[0], (float)yuv2g[0], (float)yuv2g[0], (float)yuv2g[0]);
        __m128 y_rst = _mm_mul_ps(y_param, y_minus_16);
        
        __m128 u = _mm_set_ps((float)tmp_u[i], (float)tmp_u[i + 1], (float)tmp_u[i + 2], (float)tmp_u[i + 3]);
        __m128 u_minus_128 = _mm_add_ps(u, MINUS_128);
        __m128 u_param = _mm_set_ps((float)yuv2g[1], (float)yuv2g[1], (float)yuv2g[1], (float)yuv2g[1]);
        __m128 u_rst = _mm_mul_ps(u_param, u_minus_128);
        
        __m128 v = _mm_set_ps((float)tmp_v[i], (float)tmp_v[i + 1], (float)tmp_v[i + 2], (float)tmp_v[i + 3]);
        __m128 v_minus_128 = _mm_add_ps(v, MINUS_128);
        __m128 v_param = _mm_set_ps((float)yuv2g[2], (float)yuv2g[2], (float)yuv2g[2], (float)yuv2g[2]);
        __m128 v_rst = _mm_mul_ps(v_param, v_minus_128);
        
        __m128 rst1 = _mm_add_ps(y_rst, u_rst);
        __m128 rst = _mm_add_ps(rst1, v_rst);
        write_back_from_128(rst, rgb.g, i);
    }
    
    /* YUV2B */
    for(int i = 0; i < yuv.width * yuv.height; i += 4){
        __m128 y = _mm_set_ps((float)yuv.y[i], (float)yuv.y[i + 1], (float)yuv.y[i + 2], (float)yuv.y[i + 3]);
        __m128 y_minus_16 = _mm_add_ps(y, MINUS_16);  // y - 16
        __m128 y_param = _mm_set_ps((float)yuv2b[0], (float)yuv2b[0], (float)yuv2b[0], (float)yuv2b[0]);
        __m128 y_rst = _mm_mul_ps(y_param, y_minus_16);
        
        __m128 u = _mm_set_ps((float)tmp_u[i], (float)tmp_u[i + 1], (float)tmp_u[i + 2], (float)tmp_u[i + 3]);
        __m128 u_minus_128 = _mm_add_ps(u, MINUS_128);
        __m128 u_param = _mm_set_ps((float)yuv2b[1], (float)yuv2b[1], (float)yuv2b[1], (float)yuv2b[1]);
        __m128 u_rst = _mm_mul_ps(u_param, u_minus_128);
        
        __m128 rst = _mm_add_ps(y_rst, u_rst);
        write_back_from_128(rst, rgb.b, i);
    }
    
    _mm_empty();
}

/* AVX */
void yuv2rgb_avx(YUVImage& yuv, RGBImage& rgb){
    _mm_empty();
    _mm_empty();
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
void rgb2yuv_mmx(RGBImage& rgb, YUVImage& yuv){
    _mm_empty();
    _mm_empty();
}

/* SSE2 */
void rgb2yuv_sse2(RGBImage& rgb, YUVImage& yuv){
    _mm_empty();
    
    uint8_t tmp_u[yuv.width * yuv.height];
    uint8_t tmp_v[yuv.width * yuv.height];
    
    __m128 CONST_16 = _mm_set_ps((float)16, (float)16, (float)16, (float)16);
    
    /* RGB2Y */
    for(int i = 0; i < rgb.width * rgb.height; i += 4){
        __m128 r = _mm_set_ps((float)rgb.r[i], (float)rgb.r[i + 1], (float)rgb.r[i + 2], (float)rgb.r[i + 3]);
        __m128 r_param = _mm_set_ps((float)rgb2y[0], (float)rgb2y[0], (float)rgb2y[0], (float)rgb2y[0]);
        __m128 r_rst = _mm_mul_ps(r_param, r);
        
        __m128 g = _mm_set_ps((float)rgb.g[i], (float)rgb.g[i + 1], (float)rgb.g[i + 2], (float)rgb.g[i + 3]);
        __m128 g_param = _mm_set_ps((float)rgb2y[1], (float)rgb2y[1], (float)rgb2y[1], (float)rgb2y[1]);
        __m128 g_rst = _mm_mul_ps(g_param, g);
        
        __m128 b = _mm_set_ps((float)rgb.b[i], (float)rgb.b[i + 1], (float)rgb.b[i + 2], (float)rgb.b[i + 3]);
        __m128 b_param = _mm_set_ps((float)rgb2y[2], (float)rgb2y[2], (float)rgb2y[2], (float)rgb2y[2]);
        __m128 b_rst = _mm_mul_ps(b_param, b);
        
        __m128 rst1 = _mm_add_ps(r_rst, g_rst);
        __m128 rst2 = _mm_add_ps(b_rst, rst1);
        __m128 rst = _mm_add_ps(rst2, CONST_16);
        write_back_from_128(rst, yuv.y, i);
    }
    
    /* RGB2U */
    for(int i = 0, uv_pos = 0; i < rgb.height; i += 2){
        for(int j = 0; j < rgb.width; j += 8, uv_pos += 4){
            int rgb_pos = i * rgb.width + j;
            __m128 r = _mm_set_ps((float)rgb.r[rgb_pos], (float)rgb.r[rgb_pos + 2], (float)rgb.r[rgb_pos + 4], (float)rgb.r[rgb_pos + 6]);
            __m128 r_param = _mm_set_ps((float)rgb2u[0], (float)rgb2u[0], (float)rgb2u[0], (float)rgb2u[0]);
            __m128 r_rst = _mm_mul_ps(r_param, r);
        
            __m128 g = _mm_set_ps((float)rgb.g[rgb_pos], (float)rgb.g[rgb_pos + 2], (float)rgb.g[rgb_pos + 4], (float)rgb.g[rgb_pos + 6]);
            __m128 g_param = _mm_set_ps((float)rgb2u[1], (float)rgb2u[1], (float)rgb2u[1], (float)rgb2u[1]);
            __m128 g_rst = _mm_mul_ps(g_param, g);
            
            __m128 b = _mm_set_ps((float)rgb.b[rgb_pos], (float)rgb.b[rgb_pos + 2], (float)rgb.b[rgb_pos + 4], (float)rgb.b[rgb_pos + 6]);
            __m128 b_param = _mm_set_ps((float)rgb2u[2], (float)rgb2u[2], (float)rgb2u[2], (float)rgb2u[2]);
            __m128 b_rst = _mm_mul_ps(b_param, b);
            
            __m128 rst1 = _mm_add_ps(r_rst, g_rst);
            __m128 rst = _mm_add_ps(b_rst, rst1);
            write_back_from_128(rst, yuv.u, uv_pos);
        }
    }

    /* RGB2V */
    for(int i = 0, uv_pos = 0; i < rgb.height; i += 2){
        for(int j = 0; j < rgb.width; j += 8, uv_pos += 4){
            int rgb_pos = i * rgb.width + j;
            __m128 r = _mm_set_ps((float)rgb.r[rgb_pos], (float)rgb.r[rgb_pos + 2], (float)rgb.r[rgb_pos + 4], (float)rgb.r[rgb_pos + 6]);
            __m128 r_param = _mm_set_ps((float)rgb2v[0], (float)rgb2v[0], (float)rgb2v[0], (float)rgb2v[0]);
            __m128 r_rst = _mm_mul_ps(r_param, r);
            
            __m128 g = _mm_set_ps((float)rgb.g[rgb_pos], (float)rgb.g[rgb_pos + 2], (float)rgb.g[rgb_pos + 4], (float)rgb.g[rgb_pos + 6]);
            __m128 g_param = _mm_set_ps((float)rgb2v[1], (float)rgb2v[1], (float)rgb2v[1], (float)rgb2v[1]);
            __m128 g_rst = _mm_mul_ps(g_param, g);
            
            __m128 b = _mm_set_ps((float)rgb.b[rgb_pos], (float)rgb.b[rgb_pos + 2], (float)rgb.b[rgb_pos + 4], (float)rgb.b[rgb_pos + 6]);
            __m128 b_param = _mm_set_ps((float)rgb2v[2], (float)rgb2v[2], (float)rgb2v[2], (float)rgb2v[2]);
            __m128 b_rst = _mm_mul_ps(b_param, b);
            
            __m128 rst1 = _mm_add_ps(r_rst, g_rst);
            __m128 rst = _mm_add_ps(b_rst, rst1);
            write_back_from_128(rst, yuv.v, uv_pos);
        }
    }
    _mm_empty();
}

/* AVX */
void rgb2yuv_avx(RGBImage& rgb, YUVImage& yuv){
    _mm_empty();
    
    uint8_t tmp_u[yuv.width * yuv.height];
    uint8_t tmp_v[yuv.width * yuv.height];
    
    __m256 CONST_16 = _mm256_set_ps((float)16, (float)16, (float)16, (float)16, (float)16, (float)16, (float)16, (float)16);
    
    /* RGB2Y */
    for(int i = 0; i < rgb.width * rgb.height; i += 8){
        __m256 r = _mm256_set_ps((float)rgb.r[i], (float)rgb.r[i + 1], (float)rgb.r[i + 2], (float)rgb.r[i + 3], (float)rgb.r[i + 4], (float)rgb.r[i + 5], (float)rgb.r[i + 6], (float)rgb.r[i + 7]);
        __m256 r_param = _mm256_set_ps((float)rgb2y[0], (float)rgb2y[0], (float)rgb2y[0], (float)rgb2y[0], (float)rgb2y[0], (float)rgb2y[0], (float)rgb2y[0], (float)rgb2y[0]);
        __m256 r_rst = _mm256_mul_ps(r_param, r);
        
        __m256 g = _mm256_set_ps((float)rgb.g[i], (float)rgb.g[i + 1], (float)rgb.g[i + 2], (float)rgb.g[i + 3], (float)rgb.g[i + 4], (float)rgb.g[i + 5], (float)rgb.g[i + 6], (float)rgb.g[i + 7]);
        __m256 g_param = _mm256_set_ps((float)rgb2y[1], (float)rgb2y[1], (float)rgb2y[1], (float)rgb2y[1], (float)rgb2y[1], (float)rgb2y[1], (float)rgb2y[1], (float)rgb2y[1]);
        __m256 g_rst = _mm256_mul_ps(g_param, g);
        
        __m256 b = _mm256_set_ps((float)rgb.b[i], (float)rgb.b[i + 1], (float)rgb.b[i + 2], (float)rgb.b[i + 3], (float)rgb.g[i + 4], (float)rgb.g[i + 5], (float)rgb.g[i + 6], (float)rgb.g[i + 7]);
        __m256 b_param = _mm256_set_ps((float)rgb2y[2], (float)rgb2y[2], (float)rgb2y[2], (float)rgb2y[2], (float)rgb2y[2], (float)rgb2y[2], (float)rgb2y[2], (float)rgb2y[2]);
        __m256 b_rst = _mm256_mul_ps(b_param, b);
        
        __m256 rst1 = _mm256_add_ps(r_rst, g_rst);
        __m256 rst2 = _mm256_add_ps(b_rst, rst1);
        __m256 rst = _mm256_add_ps(rst2, CONST_16);
        write_back_from_256(rst, yuv.y, i);
    }
    
    /* RGB2U */
    for(int i = 0, uv_pos = 0; i < rgb.height; i += 2){
        for(int j = 0; j < rgb.width; j += 16, uv_pos += 8){
            int rgb_pos = i * rgb.width + j;
            __m256 r = _mm256_set_ps((float)rgb.r[rgb_pos], (float)rgb.r[rgb_pos + 2], (float)rgb.r[rgb_pos + 4], (float)rgb.r[rgb_pos + 6], (float)rgb.r[rgb_pos + 8], (float)rgb.r[rgb_pos + 10], (float)rgb.r[rgb_pos + 12], (float)rgb.r[rgb_pos + 14]);
            __m256 r_param = _mm256_set_ps((float)rgb2u[0], (float)rgb2u[0], (float)rgb2u[0], (float)rgb2u[0], (float)rgb2u[0], (float)rgb2u[0], (float)rgb2u[0], (float)rgb2u[0]);
            __m256 r_rst = _mm256_mul_ps(r_param, r);
            
            __m256 g = _mm256_set_ps((float)rgb.g[rgb_pos], (float)rgb.g[rgb_pos + 2], (float)rgb.g[rgb_pos + 4], (float)rgb.g[rgb_pos + 6], (float)rgb.g[rgb_pos + 8], (float)rgb.g[rgb_pos + 10], (float)rgb.g[rgb_pos + 12], (float)rgb.g[rgb_pos + 14]);
            __m256 g_param = _mm256_set_ps((float)rgb2u[1], (float)rgb2u[1], (float)rgb2u[1], (float)rgb2u[1], (float)rgb2u[1], (float)rgb2u[1], (float)rgb2u[1], (float)rgb2u[1]);
            __m256 g_rst = _mm256_mul_ps(g_param, g);
            
            __m256 b = _mm256_set_ps((float)rgb.b[rgb_pos], (float)rgb.b[rgb_pos + 2], (float)rgb.b[rgb_pos + 4], (float)rgb.b[rgb_pos + 6], (float)rgb.b[rgb_pos + 8], (float)rgb.b[rgb_pos + 10], (float)rgb.b[rgb_pos + 12], (float)rgb.b[rgb_pos + 14]);
            __m256 b_param = _mm256_set_ps((float)rgb2u[2], (float)rgb2u[2], (float)rgb2u[2], (float)rgb2u[2], (float)rgb2u[2], (float)rgb2u[2], (float)rgb2u[2], (float)rgb2u[2]);
            __m256 b_rst = _mm256_mul_ps(b_param, b);
            
            __m256 rst1 = _mm256_add_ps(r_rst, g_rst);
            __m256 rst = _mm256_add_ps(b_rst, rst1);
            write_back_from_256(rst, yuv.u, uv_pos);
        }
    }
    
    /* RGB2V */
    for(int i = 0, uv_pos = 0; i < rgb.height; i += 2){
        for(int j = 0; j < rgb.width; j += 16, uv_pos += 8){
            int rgb_pos = i * rgb.width + j;
            __m256 r = _mm256_set_ps((float)rgb.r[rgb_pos], (float)rgb.r[rgb_pos + 2], (float)rgb.r[rgb_pos + 4], (float)rgb.r[rgb_pos + 6], (float)rgb.r[rgb_pos + 8], (float)rgb.r[rgb_pos + 10], (float)rgb.r[rgb_pos + 12], (float)rgb.r[rgb_pos + 14]);
            __m256 r_param = _mm256_set_ps((float)rgb2v[0], (float)rgb2v[0], (float)rgb2v[0], (float)rgb2v[0], (float)rgb2v[0], (float)rgb2v[0], (float)rgb2v[0], (float)rgb2v[0]);
            __m256 r_rst = _mm256_mul_ps(r_param, r);
            
            __m256 g = _mm256_set_ps((float)rgb.g[rgb_pos], (float)rgb.g[rgb_pos + 2], (float)rgb.g[rgb_pos + 4], (float)rgb.g[rgb_pos + 6], (float)rgb.g[rgb_pos + 8], (float)rgb.g[rgb_pos + 10], (float)rgb.g[rgb_pos + 12], (float)rgb.g[rgb_pos + 14]);
            __m256 g_param = _mm256_set_ps((float)rgb2v[1], (float)rgb2v[1], (float)rgb2v[1], (float)rgb2v[1], (float)rgb2v[1], (float)rgb2v[1], (float)rgb2v[1], (float)rgb2v[1]);
            __m256 g_rst = _mm256_mul_ps(g_param, g);
            
            __m256 b = _mm256_set_ps((float)rgb.b[rgb_pos], (float)rgb.b[rgb_pos + 2], (float)rgb.b[rgb_pos + 4], (float)rgb.b[rgb_pos + 6], (float)rgb.b[rgb_pos + 8], (float)rgb.b[rgb_pos + 10], (float)rgb.b[rgb_pos + 12], (float)rgb.b[rgb_pos + 14]);
            __m256 b_param = _mm256_set_ps((float)rgb2v[2], (float)rgb2v[2], (float)rgb2v[2], (float)rgb2v[2], (float)rgb2v[2], (float)rgb2v[2], (float)rgb2v[2], (float)rgb2v[2]);
            __m256 b_rst = _mm256_mul_ps(b_param, b);
            
            __m256 rst1 = _mm256_add_ps(r_rst, g_rst);
            __m256 rst = _mm256_add_ps(b_rst, rst1);
            write_back_from_256(rst, yuv.v, uv_pos);
        }
    }
    
    _mm_empty();
}
