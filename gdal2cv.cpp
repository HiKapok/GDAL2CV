/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "gdal2cv.h"
#include <iostream>
#include <vector>

/**
* Convert GDAL Palette Interpretation to OpenCV Pixel Type
*/
int  KGDAL2CV::gdalPaletteInterpretation2OpenCV(GDALPaletteInterp const& paletteInterp, GDALDataType const& gdalType){

	switch (paletteInterp){

		/// GRAYSCALE
	case GPI_Gray:
		if (gdalType == GDT_Byte){ return CV_8UC1; }
		if (gdalType == GDT_UInt16){ return CV_16UC1; }
		if (gdalType == GDT_Int16){ return CV_16SC1; }
		if (gdalType == GDT_UInt32){ return CV_32SC1; }
		if (gdalType == GDT_Int32){ return CV_32SC1; }
		if (gdalType == GDT_Float32){ return CV_32FC1; }
		if (gdalType == GDT_Float64){ return CV_64FC1; }
		return -1;

		/// RGB
	case GPI_RGB:
		if (gdalType == GDT_Byte){ return CV_8UC1; }
		if (gdalType == GDT_UInt16){ return CV_16UC3; }
		if (gdalType == GDT_Int16){ return CV_16SC3; }
		if (gdalType == GDT_UInt32){ return CV_32SC3; }
		if (gdalType == GDT_Int32){ return CV_32SC3; }
		if (gdalType == GDT_Float32){ return CV_32FC3; }
		if (gdalType == GDT_Float64){ return CV_64FC3; }
		return -1;


		/// otherwise
	default:
		return -1;

	}
}

/**
* Convert gdal type to opencv type
*/
int KGDAL2CV::gdal2opencv(const GDALDataType& gdalType, const int& channels){

	switch (gdalType){

		/// UInt8
	case GDT_Byte:
		if (channels == 1){ return CV_8UC1; }
		if (channels == 3){ return CV_8UC3; }
		if (channels == 4){ return CV_8UC4; }
		else { return CV_8UC(channels); }
		return -1;

		/// UInt16
	case GDT_UInt16:
		if (channels == 1){ return CV_16UC1; }
		if (channels == 3){ return CV_16UC3; }
		if (channels == 4){ return CV_16UC4; }
		else { return CV_16UC(channels); }
		return -1;

		/// Int16
	case GDT_Int16:
		if (channels == 1){ return CV_16SC1; }
		if (channels == 3){ return CV_16SC3; }
		if (channels == 4){ return CV_16SC4; }
		else { return CV_16SC(channels); }
		return -1;

		/// UInt32
	case GDT_UInt32:
	case GDT_Int32:
		if (channels == 1){ return CV_32SC1; }
		if (channels == 3){ return CV_32SC3; }
		if (channels == 4){ return CV_32SC4; }
		else { return CV_32SC(channels); }
		return -1;

	case GDT_Float32:
		if (channels == 1){ return CV_32FC1; }
		if (channels == 3){ return CV_32FC3; }
		if (channels == 4){ return CV_32FC4; }
		else { return CV_32FC(channels); }
		return -1;

	case GDT_Float64:
		if (channels == 1){ return CV_64FC1; }
		if (channels == 3){ return CV_64FC3; }
		if (channels == 4){ return CV_64FC4; }
		else { return CV_64FC(channels); }
		return -1;

	default:
		std::cout << "Unknown GDAL Data Type" << std::endl;
		std::cout << "Type: " << GDALGetDataTypeName(gdalType) << std::endl;
		return -1;
	}

	return -1;
}

bool KGDAL2CV::readHeader()
{
	// load the dataset
	m_dataset = static_cast<GDALDataset*>(GDALOpen(m_filename.c_str(), GA_ReadOnly));

	// if dataset is null, then there was a problem
	if (m_dataset == nullptr){
		return false;
	}

	// make sure we have pixel data inside the raster
	if (m_dataset->GetRasterCount() <= 0){
		return false;
	}

	//extract the driver infomation
	m_driver = m_dataset->GetDriver();

	// if the driver failed, then exit
	if (m_driver == nullptr){
		return false;
	}


	// get the image dimensions
	m_width = m_dataset->GetRasterXSize();
	m_height = m_dataset->GetRasterYSize();

	m_nBand = m_dataset->GetRasterCount();
	// make sure we have at least one band/channel
	if (m_dataset->GetRasterCount() <= 0){
		return false;
	}

	// check if we have a color palette
	int tempType;
	if (m_dataset->GetRasterBand(1)->GetColorInterpretation() == GCI_PaletteIndex){

		// remember that we have a color palette
		hasColorTable = true;

		// if the color tables does not exist, then we failed
		if (m_dataset->GetRasterBand(1)->GetColorTable() == NULL){
			return false;
		}

		// otherwise, get the pixeltype
		
		// convert the palette interpretation to opencv type
		tempType = gdalPaletteInterpretation2OpenCV(m_dataset->GetRasterBand(1)->GetColorTable()->GetPaletteInterpretation(),
			m_dataset->GetRasterBand(1)->GetRasterDataType());

		if (tempType == -1){
			return false;
		}
		m_type = tempType;	

	}

	// otherwise, we have standard channels
	else{

		// remember that we don't have a color table
		hasColorTable = false;

		// convert the datatype to opencv
		tempType = gdal2opencv(m_dataset->GetRasterBand(1)->GetRasterDataType(), m_dataset->GetRasterCount());
		if (tempType == -1){
			return false;
		}
		m_type = tempType;
	}

	return true;
}

bool KGDAL2CV::CheckDataType(const GDALDataType& gdalDataType, cv::Mat img)
{
	int TypeMap_GDAL2_0[GDT_TypeCount] = { CV_USRTYPE1, CV_8U, CV_16U, CV_16S, CV_32S, CV_32S, CV_32F, CV_64F, CV_USRTYPE1, CV_USRTYPE1, CV_USRTYPE1, CV_USRTYPE1};
	int imgType = img.type();
	
	if (imgType == CV_MAKETYPE(TypeMap_GDAL2_0[gdalDataType], img.channels()))
	{
		if (gdalDataType == GDT_UInt32) std::cout << "cv::Mat doesn't support datatype: CV_32U!" << std::endl;
		if (TypeMap_GDAL2_0[gdalDataType] == CV_USRTYPE1) std::cout << "user define datatype may be unmatched!" << std::endl;
		return true;
	} 
	std::cout << "Warning: use the different Data Type between cv::Mat and GDAL!\r\n" 
				 "\tproper range cast may be used!" << std::endl;
	return false;
}

/**
* Convert data range
*/
double KGDAL2CV::range_cast(const GDALDataType& gdalType,
	const int& cvDepth,
	const double& value)
{
	// uint8 -> uint8
	if (gdalType == GDT_Byte && cvDepth == CV_8U){
		return value;
	}
	// uint8 -> uint16
	if (gdalType == GDT_Byte && (cvDepth == CV_16U || cvDepth == CV_16S)){
		return (value * 256);
	}

	// uint8 -> uint32
	if (gdalType == GDT_Byte && (cvDepth == CV_32F || cvDepth == CV_32S)){
		return (value * 16777216);
	}

	// int16 -> uint8
	if ((gdalType == GDT_UInt16 || gdalType == GDT_Int16) && cvDepth == CV_8U){
		return std::floor(value / 256.0);
	}

	// int16 -> int16
	if ((gdalType == GDT_UInt16 || gdalType == GDT_Int16) &&
		(cvDepth == CV_16U || cvDepth == CV_16S)){
		return value;
	}

	// float32 -> float32
	// float64 -> float64
	if ((gdalType == GDT_Float32 || gdalType == GDT_Float64) &&
		(cvDepth == CV_32F || cvDepth == CV_64F)){
		return value;
	}

	std::cout << GDALGetDataTypeName(gdalType) << std::endl;
	std::cout << "warning: unknown range cast requested." << std::endl;
	return (value);
}

double KGDAL2CV::range_cast_inv(const GDALDataType& gdalType,
	const int& cvDepth,
	const double& value)
{
	// uint8 -> uint8
	if (gdalType == GDT_Byte && cvDepth == CV_8U){
		return value;
	}
	// uint16 -> uint8
	if (gdalType == GDT_Byte && (cvDepth == CV_16U || cvDepth == CV_16S)){
		return std::floor(value / 256.f);
	}

	// uint32 -> uint8
	if (gdalType == GDT_Byte && (cvDepth == CV_32F || cvDepth == CV_32S)){
		return std::floor(value / 16777216.f);
	}

	// int8 -> uint16
	if ((gdalType == GDT_UInt16 || gdalType == GDT_Int16) && cvDepth == CV_8U){
		return (value * 256.f);
	}

	// int16 -> int16
	if ((gdalType == GDT_UInt16 || gdalType == GDT_Int16) &&
		(cvDepth == CV_16U || cvDepth == CV_16S)){
		return value;
	}

	// float32 -> float32
	// float64 -> float64
	if ((gdalType == GDT_Float32 || gdalType == GDT_Float64) &&
		(cvDepth == CV_32F || cvDepth == CV_64F)){
		return value;
	}

	std::cout << GDALGetDataTypeName(gdalType) << std::endl;
	std::cout << "warning: unknown range cast requested." << std::endl;
	return (value);
}

// be sure the cv::Mat either a gray image or in RGB order!
bool KGDAL2CV::ImgWriteByGDAL(GDALDataset * dataset, const cv::Mat img, int xStart, int yStart)
{
	// if dataset is null, then there was a problem
	if (dataset == nullptr){
		return false;
	}
	// make sure we have pixel data inside the raster
	if (dataset->GetRasterCount() <= 0){
		return false;
	}
	// make sure we have the proper access

	if (dataset->GetAccess() == GA_ReadOnly){
		std::cout << "Invalid access type of the dataset!" << std::endl;
		return false;
	}

	int nBand = dataset->GetRasterCount();

	if (nBand > img.channels())
	{
		std::cout << "The channels of GDALDataset shouldn't be more than cv::Mat!" << std::endl;
		return false;
	}

	int width = dataset->GetRasterXSize();
	int height = dataset->GetRasterYSize();

	if (xStart < 0 || yStart < 0 || xStart >= width || yStart >= height)
	{
		std::cout << "wrong param!" << std::endl;
		return false;
	}
	cv::Mat imgToSave = img;
	int xWidth = imgToSave.cols;
	int yWidth = imgToSave.rows;

	if (xStart + xWidth > width)
	{
		std::cout << "Saved image will be cutted!" << std::endl;
		imgToSave = imgToSave.colRange(0, width - xStart);
	}
	if (yStart + yWidth > height)
	{
		std::cout << "Saved image will be cutted!" << std::endl;
		imgToSave = imgToSave.rowRange(0, height - yStart);
	}

	/*if (width != img.cols || height != img.rows) {
		std::cout << "The size of cv::Mat and GDALDataset should be same!" << std::endl;
		return false;
	}*/

	GDALDataType dataType = dataset->GetRasterBand(1)->GetRasterDataType();
	CheckDataType(dataType, imgToSave);
	std::vector<cv::Mat> singleMats;
	int ret = 0;
	cv::split(imgToSave, singleMats);
	for (int index = 0; index < nBand; ++index)
	{
		GDALRasterBand* band = dataset->GetRasterBand(index + 1);
		ret += (true == ImgWriteByGDAL(band, singleMats[index], xStart, yStart) ? 0 : 1);
	}
	
	return (0 == ret);
}

bool KGDAL2CV::ImgWriteByGDAL(GDALRasterBand* pBand, const cv::Mat img, int xStart, int yStart)
{
	// if dataset is null, then there was a problem
	if (pBand == nullptr){
		return false;
	}
	// make sure we have the proper access
	if (pBand->GetAccess() == GA_ReadOnly){
		std::cout << "Invalid access type of the GDALRasterBand!" << std::endl;
		return false;
	}
	cv::Mat imgToSave = img.clone(); 
	if (img.channels() > 1){
		std::cout << "More channels of the cv::Mat will be passed!" << std::endl;
		std::vector<cv::Mat> singleMats;
		cv::split(img, singleMats);
		//singleMats[0].copyTo(imgToSave);
		imgToSave = singleMats[0].clone();
	}

	int width = pBand->GetXSize();
	int height = pBand->GetYSize();

	if (xStart < 0 || yStart < 0 || xStart >= width || yStart >= height)
	{
		std::cout << "wrong param!" << std::endl;
		return false;
	}
	
	int xWidth = imgToSave.cols;
	int yWidth = imgToSave.rows;

	if (xStart + xWidth > width)
	{
		std::cout << "Saved image will be cutted!" << std::endl;
		imgToSave = (imgToSave.colRange(0, width - xStart)).clone();
	}
	if (yStart + yWidth > height)
	{
		std::cout << "Saved image will be cutted!" << std::endl;
		imgToSave = (imgToSave.rowRange(0, height - yStart)).clone();
	}

	GDALDataType dataType = pBand->GetRasterDataType();
	CheckDataType(dataType, imgToSave);
	
	xWidth = imgToSave.cols;
	yWidth = imgToSave.rows;
	int cvDepth = imgToSave.depth();
	imgToSave.reshape(1, 1);

	double *imgBuff = new(std::nothrow) double[xWidth * yWidth];
	if (nullptr == imgBuff) return false;

	if (cvDepth == CV_8S)
	{
		char * data = imgToSave.ptr<char>(0);
		for (int index = 0; index < xWidth * yWidth; ++index) imgBuff[index] = static_cast<double>(data[index]);
	}
	else if (cvDepth == CV_16U)
	{
		unsigned short * data = imgToSave.ptr<unsigned short>(0);
		for (int index = 0; index < xWidth * yWidth; ++index) imgBuff[index] = static_cast<double>(data[index]);
	}
	else if (cvDepth == CV_16S)
	{
		short * data = imgToSave.ptr<short>(0);
		for (int index = 0; index < xWidth * yWidth; ++index) imgBuff[index] = static_cast<double>(data[index]);
	}
	else if (cvDepth == CV_32S)
	{
		int * data = imgToSave.ptr<int>(0);
		for (int index = 0; index < xWidth * yWidth; ++index) imgBuff[index] = static_cast<double>(data[index]);
	}
	else if (cvDepth == CV_32F)
	{
		float * data = imgToSave.ptr<float>(0);
		for (int index = 0; index < xWidth * yWidth; ++index) imgBuff[index] = static_cast<double>(data[index]);
	}
	else if (cvDepth == CV_64F)
	{
		double * data = imgToSave.ptr<double>(0);
		for (int index = 0; index < xWidth * yWidth; ++index) imgBuff[index] = static_cast<double>(data[index]);
	}
	else{
		unsigned char * data = imgToSave.ptr<unsigned char>(0);
		for (int index = 0; index < xWidth * yWidth; ++index) imgBuff[index] = static_cast<double>(data[index]);
	}

	// datatype translate
	//for (int index = 0; index < xWidth * yWidth; ++index) imgBuff[index] = range_cast_inv(dataType, cvDepth, imgBuff[index]);
	
	pBand->RasterIO(GF_Write, xStart, yStart, xWidth, yWidth, imgBuff, xWidth, yWidth, GDT_Float64, 0, 0);
	pBand->FlushCache();

	delete[] imgBuff;
	return true;
}

/**
* There are some better mpl techniques for doing this.
*/
void KGDAL2CV::write_pixel(const double& pixelValue,
	const GDALDataType& gdalType,
	const int& gdalChannels,
	cv::Mat& image,
	const int& row,
	const int& col,
	const int& channel){

	// convert the pixel
	double newValue = range_cast(gdalType, image.depth(), pixelValue);

	// input: 1 channel, output: 1 channel
	if (gdalChannels == 1 && image.channels() == 1){
		if (image.depth() == CV_8U){ image.ptr<uchar>(row)[col] = static_cast<uchar>(newValue); }
		else if (image.depth() == CV_16U){ image.ptr<unsigned short>(row)[col] = static_cast<unsigned short>(newValue); }
		else if (image.depth() == CV_16S){ image.ptr<short>(row)[col] = static_cast<short>(newValue); }
		else if (image.depth() == CV_32S){ image.ptr<int>(row)[col] = static_cast<int>(newValue); }
		else if (image.depth() == CV_32F){ image.ptr<float>(row)[col] = static_cast<float>(newValue); }
		else if (image.depth() == CV_64F){ image.ptr<double>(row)[col] = newValue; }
		else{ throw std::runtime_error("Unknown image depth, gdal: 1, img: 1"); }
	}

	// input: 1 channel, output: 3 channel
	else if (gdalChannels == 1 && image.channels() == 3){
		if (image.depth() == CV_8U){ image.ptr<cv::Vec3b>(row)[col] = cv::Vec3b(newValue, newValue, newValue); }
		else if (image.depth() == CV_16U){ image.ptr<cv::Vec3w>(row)[col] = cv::Vec3w(newValue, newValue, newValue); }
		else if (image.depth() == CV_16S){ image.ptr<cv::Vec3s>(row)[col] = cv::Vec3s(newValue, newValue, newValue); }
		else if (image.depth() == CV_32S){ image.ptr<cv::Vec3i>(row)[col] = cv::Vec3i(newValue, newValue, newValue); }
		else if (image.depth() == CV_32F){ image.ptr<cv::Vec3f>(row)[col] = cv::Vec3f(newValue, newValue, newValue); }
		else if (image.depth() == CV_64F){ image.ptr<cv::Vec3d>(row)[col] = cv::Vec3d(newValue, newValue, newValue); }
		else{ throw std::runtime_error("Unknown image depth, gdal:1, img: 3"); }
	}

	// input: 3 channel, output: 1 channel
	else if (gdalChannels == 3 && image.channels() == 1){
		if (image.depth() == CV_8U){ image.ptr<uchar>(row)[col] += (newValue / 3.0); }
		else{ throw std::runtime_error("Unknown image depth, gdal:3, img: 1"); }
	}

	// input: 4 channel, output: 1 channel
	else if (gdalChannels == 4 && image.channels() == 1){
		if (image.depth() == CV_8U){ image.ptr<uchar>(row)[col] = newValue; }
		else{ throw std::runtime_error("Unknown image depth, gdal: 4, image: 1"); }
	}

	// input: 3 channel, output: 3 channel
	else if (gdalChannels == 3 && image.channels() == 3){
		if (image.depth() == CV_8U){ image.at<cv::Vec3b>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_16U){ image.at<cv::Vec3w>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_16S){ image.at<cv::Vec3s>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_32S){ image.at<cv::Vec3i>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_32F){ image.at<cv::Vec3f>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_64F){ image.at<cv::Vec3d>(row, col)[channel] = newValue; }
		else{ throw std::runtime_error("Unknown image depth, gdal: 3, image: 3"); }
	}

	// input: 4 channel, output: 3 channel
	else if (gdalChannels == 4 && image.channels() == 3){
		if (channel >= 4){ return; }
		else if (image.depth() == CV_8U  && channel < 4){ image.at<cv::Vec3b>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_16U && channel < 4){ image.at<cv::Vec3w>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_16S && channel < 4){ image.at<cv::Vec3s>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_32S && channel < 4){ image.at<cv::Vec3i>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_32F && channel < 4){ image.at<cv::Vec3f>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_64F && channel < 4){ image.at<cv::Vec3d>(row, col)[channel] = newValue; }
		else{ throw std::runtime_error("Unknown image depth, gdal: 4, image: 3"); }
	}

	// input: 4 channel, output: 4 channel
	else if (gdalChannels == 4 && image.channels() == 4){
		if (image.depth() == CV_8U){ image.at<cv::Vec4b>(row, col)[channel] = newValue; }
		//if (image.depth() == CV_8U){ image.ptr<cv::Vec4b>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_16U){ image.at<cv::Vec4w>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_16S){ image.at<cv::Vec4s>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_32S){ image.at<cv::Vec4i>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_32F){ image.at<cv::Vec4f>(row, col)[channel] = newValue; }
		else if (image.depth() == CV_64F){ image.at<cv::Vec4d>(row, col)[channel] = newValue; }
		else{ throw std::runtime_error("Unknown image depth, gdal: 4, image: 4"); }
	}

	// input: > 4 channels, output: > 4 channels
	else if (gdalChannels > 4 && image.channels() > 4){
		if (image.depth() == CV_8U){
			uchar * data = image.ptr<uchar>(row);
			data[col*image.channels() + channel] = newValue;
			//image.ptr<uchar>(row, col)[channel] = newValue;
		}else if (image.depth() == CV_16U){
			ushort * data = image.ptr<ushort>(row);
			data[col*image.channels() + channel] = newValue;
			//image.ptr<unsigned short>(row, col)[channel] = newValue;
		}else if (image.depth() == CV_16S){
			short * data = image.ptr<short>(row);
			data[col*image.channels() + channel] = newValue;
			//image.ptr<short>(row, col)[channel] = newValue;
		}else if (image.depth() == CV_32S){
			int * data = image.ptr<int>(row);
			data[col*image.channels() + channel] = newValue;
			//image.ptr<int>(row, col)[channel] = newValue;
		}else if (image.depth() == CV_32F){
			float * data = image.ptr<float>(row);
			data[col*image.channels() + channel] = newValue;
			//image.ptr<float>(row, col)[channel] = newValue;
		}else if (image.depth() == CV_64F){
			double * data = image.ptr<double>(row);
			data[col*image.channels() + channel] = newValue;
			//image.ptr<double>(row, col)[channel] = newValue;
		}else{ throw std::runtime_error("Unknown image depth, gdal: N, img: N"); }
	}
	// otherwise, throw an error
	else{
		throw std::runtime_error("error: can't convert types.");
	}
}

void KGDAL2CV::write_ctable_pixel(const double& pixelValue,
	const GDALDataType& gdalType,
	GDALColorTable const* gdalColorTable,
	cv::Mat& image,
	const int& y,
	const int& x,
	const int& c){

	if (gdalColorTable == NULL){
		write_pixel(pixelValue, gdalType, 1, image, y, x, c);
	}

	// if we are Grayscale, then do a straight conversion
	if (gdalColorTable->GetPaletteInterpretation() == GPI_Gray){
		write_pixel(pixelValue, gdalType, 1, image, y, x, c);
	}

	// if we are rgb, then convert here
	else if (gdalColorTable->GetPaletteInterpretation() == GPI_RGB){

		// get the pixel
		short r = gdalColorTable->GetColorEntry((int)pixelValue)->c1;
		short g = gdalColorTable->GetColorEntry((int)pixelValue)->c2;
		short b = gdalColorTable->GetColorEntry((int)pixelValue)->c3;
		short a = gdalColorTable->GetColorEntry((int)pixelValue)->c4;

		write_pixel(r, gdalType, 4, image, y, x, 2);
		write_pixel(g, gdalType, 4, image, y, x, 1);
		write_pixel(b, gdalType, 4, image, y, x, 0);
		if (image.channels() > 3){
			write_pixel(a, gdalType, 4, image, y, x, 1);
		}
	}

	// otherwise, set zeros
	else{
		write_pixel(pixelValue, gdalType, 1, image, y, x, c);
	}
}

/**
* read data
*/
bool KGDAL2CV::readData(cv::Mat img){
	// make sure the image is the proper size
	if (img.size().height != m_height){
		return false;
	}
	if (img.size().width != m_width){
		return false;
	}

	// make sure the raster is alive
	if (m_dataset == NULL || m_driver == NULL){
		return false;
	}

	// set the image to zero
	img = 0;

	// iterate over each raster band
	// note that OpenCV does bgr rather than rgb
	int nChannels = m_dataset->GetRasterCount();

	GDALColorTable* gdalColorTable = NULL;
	if (m_dataset->GetRasterBand(1)->GetColorTable() != NULL){
		gdalColorTable = m_dataset->GetRasterBand(1)->GetColorTable();
	}

	const GDALDataType gdalType = m_dataset->GetRasterBand(1)->GetRasterDataType();
	int nRows, nCols;

	//if (nChannels > img.channels()){
	//	nChannels = img.channels();
	//}

	for (int c = 0; c < img.channels(); c++){

		int realBandIndex = c;
		
		// get the GDAL Band
		GDALRasterBand* band = m_dataset->GetRasterBand(c + 1);

		//if (hasColorTable == false){
		if (GCI_RedBand == band->GetColorInterpretation()) realBandIndex = 2;
		if (GCI_GreenBand == band->GetColorInterpretation()) realBandIndex = 1;
		if (GCI_BlueBand == band->GetColorInterpretation()) realBandIndex = 0;
		//}
		if (hasColorTable && gdalColorTable->GetPaletteInterpretation() == GPI_RGB) c = img.channels() - 1;
		// make sure the image band has the same dimensions as the image
		if (band->GetXSize() != m_width || band->GetYSize() != m_height){ return false; }

		// grab the raster size
		nRows = band->GetYSize();
		nCols = band->GetXSize();

		// create a temporary scanline pointer to store data
		double* scanline = new double[nCols];

		// iterate over each row and column
		for (int y = 0; y<nRows; y++){

			// get the entire row
			band->RasterIO(GF_Read, 0, y, nCols, 1, scanline, nCols, 1, GDT_Float64, 0, 0);

			// set inside the image
			for (int x = 0; x<nCols; x++){

				// set depending on image types
				// given boost, I would use enable_if to speed up.  Avoid for now.
				if (hasColorTable == false){
					write_pixel(scanline[x], gdalType, nChannels, img, y, x, realBandIndex);
				}
				else{
					write_ctable_pixel(scanline[x], gdalType, gdalColorTable, img, y, x, c);
				}
			}
		}
		// delete our temp pointer
		delete[] scanline;
	}

	return true;
}

cv::Mat KGDAL2CV::ImgReadByGDAL(GDALRasterBand* pBand)
{
	m_width = pBand->GetXSize();
	m_height = pBand->GetYSize();

	m_nBand = 1;
	// check if we have a color palette
	int tempType;
	if (pBand->GetColorInterpretation() == GCI_PaletteIndex){

		// remember that we have a color palette
		hasColorTable = true;
		// if the color tables does not exist, then we failed
		if (pBand->GetColorTable() == NULL){
			return cv::Mat();
		}
		// otherwise, get the pixeltype

		// convert the palette interpretation to opencv type
		tempType = gdalPaletteInterpretation2OpenCV(pBand->GetColorTable()->GetPaletteInterpretation(), pBand->GetRasterDataType());

		if (tempType == -1){
			return cv::Mat();
		}
		m_type = tempType;
	}
	// otherwise, we have standard channels
	else{
		// remember that we don't have a color table
		hasColorTable = false;
		// convert the datatype to opencv
		tempType = gdal2opencv(pBand->GetRasterDataType(), m_nBand);
		if (tempType == -1){
			return cv::Mat();
		}
		m_type = tempType;
	}

	cv::Mat img(m_height, m_width, m_type, cv::Scalar::all(0.f));

	// iterate over each raster band
	// note that OpenCV does bgr rather than rgb
	GDALColorTable* gdalColorTable = NULL;
	if (pBand->GetColorTable() != NULL){
		gdalColorTable = pBand->GetColorTable();
	}

	const GDALDataType gdalType = pBand->GetRasterDataType();
	int nRows, nCols;

	//if (m_nBand > img.channels()){
	//	m_nBand = img.channels();
	//}

	for (int c = 0; c < img.channels(); c++){
		// grab the raster size
		nRows = pBand->GetYSize();
		nCols = pBand->GetXSize();

		if (hasColorTable && gdalColorTable->GetPaletteInterpretation() == GPI_RGB) c = img.channels() - 1;
		// create a temporary scanline pointer to store data
		double* scanline = new double[nCols];

		// iterate over each row and column
		for (int y = 0; y<nRows; y++){

			// get the entire row
			pBand->RasterIO(GF_Read, 0, y, nCols, 1, scanline, nCols, 1, GDT_Float64, 0, 0);

			// set inside the image
			for (int x = 0; x<nCols; x++){

				// set depending on image types
				// given boost, I would use enable_if to speed up.  Avoid for now.
				if (hasColorTable == false){
					write_pixel(scanline[x], gdalType, m_nBand, img, y, x, c);
				}
				else{
					write_ctable_pixel(scanline[x], gdalType, gdalColorTable, img, y, x, c);
				}
			}
		}
		// delete our temp pointer
		delete[] scanline;
	}

	return img;
}

cv::Mat KGDAL2CV::ImgReadByGDAL(cv::String filename, int xStart, int yStart, int xWidth, int yWidth, bool beReadFourth)
{
	m_filename = filename;
	if (!readHeader()) return cv::Mat();
	
	int tempType = m_type;

	if (xStart < 0 || yStart < 0 || xWidth < 1 || yWidth < 1 || xStart > m_width - 1 || yStart > m_height - 1) return cv::Mat();

	if (xStart + xWidth > m_width)
	{
		std::cout << "The specified width is invalid, Automatic optimization is executed!" << std::endl;
		xWidth = m_width - xStart;
	}

	if (yStart + yWidth > m_height)
	{
		std::cout << "The specified height is invalid, Automatic optimization is executed!" << std::endl;
		yWidth = m_height - yStart;
	}

	if (!beReadFourth && 4 == m_nBand)
	{
		for (unsigned int index = CV_8S; index < CV_USRTYPE1; ++index)
		{
			if (CV_MAKETYPE(index, m_nBand) == m_type)
			{
				std::cout << "We won't read the fourth band unless it's datatype is GDT_Byte!" << std::endl;
				//tempType = -1;
				tempType = tempType - ((3 << CV_CN_SHIFT) - (2 << CV_CN_SHIFT));
				break;
				//tempType = CV_MAKETYPE(index, m_nBand);
			}
		}
	}

	cv::Mat img(yWidth, xWidth, tempType, cv::Scalar::all(0.f));
	// iterate over each raster band
	// note that OpenCV does bgr rather than rgb
	int nChannels = m_dataset->GetRasterCount();

	GDALColorTable* gdalColorTable = NULL;
	if (m_dataset->GetRasterBand(1)->GetColorTable() != NULL){
		gdalColorTable = m_dataset->GetRasterBand(1)->GetColorTable();
	}

	const GDALDataType gdalType = m_dataset->GetRasterBand(1)->GetRasterDataType();

	//if (nChannels > img.channels()){
	//	nChannels = img.channels();
	//}
	for (int c = 0; c < img.channels(); c++){

		int realBandIndex = c;

		// get the GDAL Band
		GDALRasterBand* band = m_dataset->GetRasterBand(c + 1);

		//if (hasColorTable == false){
		if (GCI_RedBand == band->GetColorInterpretation()) realBandIndex = 2;
		if (GCI_GreenBand == band->GetColorInterpretation()) realBandIndex = 1;
		if (GCI_BlueBand == band->GetColorInterpretation()) realBandIndex = 0;
		//}
		if (hasColorTable && gdalColorTable->GetPaletteInterpretation() == GPI_RGB) c = img.channels() - 1;
		// make sure the image band has the same dimensions as the image
		if (band->GetXSize() != m_width || band->GetYSize() != m_height){ return cv::Mat(); }

		// create a temporary scanline pointer to store data
		double* scanline = new double[xWidth];

		// iterate over each row and column
		for (int y = 0; y<yWidth; y++){

			// get the entire row
			band->RasterIO(GF_Read, xStart, y + yStart, xWidth, 1, scanline, xWidth, 1, GDT_Float64, 0, 0);
			// set inside the image
			for (int x = 0; x<xWidth; x++){
				// set depending on image types
				// given boost, I would use enable_if to speed up.  Avoid for now.
				if (hasColorTable == false){
					write_pixel(scanline[x], gdalType, nChannels, img, y, x, realBandIndex);
				}
				else{
					write_ctable_pixel(scanline[x], gdalType, gdalColorTable, img, y, x, c);
				}
			}
		}
		// delete our temp pointer
		delete[] scanline;
	}
	return img;
}

cv::Mat KGDAL2CV::ImgReadByGDAL(GDALRasterBand* pBand, int xStart, int yStart, int xWidth, int yWidth)
{
	m_width = pBand->GetXSize();
	m_height = pBand->GetYSize();

	m_nBand = 1;
	// check if we have a color palette
	int tempType;
	if (pBand->GetColorInterpretation() == GCI_PaletteIndex){

		// remember that we have a color palette
		hasColorTable = true;
		// if the color tables does not exist, then we failed
		if (pBand->GetColorTable() == NULL){
			return cv::Mat();
		}
		// otherwise, get the pixeltype

		// convert the palette interpretation to opencv type
		tempType = gdalPaletteInterpretation2OpenCV(pBand->GetColorTable()->GetPaletteInterpretation(), pBand->GetRasterDataType());

		if (tempType == -1){
			return cv::Mat();
		}
		m_type = tempType;
	}
	// otherwise, we have standard channels
	else{
		// remember that we don't have a color table
		hasColorTable = false;
		// convert the datatype to opencv
		tempType = gdal2opencv(pBand->GetRasterDataType(), m_nBand);
		if (tempType == -1){
			return cv::Mat();
		}
		m_type = tempType;
	}

	if (xStart < 0 || yStart < 0 || xWidth < 1 || yWidth < 1 || xStart > m_width - 1 || yStart > m_height - 1) return cv::Mat();

	if (xStart + xWidth > m_width)
	{
		std::cout << "The specified width is invalid, Automatic optimization is executed!" << std::endl;
		xWidth = m_width - xStart;
	}

	if (yStart + yWidth > m_height)
	{
		std::cout << "The specified height is invalid, Automatic optimization is executed!" << std::endl;
		yWidth = m_height - yStart;
	}

	cv::Mat img(yWidth, xWidth, m_type, cv::Scalar::all(0.f));

	// iterate over each raster band
	// note that OpenCV does bgr rather than rgb
	GDALColorTable* gdalColorTable = NULL;
	if (pBand->GetColorTable() != NULL){
		gdalColorTable = pBand->GetColorTable();
	}

	const GDALDataType gdalType = pBand->GetRasterDataType();

	//if (m_nBand > img.channels()){
	//	m_nBand = img.channels();
	//}

	for (int c = 0; c < img.channels(); c++){
		// grab the raster size

		if (hasColorTable && gdalColorTable->GetPaletteInterpretation() == GPI_RGB) c = img.channels() - 1;
		// create a temporary scanline pointer to store data
		double* scanline = new double[xWidth];

		// iterate over each row and column
		for (int y = 0; y<yWidth; y++){

			// get the entire row
			pBand->RasterIO(GF_Read, xStart, y + yStart, xWidth, 1, scanline, xWidth, 1, GDT_Float64, 0, 0);

			// set inside the image
			for (int x = 0; x<xWidth; x++){
				// set depending on image types
				// given boost, I would use enable_if to speed up.  Avoid for now.
				if (hasColorTable == false){
					write_pixel(scanline[x], gdalType, m_nBand, img, y, x, c);
				}
				else{
					write_ctable_pixel(scanline[x], gdalType, gdalColorTable, img, y, x, c);
				}
			}
		}
		// delete our temp pointer
		delete[] scanline;
	}

	return img;
}

cv::Mat KGDAL2CV::ImgReadByGDAL(cv::String filename, bool beReadFourth)
{
	m_filename = filename;
	if (!readHeader()) return cv::Mat();
	
	int tempType = m_type;

	if (!beReadFourth && 4 == m_nBand)
	{
		for (unsigned int index = CV_8S; index < CV_USRTYPE1; ++index)
		{
			if (CV_MAKETYPE(index, m_nBand) == m_type)
			{
				std::cout << "We won't read the fourth band unless it's datatype is GDT_Byte!" << std::endl;
				//tempType = -1;
				tempType = tempType - ((3 << CV_CN_SHIFT) - (2 << CV_CN_SHIFT));
				break;
				//tempType = CV_MAKETYPE(index, m_nBand);
			}
		}
	}

	cv::Mat img(m_height, m_width, tempType, cv::Scalar::all(0.f));
	//if (-1 == tempType) tempType = m_type - ((3 << CV_CN_SHIFT) - (2 << CV_CN_SHIFT));
	//img.create(m_height, m_width, tempType);
	if (!readData(img)) img.release(); 
	//cv::cvtColor(img, img, CV_RGB2BGR);
	return img;
}

void KGDAL2CV::Close()
{
	if (nullptr != m_dataset) GDALClose(static_cast<GDALDatasetH>(m_dataset));
	m_dataset = nullptr;
	m_driver = nullptr;
}

KGDAL2CV::KGDAL2CV() : m_dataset(nullptr), m_filename(""), m_driver(nullptr), hasColorTable(false), m_width(0), m_height(0), m_type(-1), m_nBand(0)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
}

KGDAL2CV::~KGDAL2CV()
{
	Close();
}
