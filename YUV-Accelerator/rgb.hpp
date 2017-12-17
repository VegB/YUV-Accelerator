//
//  rgb.hpp
//  rgb-Accelerator
//
//  Created by 朱芄蓉 on 17/12/2017.
//  Copyright © 2017 朱芄蓉. All rights reserved.
//

#ifndef rgb_hpp
#define rgb_hpp

#include <iostream>
#include <string>
using namespace std;

/* stores a single RGB Image */
class RGBImage{
public:
    string name;
    int width;
    int height;
    unsigned char* r;
    unsigned char* g;
    unsigned char* b;
    
    RGBImage(int w, int l, string name_){
        width = w;
        height = l;
        name = name_;
        r = new unsigned char[width * height];
        g = new unsigned char[width * height];
        b = new unsigned char[width * height];
        cout << "RGB Image created!" << endl;
    };
    
    ~RGBImage(){
        delete []r;
        delete []g;
        delete []b;
        cout << "RGB Imaged destroyed!" << endl;
    };
    
    void read_in_image(string filename);
    void init_image();
    void alpha_blend_ori(RGBImage& src, int alpha);
    void superimpose_ori(RGBImage& img1, RGBImage& img2, int alpha);
    void write_bmp(FILE* fout) const;
};

#endif /* rgb_hpp */

