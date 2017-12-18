//
//  main.cpp
//  YUV-Accelerator
//
//  Created by VegB on 17/12/2017.
//  Copyright © 2017 VegB. All rights reserved.
//

#include "image_conversion.hpp"
using namespace std;

//#define NO_SIMD
//#define MMX
//#define SSE2
#define AVX

#define WIDTH 1920
#define HEIGHT 1080

// Output a BMP from RGB(u8) for debugging.
#define OUTPUT_BMP "./output/out.bmp"
#define BMP_OUT(path, rgb)    do{FILE* bmpout = fopen(path, "wb"); \
rgb.write_bmp(bmpout); \
fclose(bmpout);} while(false)

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
    cout << "[Main]: Process without SIMD." << endl;
    
    YUVImage tmp_yuv(WIDTH, HEIGHT, "tmp_yuv");
    RGBImage tmp_rgb(WIDTH, HEIGHT, "tmp_rgb");
    
    clock_t time1 = 0, time2 = 0;
    
    /* YUV2RGB */
    start_time = clock();
    yuv2rgb_ori(yuv1, rgb1);
    yuv2rgb_ori(yuv2, rgb2);
    // Output a BMP file from INPUT_YUV_1.toRGB() to see if YUV::yuv2rgb() works correctly.
    // BMP_OUT(OUTPUT_BMP, rgb2);
    end_time = clock();
    time1 += end_time - start_time;
    
    /* Alpha Blending */
    for(int alpha = 1; alpha <= 255; alpha += 3){
        start_time = clock();
        tmp_rgb.alpha_blend_ori(rgb2, alpha);
        /* RGB2YUV */
        rgb2yuv_ori(tmp_rgb, tmp_yuv);
        end_time = clock();
        time1 += end_time - start_time;
        
        /* write to file */
        tmp_yuv.write_to_file(alpha_blend_output_filepath);
    }
    cout << "[Main]: alpha blending without SIMD: " << (double)time1 / CLOCKS_PER_SEC << " s"  << endl;
    
    /* ------------------------------------------------------------------------ */
    
    /* YUV2RGB */
    start_time = clock();
    yuv2rgb_ori(yuv1, rgb1);
    yuv2rgb_ori(yuv2, rgb2);
    end_time = clock();
    time2 += end_time - start_time;
    
    /* Superimposing */
    for(int alpha = 1; alpha <= 255; alpha += 3){
        start_time = clock();
        tmp_rgb.superimpose_ori(rgb2, rgb1, alpha);
        /* RGB2YUV */
        rgb2yuv_ori(tmp_rgb, tmp_yuv);
        end_time = clock();
        time2 += end_time - start_time;
        
        /* write to file */
        tmp_yuv.write_to_file(superimposing_output_filepath);
    }
    cout << "[Main]: superimposing without SIMD: " << (double)time2 / CLOCKS_PER_SEC << " s"  << endl;
    
    return (end_time - start_time);
}

clock_t process_with_mmx(){
    clock_t start_time = 0, end_time = 0;
    cout << "[Main]: Process with MMX." << endl;
    
    YUVImage tmp_yuv(WIDTH, HEIGHT, "tmp_yuv");
    RGBImage tmp_rgb(WIDTH, HEIGHT, "tmp_rgb");
    
    clock_t time1 = 0, time2 = 0;
    
    /* YUV2RGB */
    start_time = clock();
    yuv2rgb_ori(yuv1, rgb1);
    yuv2rgb_ori(yuv2, rgb2);
    // Output a BMP file from INPUT_YUV_1.toRGB() to see if YUV::yuv2rgb() works correctly.
    // BMP_OUT(OUTPUT_BMP, rgb2);
    end_time = clock();
    time1 += end_time - start_time;
    
    /* Alpha Blending */
    for(int alpha = 1; alpha <= 255; alpha += 3){
        start_time = clock();
        tmp_rgb.alpha_blend_mmx(rgb2, alpha);
        /* RGB2YUV */
        rgb2yuv_ori(tmp_rgb, tmp_yuv);
        end_time = clock();
        time1 += end_time - start_time;
        
        /* write to file */
        tmp_yuv.write_to_file(alpha_blend_output_filepath);
    }
    cout << "[Main]: Alpha blend time with MMX: " << (double)time1 / CLOCKS_PER_SEC << " s"  << endl;
    /* ------------------------------------------------------------------------ */
    
    /* YUV2RGB */
    start_time = clock();
    yuv2rgb_ori(yuv1, rgb1);
    yuv2rgb_ori(yuv2, rgb2);
    end_time = clock();
    time2 += end_time - start_time;
    
    /* Superimposing */
    for(int alpha = 1; alpha <= 255; alpha += 3){
        start_time = clock();
        tmp_rgb.superimpose_mmx(rgb2, rgb1, alpha);
        /* RGB2YUV */
        rgb2yuv_ori(tmp_rgb, tmp_yuv);
        end_time = clock();
        time2 += end_time - start_time;
        /* write to file */
        tmp_yuv.write_to_file(superimposing_output_filepath);
    }
    cout << "[Main]: Superimposing with MMX: " << (double)time2 / CLOCKS_PER_SEC << " s"  << endl;
    
    return (end_time - start_time);
}

clock_t process_with_sse2(){
    clock_t start_time = 0, end_time = 0;
    cout << "[Main]: Process with SSE2." << endl;
    
    YUVImage tmp_yuv(WIDTH, HEIGHT, "tmp_yuv");
    RGBImage tmp_rgb(WIDTH, HEIGHT, "tmp_rgb");
    
    clock_t time1 = 0, time2 = 0;
    start_time = clock();
    /* YUV2RGB */
    yuv2rgb_sse2(yuv1, rgb1);
    yuv2rgb_sse2(yuv2, rgb2);
    // Output a BMP file from INPUT_YUV_1.toRGB() to see if YUV::yuv2rgb() works correctly.
    // BMP_OUT(OUTPUT_BMP, rgb2);
    end_time = clock();
    time1 += end_time -start_time;
    
    /* Alpha Blending */
    for(int alpha = 1; alpha <= 255; alpha += 3){
        start_time = clock();
        tmp_rgb.alpha_blend_sse2(rgb2, alpha);
        /* RGB2YUV */
        rgb2yuv_sse2(tmp_rgb, tmp_yuv);
        end_time = clock();
        time1 += end_time - start_time;
        
        /* write to file */
        tmp_yuv.write_to_file(alpha_blend_output_filepath);
    }
    cout << "[Main]: Alpha blend time with SSE2: " << (double)time1 / CLOCKS_PER_SEC << " s"  << endl;
    
    /* ------------------------------------------------------------------------ */
    
    /* YUV2RGB */
    start_time = clock();
    yuv2rgb_sse2(yuv1, rgb1);
    yuv2rgb_sse2(yuv2, rgb2);
    end_time = clock();
    time2 += end_time - start_time;
    
    /* Superimposing */
    for(int alpha = 1; alpha <= 255; alpha += 3){
        start_time = clock();
        tmp_rgb.superimpose_sse2(rgb2, rgb1, alpha);
        /* RGB2YUV */
        rgb2yuv_sse2(tmp_rgb, tmp_yuv);
        end_time = clock();
        time2 += end_time - start_time;
        /* write to file */
        tmp_yuv.write_to_file(superimposing_output_filepath);
    }
    cout << "[Main]: Superimposing with SSE2: " << (double)time2 / CLOCKS_PER_SEC << " s"  << endl;
    
    return end_time - start_time;
}

clock_t process_with_avx(){
    clock_t start_time = 0, end_time = 0;
    cout << "[Main]: Process with AVX." << endl;
    
    YUVImage tmp_yuv(WIDTH, HEIGHT, "tmp_yuv");
    RGBImage tmp_rgb(WIDTH, HEIGHT, "tmp_rgb");
    
    clock_t time1 = 0, time2 = 0;
    start_time = clock();
    /* YUV2RGB */
    yuv2rgb_avx(yuv1, rgb1);
    yuv2rgb_avx(yuv2, rgb2);
    // Output a BMP file from INPUT_YUV_1.toRGB() to see if YUV::yuv2rgb() works correctly.
    // BMP_OUT(OUTPUT_BMP, rgb2);
    end_time = clock();
    time1 += end_time - start_time;
    
    /* Alpha Blending */
    for(int alpha = 1; alpha <= 255; alpha += 3){
        start_time = clock();
        tmp_rgb.alpha_blend_avx(rgb2, alpha);
        /* RGB2YUV */
        rgb2yuv_avx(tmp_rgb, tmp_yuv);
        end_time = clock();
        time1 += end_time - start_time;
        
        /* write to file */
        tmp_yuv.write_to_file(alpha_blend_output_filepath);
    }
    cout << "[Main]: Alpha blend time with AVX: " << (double)time1 / CLOCKS_PER_SEC << " s"  << endl;
    
    /* ------------------------------------------------------------------------ */

    /* YUV2RGB */
    start_time = clock();
    yuv2rgb_avx(yuv1, rgb1);
    yuv2rgb_avx(yuv2, rgb2);
    end_time = clock();
    time2 += end_time - start_time;
    
    /* Superimposing */
    for(int alpha = 1; alpha <= 255; alpha += 3){
        start_time = clock();
        tmp_rgb.superimpose_avx(rgb2, rgb1, alpha);
        /* RGB2YUV */
        rgb2yuv_ori(tmp_rgb, tmp_yuv);
        end_time = clock();
        time2 += end_time - start_time;
        /* write to file */
        tmp_yuv.write_to_file(superimposing_output_filepath);
    }
    cout << "[Main]: Superimposing with AVX: " << (double)time2 / CLOCKS_PER_SEC << " s"  << endl;
    return (end_time - start_time);
}

void check_file_size(){
    FILE *file=fopen(YUV_filepath1.c_str(),"rb");
    fseek(file,0,SEEK_END);
    long long n = ftell(file);
    cout << "[Main]: Input size: " << n <<endl;
    fclose(file);
    
    file=fopen(alpha_blend_output_filepath.c_str(),"rb");
    fseek(file,0,SEEK_END);
    n = ftell(file);
    cout << "[Main]: Alpha blend output size: " << n <<endl;
    fclose(file);
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
    
    return 0;
}
