//
// Created by frivas on 31/01/17.
//
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
// OpenCV
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <Common/Sample.h>
#include <DatasetConverters/ClassTypeGeneric.h>
#include "DarknetInferencer.h"
#include <glog/logging.h>



using namespace std;
using namespace cv;
using namespace dnn;


DarknetInferencer::DarknetInferencer(const std::string &netConfig, const std::string &netWeights,const std::string& classNamesFile): netConfig(netConfig),netWeights(netWeights) {
    	this->classNamesFile=classNamesFile;
 	this->netConfig=netConfig;
	this->netWeights=netWeights;

	std::vector<string> classes = {};
        string classesFile = "/home/docker/darknet/data/coco.names";
        // ifstream ifs(this->classNamesFile.c_str());
        ifstream ifs(classesFile.c_str());
	string line;
        while (getline(ifs, line)) classes.push_back(line);
	this->classes=classes;
    	/*Net net = readNetFromDarknet(this->netConfig, this->netWeights);
        net.setPreferableBackend(DNN_BACKEND_OPENCV);
        net.setPreferableTarget(DNN_TARGET_CPU);
	this->net=net;*/ 
}

Sample DarknetInferencer::detectImp(const cv::Mat &image, double confThreshold) {
        //printf("OpenCV: %s", cv::getBuildInformation().c_str());
	
	cout << "Width -> " << image.cols << endl;
	cout << "Height -> " << image.rows << endl;

	float nmsThreshold = 0.4; // Non-maximum suppression threshold

	int inpWidth = (image.cols/32) * 32; // Width of network's input image
	int inpHeight = (image.rows/32) * 32;;

        // Load the network
	Net net = readNetFromDarknet(this->netConfig, this->netWeights);
	net.setPreferableBackend(DNN_BACKEND_OPENCV);
        net.setPreferableTarget(DNN_TARGET_CPU);
        // net.setPreferableTarget(DNN_TARGET_OPENCL);
        vector<String> outNames = net.getUnconnectedOutLayersNames();	
        Mat rgbImage;
        resize(image, rgbImage, Size(inpWidth, inpHeight), 1, 1);

    	Mat blob;
    	blobFromImage(rgbImage, blob, 1.0, cvSize(inpWidth, inpHeight), Scalar(), true, false, CV_8U);
    	//blobFromImage(image, blob, 1.0, cvSize(image.cols, image.rows), Scalar(), true, false, CV_8U);
	net.setInput(blob, "", 0.00392, Scalar());
    	
	// END preprocess
    	vector<Mat> outs;
    	net.forward(outs, outNames);
    	// postprocess

        static vector<int> outLayers = net.getUnconnectedOutLayers();
        static string outLayerType = net.getLayer(outLayers[0])->type;
        vector<int> classIds;
        vector<float> confidences;
	vector<Rect> boxes;
    
    	cout << outLayerType << endl;

        for (size_t i = 0; i < outs.size(); i++)
        {
                float* data = (float*)outs[i].data;
                for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
            	{
                	Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
                	Point classIdPoint;
                	double confidence;
                	minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
                	if (confidence > confThreshold)
                	{
                    		// The boxes should be resized to the original size
				//
				/*int centerX = (int)(data[0] * rgbImage.cols);
                    		int centerY = (int)(data[1] * rgbImage.rows);
                    		int width = (int)(data[2] * rgbImage.cols);
                    		int height = (int)(data[3] * rgbImage.rows);*/
				int centerX = (int)(data[0] * image.cols);
                                int centerY = (int)(data[1] * image.rows);
                                int width = (int)(data[2] * image.cols);
                                int height = (int)(data[3] * image.rows);
				int left = centerX - width / 2;
				int top = centerY - height / 2;

                    		classIds.push_back(classIdPoint.x);
                    		confidences.push_back((float)confidence);
                    		boxes.push_back(Rect(left, top, width, height));
                	}
            	}
        }
        cout << classIds.size() << endl;

        vector<int> indices;
        NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

	Sample sample;
    	RectRegionsPtr regions(new RectRegions());
	for (size_t i = 0; i < indices.size(); i++) {
                int idx = indices[i];
                Rect box = boxes[idx];
                string label = this->classes[classIds[idx]];
		regions->add(box, label, confidences[idx]);
		LOG(INFO) << "Label -> " << label << " Confidence -> " << confidences[idx] << endl;
        }

	//sample.setColorImage(rgbImage);
	//sample.setSampleDims(inpWidth, inpHeight);
	sample.setRectRegions(regions);
	return sample;
}
