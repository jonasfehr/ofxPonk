//
//  ofxPonk.cpp
//  example
//
//  Created by Jonas Fehr on 26/10/2022.
//

#include "ofxPonk.h"

ofxPonk::ofxPonk():socket(INADDR_ANY,0){
    fullData.reserve(65536);
}

void ofxPonk::newFrame(){
    fullData.clear();
    pathNumber = 0;
}

void ofxPonk::add(ofPolyline poly, ofColor color, std::vector<std::pair<std::string, float>> metaData){
    
    // Generate circle data with 1024 points
    fullData.push_back(PONK_DATA_FORMAT_XY_F32_RGB_U8); // Write Format Data

    // Meta Data
    fullData.push_back(1+metaData.size()); // Write meta data count
    pushMetaData(fullData,"PATHNUMB",static_cast<float>(pathNumber++));
    for(auto & m : metaData){
        pushMetaData(fullData,m.first,m.second);
    }

    push16bits(fullData,poly.getVertices().size());

    for(auto & p : poly.getVertices()){
        // Push X - LSB first
        push32bits(fullData,static_cast<float>(p.x));
        // Push Y - LSB first
        push32bits(fullData,static_cast<float>(p.y));
        assert(p.x>=-1 && p.x<=1 && p.y>=-1 && p.y<=1);

        // Push R - LSB first
        push8bits(fullData,color.r);
        // Push G - LSB first
        push8bits(fullData,color.g);
        // Push B - LSB first
        push8bits(fullData,color.b);

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
        header.senderIdentifier = 123123; // Unique ID (so when changing name in sender, the receiver can just rename existing stream)
        strncpy(header.senderName,"Sample Sender",sizeof(header.senderName));
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
        // Unicast on localhost 127.0.0.1
        destAddr.ip = ((127 << 24) + (0 << 16) + (0 << 8) + 1);
        destAddr.port = PONK_PORT;
        socket.sendTo(destAddr, &packet.front(), static_cast<unsigned int>(packet.size()));
        
        chunkNumber++;
    }
    frameNumber++;
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
