#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
}

//--------------------------------------------------------------
void ofApp::update(){
    ponk.newFrame();
    
    
    // draw polyline: width -1 to 1 and height -1 to 1
    poly.clear();
    poly.arc(glm::vec3(0,0,0), 0.5, 0.5, 0, 360, true, (ofGetFrameNum()/20)%15+3);
    
    poly2.clear();
    poly2.arc(glm::vec3(0,0,0), 0.6, 0.6, 0, 360, true, (ofGetFrameNum()/20)%5+3);
    
    hotspot.clear();
    hotspot.addVertex(glm::vec3(ofRandom(-.5, 0.5),ofRandom(-.5, 0.5), 0));
    
    ponk.add(poly, ofColor::orange);
    ponk.add(poly2, ofColor::cyan, {{"MAXSPEED",2.f}});
    ponk.add(hotspot, ofColor::blueViolet, {{"SNGLPTIN",30.f}});

    ponk.sendFrame();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofPushMatrix();
    ofTranslate(ofGetWindowWidth()/2, ofGetWindowHeight()/2);
    ofScale(ofGetWindowWidth()/2, ofGetWindowHeight()/2);
    ofSetColor(ofColor::orange);
    poly.draw();
    ofSetColor(ofColor::cyan);
    poly2.draw();
    ofSetColor(ofColor::blueViolet);
    hotspot.draw();
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
