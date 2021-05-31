#define BOOST_TEST_MODULE mytests
#include <boost/test/included/unit_test.hpp>

#include "Texture.h"

namespace {
	void printImagePPM(const ImageTexture& imageTexture, ImageTextureDataType dataType, ImageTextureLayoutType layoutType, const std::string& outputFileName) {
		std::ofstream os;
		os.open(outputFileName);
		os << "P3" << std::endl;
		int width = imageTexture.getWidth();
		int height = imageTexture.getHeight();
		int nbChannels = imageTexture.getNbChannels();
		os << width << " " << height << std::endl;
		os << "255" << std::endl;
		if (dataType == ImageTextureDataType::UNSIGNED_CHAR) {
			unsigned char* data = (unsigned char *)imageTexture.getData();
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width * nbChannels; j += nbChannels) {
					unsigned char r = data[i * width * nbChannels + j];
					unsigned char g = data[i * width * nbChannels + j + (1 % nbChannels)];
					unsigned char b = data[i * width * nbChannels + j + (2 % nbChannels)];
					os << (int)r << " ";
					os << (int)g << " ";
					os << (int)b << " ";
				}
				os << std::endl;
			}
		}
		else if (dataType == ImageTextureDataType::FLOAT) {
			float* data = (float*)imageTexture.getData();
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width * nbChannels; j += nbChannels) {
					float r = data[i * width * nbChannels + j];
					float g = data[i * width * nbChannels + j + (1 % nbChannels)];
					float b = data[i * width * nbChannels + j + (2 % nbChannels)];
					os << int(r * 255) << " ";
					os << int(g * 255) << " ";
					os << int(b * 255) << " ";
				}
				os << std::endl;
			}
		}
		os.flush();
		os.close();
	}
}

BOOST_AUTO_TEST_CASE(myTestCase)
{
	ImageTexture imageTextureHdrRgb("lakeside_2k.hdr", ImageTextureDataType::FLOAT, ImageTextureLayoutType::RGB);
	printImagePPM(imageTextureHdrRgb, ImageTextureDataType::FLOAT, ImageTextureLayoutType::RGB, "imageTextureHdrRgb.ppm");
	
	ImageTexture imageTextureHdrLuminance("lakeside_2k.hdr", ImageTextureDataType::FLOAT, ImageTextureLayoutType::LUMINANCE);
	printImagePPM(imageTextureHdrLuminance, ImageTextureDataType::FLOAT, ImageTextureLayoutType::LUMINANCE, "imageTextureHdrLuminance.ppm");

	ImageTexture imageTextureLdrRgb("lakeside_2k.hdr", ImageTextureDataType::UNSIGNED_CHAR, ImageTextureLayoutType::RGB);
	printImagePPM(imageTextureLdrRgb, ImageTextureDataType::UNSIGNED_CHAR, ImageTextureLayoutType::RGB, "imageTextureLdrRgb.ppm");

	ImageTexture imageTextureLdrLuminance("lakeside_2k.hdr", ImageTextureDataType::UNSIGNED_CHAR, ImageTextureLayoutType::LUMINANCE);
	printImagePPM(imageTextureLdrLuminance, ImageTextureDataType::UNSIGNED_CHAR, ImageTextureLayoutType::LUMINANCE, "imageTextureLdrLuminance.ppm");
}