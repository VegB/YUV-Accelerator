//
//  rgb.hpp
//  rgb-Accelerator
//
//  Created by 朱芄蓉 on 17/12/2017.
//  Copyright © 2017 朱芄蓉. All rights reserved.
//

#ifndef rgb_hpp
#define rgb_hpp

#include "helper.hpp"
using namespace std;

/* stores a single RGB Image */
class RGBImage{
public:
    string name;
    int width;
    int height;
    uint8_t *r, *g, *b;
    
    RGBImage(int w, int l, string name_){
        width = w;
        height = l;
        name = name_;
        r = new uint8_t[width * height];
        g = new uint8_t[width * height];
        b = new uint8_t[width * height];
        // cout << "RGB Image created!" << endl;
    };
    
    ~RGBImage(){
        delete []r;
        delete []g;
        delete []b;
        // cout << "RGB Imaged destroyed!" << endl;
    };
    
    void read_in_image(string filename);
    void init_image();
    void alpha_blend_ori(RGBImage& src, int alpha);
    void alpha_blend_mmx(RGBImage& src, int alpha);
    void alpha_blend_sse2(RGBImage& src, int alpha);
    void alpha_blend_avx(RGBImage& src, int alpha);
    void superimpose_ori(RGBImage& img1, RGBImage& img2, int alpha);
    void superimpose_mmx(RGBImage& img1, RGBImage& img2, int alpha);
    void superimpose_sse2(RGBImage& img1, RGBImage& img2, int alpha);
    void superimpose_avx(RGBImage& img1, RGBImage& img2, int alpha);
    void write_bmp(FILE* fout) const;
    void single_color_blend_mmx(uint8_t*src, uint8_t* dst, int pos, __m64 alpha);
    void single_color_impose_mmx(uint8_t* src1, uint8_t* src2, uint8_t* dst, int pos);
};

#endif /* rgb_hpp */

