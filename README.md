# GDAL2CV
Some useful interface for convert between cv::Mat and GDALDataset.
提供了一些用于OpenCV中Mat类型与GDAL库的转换接口，主要是读取和存储图像方面的，可以自动处理类型转换问题，同时提供了分块存储功能。

## cv::Mat ImgReadByGDAL(cv::String filename, bool beReadFourth = true);
* 从文件中使用GDAL的接口读取数据，返回cv::Mat类型，beReadFourth表示当数据集为四通道且数据类型不为GByte时是否仍将其读取到cv::Mat中，默认不读取。
## cv::Mat ImgReadByGDAL(cv::String filename, int xStart, int yStart, int xWidth, int yWidth, bool beReadFourth = true);
* 从文件中使用GDAL的接口在指定起点读取指定大小的数据，返回cv::Mat类型，beReadFourth选项作用同上。
## cv::Mat ImgReadByGDAL(GDALRasterBand* pBand, int xStart, int yStart, int xWidth, int yWidth);
* 从已经打开的波段中指定起点读取指定大小的数据，返回cv::Mat类型
## cv::Mat ImgReadByGDAL(GDALRasterBand* pBand);
* 从已经打开的波段中读取数据，返回cv::Mat类型
## bool ImgWriteByGDAL(GDALDataset* dataset, const cv::Mat img, int xStart = 0, int yStart = 0);
* 向指定已打开的具有写入权限的数据集中写入Mat中的数据，写入前需确认多通道Mat为RGB顺序，可以指定数据集中的写入起点，写入大小默认为img大小，根据数据集大小自动调整。
## bool ImgWriteByGDAL(GDALRasterBand * pBand, const cv::Mat img, int xStart = 0, int yStart = 0);
* 向指定已打开的具有写入权限的波段中写入Mat中的单通道数据（多通道图像只取第一通道），可以指定要写入波段中的写入起点，写入大小默认为img大小，根据数据集大小自动调整。
## void Close();
* 关闭已打开的数据集，由析构函数自动调用，也可手动调用。

# License

The MIT License (MIT) && Intel License Agreement
