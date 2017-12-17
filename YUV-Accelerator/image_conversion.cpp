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
            int uv_pos = (i / 2) * rgb.width / 2 + (j / 2);  // index of u, v
            /* red */
            double tmp = (double)yuv.y[pos]
                        + 1.140 * (double)yuv.v[uv_pos];
            rgb.r[pos] = (uint8_t)clip(0, 255, tmp);
            /* green */
            tmp = (double)yuv.y[pos]
                - 0.394 * (double)yuv.u[uv_pos]
                - 0.581 * (double)yuv.v[uv_pos];
            rgb.g[pos] = (uint8_t)clip(0, 255, tmp);
            /* blue */
            tmp = (double)yuv.y[pos]
                + 2.032 * (double)yuv.u[uv_pos];
            rgb.b[pos] = (uint8_t)clip(0, 255, tmp);
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
    
    /* Y */
    for(int i = 0, pos = 0; i < rgb.height; ++i){
        for(int j = 0; j < rgb.width; ++j, ++pos){
            double tmp = 0.299 * rgb.r[pos]
                        + 0.587 * rgb.g[pos]
                        + 0.144 * rgb.b[pos];
            yuv.y[pos] = (uint8_t)clip(0, 255, tmp);
        }
    }
    
    /* U, V */
    for(int i = 0, uv_pos = 0; i < rgb.height; i += 2){
        for(int j = 0; j < rgb.width; j += 2, ++uv_pos){
            int rgb_pos = i * rgb.height + j;
            /* U */
            double tmp = -0.147 * rgb.r[rgb_pos]
                        - 0.289 * rgb.g[rgb_pos]
                        + 0.436 * rgb.b[rgb_pos];
            yuv.u[uv_pos] = (uint8_t)clip(0, 255, tmp);
            /* V */
            tmp = 0.615 * rgb.r[rgb_pos]
                - 0.515 * rgb.g[rgb_pos]
                - 0.100 * rgb.b[rgb_pos];
            yuv.v[uv_pos] = (uint8_t)clip(0, 255, tmp);
        }
    }
}
