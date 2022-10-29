#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ponk_1.setup("stream_1", 0);
    ponk_2.setup("stream_2", 1, "localhost", PONK_DATA_FORMAT_XYRGB_U16);
}

//--------------------------------------------------------------
void ofApp::update(){
    ponk_1.newFrame();
    ponk_2.newFrame();

    
    // draw polyline: width -1 to 1 and height -1 to 1
    poly_1.clear();
    poly_1.arc(glm::vec3(0,0,0), 0.5, 0.5, 0, 360, true, (ofGetFrameNum()/20)%15+3);
    
    poly_2.clear();
    poly_2.arc(glm::vec3(0,0,0), 0.6, 0.6, 0, 360, true, (ofGetFrameNum()/20)%5+3);
    
    poly_3.clear();
    poly_3.addVertex(glm::vec3(ofRandom(-.5, 0.5),ofRandom(-.5, 0.5), 0));
    
    ponk_1.add(poly_1, ofColor::orange);
    ponk_1.add(poly_2, ofColor::cyan, {{"MAXSPEED",2.f}});
    ponk_2.add(poly_3, ofColor::blueViolet, {{"SNGLPTIN",30.f}});

    ponk_1.sendFrame();
    ponk_2.sendFrame();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofPushMatrix();
    ofTranslate(ofGetWindowWidth()/2, ofGetWindowHeight()/2);
    ofScale(ofGetWindowWidth()/2, ofGetWindowHeight()/2);
    ofSetColor(ofColor::orange);
    poly_1.draw();
    ofSetColor(ofColor::cyan);
    poly_2.draw();
    ofSetColor(ofColor::blueViolet);
    poly_3.draw();
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
