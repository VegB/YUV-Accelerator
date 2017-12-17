//
//  image_conversion.cpp
//  YUV-Accelerator
//
//  Created by 朱芄蓉 on 17/12/2017.
//  Copyright © 2017 朱芄蓉. All rights reserved.
//

#include "image_conversion.hpp"

/* clip num to [lower_bound, upper_bound] */
double clip(int lower_bound, int upper_bound, double num){
    if(num > upper_bound){
        num = upper_bound;
    }
    if(num < lower_bound){
        num = lower_bound;
    }
    return num;
}

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
