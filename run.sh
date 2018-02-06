rm ./output/*
g++ -g helper.cpp yuv.cpp rgb.cpp image_conversion.cpp main.cpp -o main
./main
