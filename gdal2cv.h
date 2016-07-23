#ifndef __GDAL_CV_HPP__
#define __GDAL_CV_HPP__

#include <gdal_priv.h>
#include <gdal.h>

#include <opencv2/core/core.hpp>

class KGDAL2CV
{
public:
	KGDAL2CV();
	~KGDAL2CV();
	bool ImgWriteByGDAL(GDALDataset *, const cv::Mat, int = 0, int = 0);
	bool ImgWriteByGDAL(GDALRasterBand *, const cv::Mat, int = 0, int = 0);
	cv::Mat ImgReadByGDAL(cv::String, bool = true);
	cv::Mat ImgReadByGDAL(cv::String, int, int, int, int, bool = true);
	cv::Mat ImgReadByGDAL(GDALRasterBand*, int, int, int, int);
	cv::Mat ImgReadByGDAL(GDALRasterBand*);
	void Close();
private:
	GDALDataset* m_dataset;
	cv::String m_filename;
	GDALDriver* m_driver;
	bool hasColorTable;

	int m_width;
	int m_height;

	int m_type;
	int m_nBand;

	bool readHeader();
	bool readData(cv::Mat img);
	int gdal2opencv(const GDALDataType&, const int&);
	int gdalPaletteInterpretation2OpenCV(GDALPaletteInterp const&, GDALDataType const&);
	void write_ctable_pixel(const double&, const GDALDataType&, GDALColorTable const*, cv::Mat&, const int&, const int&, const int&);
	void write_pixel(const double&, const GDALDataType&, const int&, cv::Mat&, const int&, const int&, const int&);
	double range_cast(const GDALDataType&, const int&, const double&);
	double range_cast_inv(const GDALDataType&, const int&, const double&);
	bool CheckDataType(const GDALDataType&, cv::Mat);
	//防止拷贝构造以及赋值操作
	KGDAL2CV(const KGDAL2CV&);
	KGDAL2CV& operator=(const KGDAL2CV&);
};


#endif /*__GDAL_CV_HPP__*/
