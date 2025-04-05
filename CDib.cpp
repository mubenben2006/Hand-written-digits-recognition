#include "CDib.h"
#include<fstream>

using namespace std;

bool CDib::Load(const char* filename)
{
    ifstream ifs(filename, ios::binary);  //用二进制方式打开文件流
    ifs.seekg(0, ios::end);  //让文件读指针（或写指针）指向从文件结尾往前的 |offset|（offset 的绝对值）字节处
    int size = ifs.tellg();
    ifs.seekg(0, ios::beg);  //文件指针指向文件头
    ifs.read((char*)&m_BitmapFileHeader, sizeof(BITMAPFILEHEADER));  //读取位图文件头
    if (m_BitmapFileHeader.bfType != 0x4d42); {
        throw"文件类型不正确";
    }
    if (m_BitmapFileHeader.bfSize != size); {
        throw"文件格式不正确";
    }
    pDib = new BYTE[size - sizeof(BITMAPFILEHEADER)];  //用于存取文件后三部分数据
    ifs.read((char*)pDib, size - sizeof(BITMAPFILEHEADER));  //读取文件后三部分数据
    m_pBitmapInfoHeader = (BITMAPINFOHEADER*)pDib;
    m_pRgbQuad = (RGBQUAD*)(pDib + sizeof(BITMAPINFOHEADER));
    int colorTableSize = m_BitmapFileHeader.bfOffBits-sizeof(BITMAPFILEHEADER)-m_pBitmapInfoHeader->biSize;
    int numberOfColors = GetNumberOfColors();  //获取颜色数目
    if (numberOfColors * sizeof(RGBQUAD) != colorTableSize) {
        delete[]pDib;
        pDib = nullptr;
        throw"颜色表大小计算错误!";
    }
    m_pData = pDib + sizeof(BITMAPINFOHEADER) + colorTableSize;
    return true;
}

bool CDib::Save(const char* filename)
{
    if(!pDib) return false;
    ofstream ofs(filename, ios::binary);
    if (ofs.fail()) return false;
    ofs.write((char*)&m_BitmapFileHeader, sizeof(BITMAPFILEHEADER));  //写位图文件头结构
    ofs.write((char*)pDib, m_BitmapFileHeader.bfSize - sizeof(BITMAPFILEHEADER));
    ofs.close();
    return true;
}

bool CDib::Create(int nWidth, int nHeight, int nColor)
{
    if (pDib) delete[]pDib;
    int colorTableSize = 0;
    if (nColor == 1)  colorTableSize = 2 * sizeof(RGBQUAD);
    else if (nColor == 4 || nColor == 8)  colorTableSize = 8 * sizeof(RGBQUAD);
    else nColor = 24;
    int bytePerLine = ((nWidth * nColor + 31) / 32) * 4;  //计算每行占用的字节数
    int dataSize = nHeight * bytePerLine;  //计算所有像素占用的字节数
    m_BitmapFileHeader.bfType = 0x4d42;
    m_BitmapFileHeader.bfReserved1 = 0;
    m_BitmapFileHeader.bfReserved2 = 0;
    m_BitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize;
    m_BitmapFileHeader.bfSize = m_BitmapFileHeader.bfOffBits + dataSize;
    pDib = new BYTE[m_BitmapFileHeader.bfSize - sizeof(BITMAPINFOHEADER)];
    m_pBitmapInfoHeader = (BITMAPINFOHEADER*)pDib;
    m_pBitmapInfoHeader->biSize = sizeof(tagBITMAPINFOHEADER);
    m_pBitmapInfoHeader->biBitCount = nColor;
    m_pBitmapInfoHeader->biClrImportant = 1;
    m_pBitmapInfoHeader->biClrUsed = colorTableSize / sizeof(RGBQUAD);
    m_pBitmapInfoHeader->biCompression = 0;
    m_pBitmapInfoHeader->biPlanes = 1;
    m_pBitmapInfoHeader->biSizeImage = dataSize;
    m_pBitmapInfoHeader->biYPelsPerMeter = 1024;
    m_pBitmapInfoHeader->biHeight = nHeight;
    m_pBitmapInfoHeader->biWidth = nWidth;
    m_pData = pDib + m_BitmapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
    m_pRgbQuad = (RGBQUAD*)(pDib + sizeof(BITMAPINFOHEADER));
    switch (nColor) {
    case 1:  //单色图像只有两种颜色
        SetColor(m_pRgbQuad, 0xff, 0xff, 0xff);  //索引值为0的颜色为白色
        SetColor(m_pRgbQuad + 1, 0, 0, 0);  //索引值为1的颜色为黑色
        memset(m_pData, 0x00, dataSize);
        break;
    case 4:  //只定义八种颜色0xffffff,0xffff00,0xff00ff,0xff0000,0x00ffff
    case 8:  //0x00ff00,0x0000ff,0x000000
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                    SetColor(m_pRgbQuad + 7 - i * 4 - j * 2 - k, i * 255, j * 255, k * 255);
        }
        memset(m_pData, 0x00, dataSize);  //设置图像中每个像素颜色索引值为零
        break;
    default:memset(m_pData, 0xff, dataSize);  //设置真彩色图像每个像素均为白色
    }
    return true;
}

int CDib::GetNumberOfColors()
{
    int numberOfColors = 0;
    if (m_pBitmapInfoHeader->biClrUsed)  //信息头中是否定义了颜色表的项目数
        numberOfColors = m_pBitmapInfoHeader->biClrUsed;
    else switch (m_pBitmapInfoHeader->biBitCount) {
    case 1:numberOfColors = 2; break;
    case 4:numberOfColors = 16; break;
    case 8:numberOfColors = 256; break;  
    }
    return numberOfColors;
}

void CDib::SetColor(RGBQUAD* rgb, BYTE r, BYTE g, BYTE b)
{
    if (rgb) {
        rgb->rgbRed = r;
        rgb->rgbGreen = g;
        rgb->rgbBlue = b;
        rgb->rgbReserved = 0;
    }
    else throw"表项不存在";
}
