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
    uint8_t* r;
    uint8_t* g;
    uint8_t* b;
    
    RGBImage(int w, int l, string name_){
        width = w;
        height = l;
        name = name_;
        r = new uint8_t[width * height];
        g = new uint8_t[width * height];
        b = new uint8_t[width * height];
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
};

#endif /* rgb_hpp */

