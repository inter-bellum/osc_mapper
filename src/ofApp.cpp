#include "ofApp.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "AbletonTrackData.hpp"
#include "OscThread.hpp"

#define OSC_SENDER_PORT 4201

int active_tracks = 0;

std::vector<AbletonTrackData<float>> ATD(100);

ofxOscReceiver osc_rec;
ofxOscSender osc_send;

ofxPanel* IP_panel;
ofParameter<std::string> ip_address;
ofParameter<float> framerate_param;

ofParameter<bool> save_state_button;
ofParameter<bool> restore_state_button;

OscThread osc_sender;

//--------------------------------------------------------------
void ofApp::setup(){
    osc_rec.setup(6969);
    osc_send.setup("localhost", OSC_SENDER_PORT);

    static ofxPanel IP_pan;
    IP_panel = &IP_pan;
    IP_panel->setup("settings");
    IP_panel->add(ip_address.set("localhost"));
    IP_panel->add(framerate_param.set("framerate", 0., 0., 120));
    IP_panel->add(save_state_button.set("save parameters", 0));
    IP_panel->add(restore_state_button.set("restore parameters", 0));
    
    ip_address.addListener(this, &ofApp::ip_addr_changed);
    
    save_state_button.addListener(this, &ofApp::save_state);
    restore_state_button.addListener(this, &ofApp::restore_state);
    
    osc_sender.setup("osc_sender", "localhost", OSC_SENDER_PORT);
    
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
            auto new_track_count = parsed_string.size();
            if (active_tracks != new_track_count){
                for (auto i = 0; i < new_track_count; i++){
                    ATD.at(i).setup(i);
                }
                
                if (active_tracks > new_track_count){
                    for (int i = new_track_count; i < active_tracks; i++){
                        ATD.at(i).de_init();
                    }
                }
                
                active_tracks = new_track_count;
                
                for (auto i = 0 ; i < new_track_count; i++){
                    ATD.at(i).set_position(i, ofGetWidth() / new_track_count);
                }
            }
            
            //set the levels for each track
            for (auto i = 0; i < active_tracks; i++){
                auto sub_array = parsed_string.at(i);
                uint8_t idx = sub_array.at(0);
                std::string name = sub_array.at(1);
                float value = sub_array.at(2);
                
                AbletonTrackData<float>* current = &ATD.at(idx);
                
                if (!current->compare_name(name)){
                    current->set_name(name);
                }
                current->set_level(value);
            }
        }
    }
    bool sent_osc = false;
    //update the lowpass filter
    for (int i = 0; i < active_tracks; i++){
        AbletonTrackData<float>* current = &ATD.at(i);
        current->update();
        
        if (current->send_osc()){
            osc_sender.send_message(current->get_address(), current->get_output());
            sent_osc = true;
        }
    }
    
    if (sent_osc){
        osc_sender.signal();
    }
    
    framerate_param.set(ofGetFrameRate());
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255, 255, 255);
    
    for (int i = 0; i < active_tracks; i++){
        if (ATD.at(i).init_complete()){
            ATD.at(i).draw();
        }
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

//--------------------------------------------------------------
void ofApp::save_state(bool & save){
    if (save){
        ofFileDialogResult res = ofSystemSaveDialog("osc_mapper.json", "Saving osc mapping config");
        
        if (res.bSuccess){
            ofFile file;
            ofJson state;
            state["num_tracks"] = active_tracks;
            state["ip_address"] = ip_address;
            for (int i = 0; i < active_tracks; i++){
                ofJson this_object;
                AbletonTrackData<float>* current = &ATD.at(i);
                
                this_object = current->to_json();
                
                state["data"].push_back(this_object);
            }
            
            std::cout << "Saving json to " << res.getPath() << std::endl;
            file.open(res.getPath(), ofFile::ReadWrite, false);
            
            ofBuffer file_buffer;
            file_buffer.set(state.dump());
            
            file.create();
            
            file.writeFromBuffer(file_buffer);
            
            save_state_button.set(false);
        }
    }
}


//--------------------------------------------------------------
void ofApp::restore_state(bool & restore){
    ofFileDialogResult res = ofSystemLoadDialog("Select json parameter file");
    
    if (res.bSuccess){
        ofFile f;
        if (f.open(res.getPath()) && f.peek() != std::ifstream::traits_type::eof()){
            ofJson settings = ofLoadJson(res.getPath());
            
            auto active_tracks_json = settings["num_tracks"].get<int>();
            for (auto i = 0; i < active_tracks_json; i++){
                AbletonTrackData<float>* current = &ATD.at(i);
                
                ofJson data_package = settings["data"].at(i);
                
                current->from_json(data_package);
                
                current->set_position(i, ofGetWidth() / active_tracks_json);
                
            }
            
            active_tracks = active_tracks_json;
            ip_address.set(settings["ip_address"].get<std::string>());
        } else {
            ofSystemAlertDialog("File was empty or could not be parsed");
        }
    }
    
    restore_state_button.set(false);
}


void ofApp::exit(){
    osc_sender.stopThread();
    osc_sender.signal();
}

























