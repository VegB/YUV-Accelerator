typedef unsigned short int WORD;
typedef unsigned int DWORD;
typedef int LONG;
typedef unsigned char BYTE;

#pragma pack(1)

typedef struct tagBITMAPFILEHEADER
{
    WORD bfType = 19778;
    DWORD bfSize = 6220854;
    WORD bfReserved1 = 0;
    WORD bfReserved2 = 0;
    DWORD bfOffBits = 54;
}t_BMPfileheader;


typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize = 40;
    LONG biWidth = 1920;
    LONG biHeight = 1080;
    WORD biPlanes = 1;
    WORD biBitCount = 24;
    DWORD biCompression = 0;
    DWORD biSizeImage = 6220800;
    LONG biXPelsPerMeter = 2835;
    LONG biYPelsPerMeter = 2835;
    DWORD biClrUsed = 0;
    DWORD biClrImportant = 0;
}t_BMPinfoheader;
#pragma pack()
