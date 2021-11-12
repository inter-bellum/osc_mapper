#include "ofApp.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "AbletonTrackData.hpp"

#define OSC_SENDER_PORT 4201

std::vector<AbletonTrackData<float>> ATD;

ofxOscReceiver osc_rec;
ofxOscSender osc_send;

ofxPanel* IP_panel;
ofParameter<std::string> ip_address;
ofParameter<float> framerate_param;


//--------------------------------------------------------------
void ofApp::setup(){
    osc_rec.setup(6969);
    osc_send.setup("localhost", OSC_SENDER_PORT);

    static ofxPanel IP_pan;
    IP_panel = &IP_pan;
    IP_panel->setup("ip_address");
    IP_panel->add(ip_address.set("localhost"));
    IP_panel->add(framerate_param.set("framerate", 0., 0., 120));
    
    ip_address.addListener(this, &ofApp::ip_addr_changed);
    
    ofSetFrameRate(200);
}

//--------------------------------------------------------------
void ofApp::update(){
    if (osc_rec.hasWaitingMessages()){
        ofxOscMessage msg;
        while(osc_rec.getNextMessage(msg)){
            std::string data = msg.getArgAsString(0);
            ofJson parsed_string = nlohmann::json::parse(data);
            
            //if the number of tracks changed
            if (ATD.size() != parsed_string.size()){
                auto track_count = parsed_string.size();
                ATD.resize(track_count);
                for (auto i = 0 ; i < track_count; i++){
                    AbletonTrackData<float>* current = &ATD[i];
                    if (!current->has_gain_listener()){
                        current->get_gain()->addListener(current, &AbletonTrackData<float>::set_gain);
                    }
                    ATD.at(i).set_position(i, ofGetWidth() / track_count);
                }
            }
            
            //set the levels for each track
            for (auto i = 0; i < parsed_string.size(); i++){
                auto sub_array = parsed_string.at(i);
                uint8_t idx = sub_array.at(0);
                std::string name = sub_array.at(1);
                float value = sub_array.at(2);
                
                if (!ATD.at(idx).compare_name(name)){
                    ATD.at(idx).set_name(name);
                }
                ATD.at(idx).set_level(value);
            }
        }
    }
    
    
    //update the lowpass filter
    for (int i = 0; i < ATD.size(); i++){
        ATD[i].update();
        
        if (ATD[i].send_osc()){
            ofxOscMessage msg;
            msg.setAddress(ATD[i].get_address());
            msg.addFloatArg(ATD[i].get_output());
            osc_send.sendMessage(msg);
        }
    }
    
    framerate_param.set(ofGetFrameRate());
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255, 255, 255);
    
    for (int i = 0; i < ATD.size(); i++){
        ATD[i].draw();
    }
    
    IP_panel->draw();
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
void ofApp::ip_addr_changed(std::string &new_ip){
    osc_send.setup(new_ip, OSC_SENDER_PORT);
}

