#pragma once

typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned char BYTE;

typedef struct tagBITMAPFILEHEADER {
	WORD bfType;  //位图文件的类型，2字节，必须为BM两字符的ASCII，即OX4D42
	DWORD bfSize;  //位图文件的大小，以字节为单位
	WORD bfReserved1;  //保留字，必须为0
	WORD bfReserved2;  //保留字，必须为0
	DWORD bfOffBits;  //位图数据的起始位置，即相对于位图文件头的偏移量
}BITMAPFILEHEADER;  //位图文件头定义

typedef struct tagBITMAPINFOHEADER {
	DWORD biSize;  //本结构占用字节数
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;  //目标设备的级别，必须为1
	WORD biBitCount;  //每个像素所需要的位数，一般为1（双色），4（16色），8（256色）或24（真彩色）等
	DWORD biCompression; //压缩类型，一般为0（不压缩），1（BI_RLE8压缩），2（BI_RLE4压缩）
	DWORD biSizeImage;  //位图的大小，以字节为单位
	LONG biXPelsPerMeter;  //位图水平分辨率（每米像素数）
	LONG biYPelsPerMeter;
	DWORD biClrUsed;  //位图使用过程中颜色表中使用的颜色数
	DWORD biClrImportant;  //位图显示过程中重要的颜色数
}BITMAPINFOHEADER;  //位图信息头定义

//位图颜色表用于说明每种像素值所代表的颜色
typedef struct tagRGBQUAR {
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;  //红色的亮度（范围0~255）
	BYTE rgbReserved;  //保留字，必须为0
}RGBQUAD;  //位图颜色表表项定义

class CDib
{
public:
	CDib() { pDib = nullptr; };
	~CDib() { if (pDib) delete[] pDib; };
	bool Load(const char* filename);  //打开BMP文件
	bool Save(const char* filename);  //保存BMP文件
	bool Create(int nWidth, int nHeight, int nColor);  //建立默认BMP结构
	BYTE* m_pData;  //像素阵列
private:
	int GetNumberOfColors();  //获取位图颜色表的表项数目
	void SetColor(RGBQUAD* rgb, BYTE r, BYTE g, BYTE b);  //设置位图颜色表表项
	BITMAPFILEHEADER m_BitmapFileHeader;  //BMP文件头
	BITMAPINFOHEADER* m_pBitmapInfoHeader;  //指向BMP文件信息头
	RGBQUAD* m_pRgbQuad;  //指向位图颜色表
	BYTE* pDib;  //指向CDib中三个指针动态存储在的一块连续空间
};

