//
//  rgb.cpp
//  YUV-Accelerator
//
//  Created by 朱芄蓉 on 17/12/2017.
//  Copyright © 2017 朱芄蓉. All rights reserved.
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
    cout << "[" << name << "]: Alpha blending without SIMD." << endl;
    
    for(int i = 0, pos = 0; i < src.height; ++i){
        for(int j = 0; j < src.width; ++j, ++pos){
            r[pos] = uint8_t((uint16_t)src.r[pos] * alpha >> 8);
            g[pos] = uint8_t((uint16_t)src.g[pos] * alpha >> 8);
            b[pos] = uint8_t((uint16_t)src.b[pos] * alpha >> 8);
        }
    }
}

/*
 Set this object to 'alpha * img1 + (256 - alpha) * img2'.
 
 * Parameters
     - img1, img2: two RGBImage objects
     - alpha: during superimposing, img1 has 'alpha' and img2 has '256 - alpha'.
 */
void RGBImage::superimpose_ori(RGBImage& img1, RGBImage &img2, int alpha){
    cout << "[" << name << "]: Superimposing without SIMD." << endl;
    
    RGBImage tmp_img1(img1.width, img1.height, "tmp_img1"), tmp_img2(img2.width, img2.height, "tmp_img2");
    tmp_img1.alpha_blend_ori(img1, alpha);
    tmp_img2.alpha_blend_ori(img2, 256 - alpha);
    for(int i = 0, pos = 0; i < height; ++i){
        for(int j = 0; j < width; ++j, ++pos){
            r[pos] = uint8_t((uint16_t)tmp_img1.r[pos] + (uint16_t)tmp_img2.r[pos]);
            g[pos] = uint8_t((uint16_t)tmp_img1.g[pos] + (uint16_t)tmp_img2.g[pos]);
            b[pos] = uint8_t((uint16_t)tmp_img1.b[pos] + (uint16_t)tmp_img2.b[pos]);
        }
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
