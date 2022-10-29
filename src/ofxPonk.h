//
//  ofxPonk.h
//  example
//
//  Created by Jonas Fehr on 26/10/2022.
//

#ifndef ofxPonk_h
#define ofxPonk_h

#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <cassert>
#include "DatagramSocket/DatagramSocket.h"
#include "PonkDefs.h"
#include "ofMain.h"



class ofxPonk {
    
public:
    ofxPonk();
    void setup(std::string name = "oF_stream", int uniqueID = 123124, std::string destinationIP = "127.0.0.1", uint8_t dataFormat = PONK_DATA_FORMAT_XY_F32_RGB_U8);
    void newFrame();
    void add(ofPolyline poly, ofColor color = ofColor::white, std::vector<std::pair<std::string, float>> metaData = {});
    void sendFrame();
    
private:
    void pushPoint_XY_F32_RGB_U8(std::vector<unsigned char>& fullData, const glm::vec3& pointPosition, const ofColor& pointColor);
    void pushPoint_XYRGB_U16(std::vector<unsigned char>& fullData, const glm::vec3& pointPosition, const ofColor& pointColor);
    
    void push8bits(std::vector<unsigned char>& fullData, unsigned char value);
    void push16bits(std::vector<unsigned char>& fullData, unsigned short value);
    void push32bits(std::vector<unsigned char>& fullData, int value);
    void push32bits(std::vector<unsigned char>& fullData, float value);
    void pushMetaData(std::vector<unsigned char>& fullData, const char (&eightCC)[9],float value);
    void pushMetaData(std::vector<unsigned char>& fullData, std::string,float value);
    
    int ipStrToInt(std::string ip);

    DatagramSocket socket;
    unsigned char frameNumber = 0;
    unsigned char pathNumber = 0;
    std::vector<unsigned char> fullData;
    
    std::string senderName;
    int uniqueID;
    int destinationIP;
    uint8_t dataFormat;
};

#endif /* ofxPonk_hpp */
