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
    void newFrame();
    void add(ofPolyline poly, ofColor color = ofColor::white, std::vector<std::pair<std::string, float>> metaData = {});
    void sendFrame();
    
private:
    void push8bits(std::vector<unsigned char>& fullData, unsigned char value);
    void push16bits(std::vector<unsigned char>& fullData, unsigned short value);
    void push32bits(std::vector<unsigned char>& fullData, int value);
    void push32bits(std::vector<unsigned char>& fullData, float value);
    void pushMetaData(std::vector<unsigned char>& fullData, const char (&eightCC)[9],float value);
    void pushMetaData(std::vector<unsigned char>& fullData, std::string,float value);

    DatagramSocket socket;
    unsigned char frameNumber = 0;
    unsigned char pathNumber = 0;
    std::vector<unsigned char> fullData;
};

#endif /* ofxPonk_hpp */
