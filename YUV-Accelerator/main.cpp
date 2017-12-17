//
//  main.cpp
//  YUV-Accelerator
//
//  Created by 朱芄蓉 on 17/12/2017.
//  Copyright © 2017 朱芄蓉. All rights reserved.
//

#include <iostream>
// #include <time.h>
#include "image_conversion.hpp"
using namespace std;

#define NO_SIMD
//#define MMX
//#define SSE2
//#define AVX

#define WIDTH 1920
#define HEIGHT 1080

int init_time = -7;

/* Filepath */
string YUV_filepath1 = "./input/dem1.yuv";
string YUV_filepath2 = "./input/dem2.yuv";
string alpha_blend_output_filepath = "./output/alpha_blend.yuv";
string superimposing_output_filepath = "./output/superimpose.yuv";

/* Image objects */
YUVImage yuv1(WIDTH, HEIGHT, "yuv1"), yuv2(WIDTH, HEIGHT, "yuv2");
RGBImage rgb1(WIDTH, HEIGHT, "rgb1"), rgb2(WIDTH, HEIGHT, "rgb2");

clock_t process_without_simd(){
    clock_t start_time = 0, end_time = 0;
    start_time = clock();
    cout << "[Main]: Process without SIMD." << endl;
    
    YUVImage tmp_yuv(WIDTH, HEIGHT, "tmp_yuv");
    RGBImage tmp_rgb(WIDTH, HEIGHT, "tmp_rgb");
    
    /* YUV2RGB */
    yuv2rgb_ori(yuv1, rgb1);
    yuv2rgb_ori(yuv2, rgb2);
    
    /* Alpha Blending */
    for(int alpha = 1; alpha <= 255; alpha += 3){
        tmp_rgb.alpha_blend_ori(rgb1, alpha);
        /* RGB2YUV */
        rgb2yuv_ori(tmp_rgb, tmp_yuv);
        /* write to file */
        tmp_yuv.write_to_file(alpha_blend_output_filepath);
    }
    
    /* Superimposing */
    for(int alpha = 1; alpha <= 255; alpha += 3){
        tmp_rgb.superimpose_ori(rgb1, rgb2, alpha);
        /* RGB2YUV */
        rgb2yuv_ori(tmp_rgb, tmp_yuv);
        /* write to file */
        tmp_yuv.write_to_file(superimposing_output_filepath);
    }
    
    end_time = clock();
    return end_time - start_time;
}

int process_with_mmx(){
    /* YUV2RGB */
    
    /* Alpha Blending */
    
    /* Superimposing */
    
    /* RGB2YUV */
    
    return 0;
}

int process_with_sse2(){
    /* YUV2RGB */
    
    /* Alpha Blending */
    
    /* Superimposing */
    
    /* RGB2YUV */
    
    return 0;
}

int process_with_avx(){
    /* YUV2RGB */
    
    /* Alpha Blending */
    
    /* Superimposing */
    
    /* RGB2YUV */
    
    return 0;
}

int main(int argc, const char * argv[]) {
    /* Read-in file */
    yuv1.read_in_image(YUV_filepath1);
    yuv2.read_in_image(YUV_filepath2);
    
    clock_t time = init_time;
    
#ifdef NO_SIMD
    time = process_without_simd();
#endif
#ifdef MMX
    if(time != init_time){
        cerr << "[Main]: More than one mode defined!" << endl;
        return 0;
    }
    time = process_with_mmx();
#endif
#ifdef SSE2
    if(time != init_time){
        cerr << "[Main]: More than one mode defined!" << endl;
        return 0;
    }
    time = process_with_sse2();
#endif
#ifdef AVX
    if(time != init_time){
        cerr << "[Main]: More than one mode defined!" << endl;
        return 0;
    }
    time = process_with_avx();
#endif
    
    if(time == init_time){
        cerr << "[Main]: Mode undefined!" << endl;
        return 0;
    }
    
    cout << "[Main]: Total time = " << time << endl;
    return 0;
}
