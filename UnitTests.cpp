#define BOOST_TEST_MODULE mytests
#include <boost/test/included/unit_test.hpp>

#include "Texture.h"
#include "Utils.h"
#include "StratifiedSampler.h"
#include "SimpleRandomSampler.h"

namespace {
	void printImagePPM(void* data, int width, int height, int nbChannels, ImageTextureDataType dataType, ImageTextureLayoutType layoutType, const std::string& outputFileName) {
		std::ofstream os;
		os.open(outputFileName);
		os << "P3" << std::endl;
		os << width << " " << height << std::endl;
		os << "255" << std::endl;
		if (dataType == ImageTextureDataType::UNSIGNED_CHAR) {
			unsigned char* char_data = (unsigned char *)data;
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width * nbChannels; j += nbChannels) {
					unsigned char r = char_data[i * width * nbChannels + j];
					unsigned char g = char_data[i * width * nbChannels + j + (1 % nbChannels)];
					unsigned char b = char_data[i * width * nbChannels + j + (2 % nbChannels)];
					os << (int)r << " ";
					os << (int)g << " ";
					os << (int)b << " ";
				}
				os << std::endl;
			}
		}
		else if (dataType == ImageTextureDataType::FLOAT) {
			float* float_data = (float*)data;
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width * nbChannels; j += nbChannels) {
					float r = float_data[i * width * nbChannels + j];
					float g = float_data[i * width * nbChannels + j + (1 % nbChannels)];
					float b = float_data[i * width * nbChannels + j + (2 % nbChannels)];
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

BOOST_AUTO_TEST_CASE(TextureLoadingTest)
{
	/*
	ImageTexture imageTextureHdrRgb("lakeside_2k.hdr", ImageTextureDataType::FLOAT, ImageTextureLayoutType::RGB);
	printImagePPM(imageTextureHdrRgb.getData(), imageTextureHdrRgb.getWidth(), imageTextureHdrRgb.getHeight(), imageTextureHdrRgb.getNbChannels(),
		ImageTextureDataType::FLOAT, ImageTextureLayoutType::RGB, "imageTextureHdrRgb.ppm");
	
	ImageTexture imageTextureHdrLuminance("lakeside_2k.hdr", ImageTextureDataType::FLOAT, ImageTextureLayoutType::LUMINANCE);
	printImagePPM(imageTextureHdrLuminance.getData(), imageTextureHdrLuminance.getWidth(), imageTextureHdrLuminance.getHeight(), imageTextureHdrLuminance.getNbChannels(),
		ImageTextureDataType::FLOAT, ImageTextureLayoutType::RGB, "imageTextureHdrRgb.ppm");

	ImageTexture imageTextureLdrRgb("lakeside_2k.hdr", ImageTextureDataType::UNSIGNED_CHAR, ImageTextureLayoutType::RGB);
	printImagePPM(imageTextureLdrRgb.getData(), imageTextureLdrRgb.getWidth(), imageTextureLdrRgb.getHeight(), imageTextureLdrRgb.getNbChannels(),
		ImageTextureDataType::FLOAT, ImageTextureLayoutType::RGB, "imageTextureHdrRgb.ppm");

	ImageTexture imageTextureLdrLuminance("lakeside_2k.hdr", ImageTextureDataType::UNSIGNED_CHAR, ImageTextureLayoutType::LUMINANCE);
	printImagePPM(imageTextureLdrLuminance.getData(), imageTextureLdrLuminance.getWidth(), imageTextureLdrLuminance.getHeight(), imageTextureLdrLuminance.getNbChannels(),
		ImageTextureDataType::FLOAT, ImageTextureLayoutType::RGB, "imageTextureHdrRgb.ppm");
		*/
}

template<class S>
void pixelSamplerTest(std::string filePrefix) {
	int width = 150, height = 100;
	float* data = new float[width * height];
	for (int i = 0; i < width * height; ++i)
		data[i] = 1;
	int nbSamplesX = 90, nbSamplesY = 90, nbDimensions = 1;
	int nb2DSamples = nbSamplesX * nbSamplesY;
	S* sampler = new S(nbSamplesX, nbSamplesY, nbDimensions);
	sampler->startPixel(glm::ivec2(0, 0));
	for (int i = 0; i < nb2DSamples; ++i) {
		for (int d = 0; d < nbDimensions; ++d) {
			glm::vec2 sample = sampler->get2D();
			int x = (int)(sample.x * width);
			int y = (int)(sample.y * height);
			data[y * width + x] = float(d) / nbDimensions;
		}
		bool nextSample = sampler->startNextSample();
		BOOST_CHECK((i == nb2DSamples - 1 && !nextSample) || nextSample);
	}
	printImagePPM(data, width, height, 1, ImageTextureDataType::FLOAT, ImageTextureLayoutType::RGB, filePrefix + "2D.ppm");

	for (int i = 0; i < width * height; ++i)
		data[i] = 1;
	S* samplerClone = (S*)sampler->clone(int(frand() * 1000));
	samplerClone->startPixel(glm::ivec2(0, 0));
	for (int i = 0; i < nb2DSamples; ++i) {
		for (int d = 0; d < nbDimensions; ++d) {
			glm::vec2 sample = samplerClone->get2D();
			int x = (int)(sample.x * width);
			int y = (int)(sample.y * height);
			data[y * width + x] = float(d) / nbDimensions;
		}
		bool nextSample = samplerClone->startNextSample();
		BOOST_CHECK((i == nb2DSamples - 1 && !nextSample) || nextSample);
	}
	printImagePPM(data, width, height, 1, ImageTextureDataType::FLOAT, ImageTextureLayoutType::RGB, filePrefix + "2DClone.ppm");
	delete[] data;
	data = nullptr;
	delete sampler;
	sampler = nullptr;
	delete samplerClone;
	samplerClone = nullptr;

	width = 100, height = 1;
	data = new float[width * height];
	for (int i = 0; i < width * height; ++i)
		data[i] = 1;
	int nb1DSamples = 30;
	nbSamplesX = 30, nbSamplesY = 1, nbDimensions = 4;
	sampler = new S(nbSamplesX, nbSamplesY, nbDimensions);
	sampler->startPixel(glm::ivec2(0, 0));
	for (int i = 0; i < nb1DSamples; ++i) {
		for (int d = 0; d < nbDimensions; ++d) {
			float sample = sampler->get1D();
			int x = (int)(sample * width);
			int y = 0;
			data[y * width + x] = float(d) / nbDimensions;
		}
		bool nextSample = sampler->startNextSample();
		BOOST_CHECK((i == nb1DSamples - 1 && !nextSample) || nextSample);
	}
	printImagePPM(data, width, height, 1, ImageTextureDataType::FLOAT, ImageTextureLayoutType::RGB, filePrefix + "1D.ppm");
	delete[] data;
	data = nullptr;
	delete sampler;
	sampler = nullptr;

	width = 150, height = 100;
	data = new float[width * height];
	for (int i = 0; i < width * height; ++i)
		data[i] = 1;
	int arraySize = 3600;
	nbSamplesX = 2, nbSamplesY = 2, nbDimensions = 0;
	nb2DSamples = nbSamplesX * nbSamplesY;
	sampler = new S(nbSamplesX, nbSamplesY, nbDimensions);
	sampler->request2DArray(arraySize);
	sampler->startPixel(glm::ivec2(0, 0));
	const glm::vec2* samples = sampler->get2DArray(arraySize);
	for (int s = 0; s < nb2DSamples; ++s) {
		for (int i = 0; i < arraySize; ++i) {
			glm::vec2 sample = samples[s * arraySize + i];
			int x = (int)(sample.x * width);
			int y = (int)(sample.y * height);
			data[y * width + x] = float(s) / nb2DSamples;
		}
		bool nextSample = sampler->startNextSample();
		BOOST_CHECK((s == nb2DSamples - 1 && !nextSample) || nextSample);
	}
	printImagePPM(data, width, height, 1, ImageTextureDataType::FLOAT, ImageTextureLayoutType::RGB, filePrefix + "Array2D.ppm");
	delete[] data;
	data = nullptr;
	delete sampler;
	sampler = nullptr;

	width = 100, height = 1;
	data = new float[width * height];
	for (int i = 0; i < width * height; ++i)
		data[i] = 1;
	arraySize = 30;
	nbSamplesX = 3, nbSamplesY = 1, nbDimensions = 0;
	nb1DSamples = nbSamplesX;
	sampler = new S(nbSamplesX, nbSamplesY, nbDimensions);
	sampler->request1DArray(arraySize);
	sampler->startPixel(glm::ivec2(0, 0));
	const float* floatSamples = sampler->get1DArray(arraySize);
	for (int s = 0; s < nb1DSamples; ++s) {
		for (int i = 0; i < arraySize; ++i) {
			float sample = floatSamples[s * arraySize + i];
			int x = (int)(sample * width);
			int y = 0;
			data[y * width + x] = float(s) / nb1DSamples;
		}
		bool nextSample = sampler->startNextSample();
		BOOST_CHECK((s == nb1DSamples - 1 && !nextSample) || nextSample);
	}
	printImagePPM(data, width, height, 1, ImageTextureDataType::FLOAT, ImageTextureLayoutType::RGB, filePrefix + "Array1D.ppm");
	delete[] data;
	data = nullptr;
	delete sampler;
	sampler = nullptr;
}

BOOST_AUTO_TEST_CASE(StratifiedSamplingTest)
{
	pixelSamplerTest<StratifiedSampler>("StratifiedSampling");
	pixelSamplerTest<SimpleRandomSampler>("SimpleRandomSampler");
}