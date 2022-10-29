//
//  ofxPonk.cpp
//  example
//
//  Created by Jonas Fehr on 26/10/2022.
//

#include "ofxPonk.h"

ofxPonk::ofxPonk():socket(INADDR_ANY,0){
    setup(); // set Defaults
    fullData.reserve(65536);
}

void ofxPonk::setup(std::string name, int uniqueID, std::string destinationIP, uint8_t dataFormat){
    this->senderName = name;
    this->uniqueID = uniqueID;
    this->destinationIP = ipStrToInt(destinationIP);
    assert(dataFormat>=PONK_DATA_FORMAT_XYRGB_U16 && dataFormat<=PONK_DATA_FORMAT_XY_F32_RGB_U8);
    this->dataFormat = dataFormat;
}

void ofxPonk::newFrame(){
    fullData.clear();
    pathNumber = 0;
}

void ofxPonk::add(ofPolyline poly, ofColor color, std::vector<std::pair<std::string, float>> metaData){
    
    fullData.push_back(dataFormat); // Write Format Data
    
    // Meta Data
    fullData.push_back(1+metaData.size()); // Write meta data count
    pushMetaData(fullData,"PATHNUMB",static_cast<float>(pathNumber++));
    for(auto & m : metaData){
        pushMetaData(fullData,m.first,m.second);
    }
    
    push16bits(fullData,poly.getVertices().size());
    
    for(auto & p : poly.getVertices()){
        switch(dataFormat){
            case PONK_DATA_FORMAT_XY_F32_RGB_U8:
                pushPoint_XY_F32_RGB_U8(fullData, p, color);
                break;
            case PONK_DATA_FORMAT_XYRGB_U16:
                pushPoint_XYRGB_U16(fullData, p, color);
                break;
            default:
                break;
        }
    }
    
}

void ofxPonk::sendFrame(){
    // Compute necessary chunk count
    size_t chunksCount64 = 1 + fullData.size() / (PONK_MAX_DATA_BYTES_PER_PACKET-sizeof(GeomUdpHeader));
    if (chunksCount64 > 255) {
        throw std::runtime_error("Protocol doesn't accept sending "
                                 "a packet that would be splitted "
                                 "in more than 255 chunks");
    }
    
    // Compute data CRC
    unsigned int dataCrc = 0;
    for (auto v: fullData) {
        dataCrc += v;
    }
    
    // Send all chunks to the desired IP address
    size_t written = 0;
    unsigned char chunkNumber = 0;
    unsigned char chunksCount = static_cast<unsigned char>(chunksCount64);
    while (written < fullData.size()) {
        // Write packet header - 8 bytes
        GeomUdpHeader header;
        strncpy(header.headerString,PONK_HEADER_STRING,sizeof(header.headerString));
        header.protocolVersion = 0;
        header.senderIdentifier = uniqueID;
        strncpy(header.senderName,senderName.c_str(),sizeof(header.senderName));
        header.frameNumber = frameNumber;
        header.chunkCount = chunksCount;
        header.chunkNumber = chunkNumber;
        header.dataCrc = dataCrc;
        
        // Prepare buffer
        std::vector<unsigned char> packet;
        size_t dataBytesForThisChunk = std::min<size_t>(fullData.size()-written, PONK_MAX_DATA_BYTES_PER_PACKET);
        packet.resize(sizeof(GeomUdpHeader) + dataBytesForThisChunk);
        // Write header
        memcpy(&packet[0],&header,sizeof(GeomUdpHeader));
        // Write data
        memcpy(&packet[sizeof(GeomUdpHeader)],&fullData[written],dataBytesForThisChunk);
        written += dataBytesForThisChunk;
        
        // Now send chunk packet
        GenericAddr destAddr;
        destAddr.family = AF_INET;
        destAddr.ip = destinationIP;
        destAddr.port = PONK_PORT;
        socket.sendTo(destAddr, &packet.front(), static_cast<unsigned int>(packet.size()));
        
        chunkNumber++;
    }
    frameNumber++;
}

void ofxPonk::pushPoint_XY_F32_RGB_U8(std::vector<unsigned char>& fullData, const glm::vec3& pointPosition, const ofColor& pointColor) {
    // Push X - LSB first
    push32bits(fullData,static_cast<float>(pointPosition.x));
    // Push Y - LSB first
    push32bits(fullData,static_cast<float>(pointPosition.y));
    assert(pointPosition.x>=-1 && pointPosition.x<=1 && pointPosition.y>=-1 && pointPosition.y<=1);
    
    // Push R - LSB first
    push8bits(fullData,pointColor.r);
    // Push G - LSB first
    push8bits(fullData,pointColor.g);
    // Push B - LSB first
    push8bits(fullData,pointColor.b);
}

void ofxPonk::pushPoint_XYRGB_U16(std::vector<unsigned char>& fullData, const glm::vec3& pointPosition, const ofColor& pointColor) {
        if (pointPosition.x < -1 || pointPosition.x > 1 || pointPosition.y < -1 || pointPosition.y > 1) {
            // Clamp position and set color = 0
            unsigned short x16Bits, y16Bits;
            if (pointPosition.x < -1) {
                x16Bits = 0;
            } else if (pointPosition.x > 1) {
                x16Bits = 65535;
            } else {
                x16Bits = static_cast<unsigned short>(((pointPosition.x + 1) / 2) * 65535);
            }
            if (pointPosition.y < -1) {
                y16Bits = 0;
            } else if (pointPosition.y > 1) {
                y16Bits = 65535;
            } else {
                y16Bits = static_cast<unsigned short>(((pointPosition.y + 1) / 2) * 65535);
            }
    
            // Push X - LSB first
            push16bits(fullData, x16Bits);
            // Push Y - LSB first
            push16bits(fullData, y16Bits);
            // Push R - LSB first
            push16bits(fullData, 0);
            // Push G - LSB first
            push16bits(fullData, 0);
            // Push B - LSB first
            push16bits(fullData, 0);
        } else {
            const auto x16Bits = static_cast<unsigned short>(((pointPosition.x + 1) / 2) * 65535);
            const auto y16Bits = static_cast<unsigned short>(((pointPosition.y + 1) / 2) * 65535);
    
            ofShortColor shortColor(pointColor);
    
            // Push X - LSB first
            push16bits(fullData, x16Bits);
            // Push Y - LSB first
            push16bits(fullData, y16Bits);
            // Push R - LSB first
            push16bits(fullData, shortColor.r);
            // Push G - LSB first
            push16bits(fullData, shortColor.g);
            // Push B - LSB first
            push16bits(fullData, shortColor.b);
        }
}

void ofxPonk::push8bits(std::vector<unsigned char>& fullData, unsigned char value) {
    fullData.push_back(value);
}

void ofxPonk::push16bits(std::vector<unsigned char>& fullData, unsigned short value) {
    fullData.push_back(static_cast<unsigned char>((value>>0) & 0xFF));
    fullData.push_back(static_cast<unsigned char>((value>>8) & 0xFF));
}

void ofxPonk::push32bits(std::vector<unsigned char>& fullData, int value) {
    fullData.push_back(static_cast<unsigned char>((value>>0) & 0xFF));
    fullData.push_back(static_cast<unsigned char>((value>>8) & 0xFF));
    fullData.push_back(static_cast<unsigned char>((value>>16) & 0xFF));
    fullData.push_back(static_cast<unsigned char>((value>>24) & 0xFF));
}

void ofxPonk::push32bits(std::vector<unsigned char>& fullData, float value) {
    push32bits(fullData,*reinterpret_cast<int*>(&value));
}

void ofxPonk::pushMetaData(std::vector<unsigned char>& fullData, const char (&eightCC)[9],float value) {
    for (int i=0; i<8; i++) {
        fullData.push_back(eightCC[i]);
    }
    push32bits(fullData,*(int*)&value);
}

void ofxPonk::pushMetaData(std::vector<unsigned char>& fullData, std::string eightCC,float value) {
    for (int i=0; i<8; i++) {
        fullData.push_back(eightCC[i]);
    }
    push32bits(fullData,*(int*)&value);
}

int ofxPonk::ipStrToInt(std::string ip) {
    if(ip == "localhost"){
        return 0x7F000001;
    }
    std::stringstream s(ip);
    int a,b,c,d;
    char ch;
    s >> a >> ch >> b >> ch >> c >> ch >> d;
    
  return    ((a << 24) & 0xFF000000) +
            ((b << 16) & 0x00FF0000) +
            ((c >> 8) & 0x0000FF00) +
            (d & 0x000000FF);
}
