#include "ofApp.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "AbletonTrackData.hpp"

std::vector<AbletonTrackData<float>> ATD;

ofxOscReceiver osc_rec;


//--------------------------------------------------------------
void ofApp::setup(){
    osc_rec.setup(6969);
}

//--------------------------------------------------------------
void ofApp::update(){
    if (osc_rec.hasWaitingMessages()){
        ofxOscMessage msg;
        while(osc_rec.getNextMessage(msg)){
            std::string data = msg.getArgAsString(0);
            ofJson parsed_string = nlohmann::json::parse(data);
            if (ATD.size() != parsed_string.size()){
                auto track_count = parsed_string.size();
                auto width_per_track = ofGetWidth() / track_count;
                ATD.resize(track_count);
                for (auto i = 0 ; i < track_count; i++){
                    AbletonTrackData<float>* current = &ATD[i];
                    ofEventListener gain_listener(current->get_gain()->newListener([current](float& value){current->set_gain(value);}));
                    ATD.at(i).add_listener(&gain_listener);
                    ATD.at(i).set_position(i * width_per_track, width_per_track);
                }
            }
            
            for (auto i = 0; i < parsed_string.size(); i++){
                auto sub_array = parsed_string.at(i);
                uint8_t idx = sub_array.at(0);
                float value = sub_array.at(2);
                ATD.at(idx).set_level(value);
            }
        }
    }
    
    for (int i = 0; i < ATD.size(); i++){
        ATD[i].update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255, 255, 255);
    for (int i = 0; i < ATD.size(); i++){
        ATD[i].draw();
    }
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

//--------------------------------------------------------------
//void ofApp::set_gain(void* adt, float value){
//    (AbletonTrackData<float> adt)->set_gain(value);
//}

