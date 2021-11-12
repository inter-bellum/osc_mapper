//
//  AbletonTrackData.hpp
//  interbellum_osc_mapper
//
//  Created by denotebook on 09/11/2021.
//

#ifndef AbletonTrackData_hpp
#define AbletonTrackData_hpp

#include <stdio.h>
#include <inttypes.h>
#include <string>
#include "ofxGui.h"
#include "LowPassFilter.hpp"
#include "ofApp.h"


template <typename T>
class AbletonTrackData {
private:
    LowPassFilter<T> lpf;
    std::string name = "dummy";
    ofxPanel gui;
    int min_width = 100;
    int x = 0, y = 0, width = 0, height = 0;
    
    ofParameterGroup params;
    ofParameter<T> level;
    ofParameter<T> gain;
    ofParameter<std::string> address_param;
    bool bool_send_osc = false;
    
    ofParameterGroup thresh;
    ofParameter<T> lo;
    ofParameter<T> hi;
    
    ofEventListener* gain_listener = nullptr;
    
    static const int history_count = 50;
    T history[history_count];
    int history_index = 0;
    
    T output_value = 0;
    
    uint8_t index;
    
public:
    AbletonTrackData<T>() : AbletonTrackData(0, 0.99, "dummy"){
    };
    
    AbletonTrackData<T>(uint8_t index, float filter_speed, std::string name) : lpf(filter_speed){
        this->index = index;
        params.setName(name);
        thresh.setName("threshold");
        params.add(level.set(name, 0., -70., 6.));
        params.add(gain.set("gain", 1., 0., 10.));
        
        thresh.add(lo.set("lo", -70., -70., 6.));
        thresh.add(hi.set("hi", 6., -70., 6.));
        
        params.add(thresh);
        params.add(address_param.set("/none"));
        gui.setup(params);
    };
    
    ~AbletonTrackData<T>() noexcept {};
    
    void update();
    void draw();
    bool compare_name(std::string name);
    T get_level();
    T get_output();
    std::string get_address();
    bool send_osc();
    void set_level(T new_level);
    void set_gain(float &new_gain);
    void set_filter_speed(float speed);
    void set_name(std::string name);
    void set_position(int index, int would_be_width);
    ofParameter<T>* get_gain();
    bool has_gain_listener();
    T get_filtered();
};



//SET_GAIN
template <typename T>
void AbletonTrackData<T>::set_gain(float &new_gain){
    this->gain = new_gain;
}


//SET_LEVEL
template <typename T>
void AbletonTrackData<T>::set_level(T new_level){
    this->lpf.set(new_level);
}



//UPDATE
template <typename T>
void AbletonTrackData<T>::update(){
    this->lpf.update();
    this->level = (lpf.get() * 76 - 70.);
    
    output_value = level >= lo.get() ? this->get_level() : 0.;
    
    this->history[history_index] = output_value;
    history_index = (history_index + 1) % history_count;
}



//GET_OUTPUT
template <typename T>
T AbletonTrackData<T>::get_output(){
    return output_value;
}


//DRAW
template <typename T>
void AbletonTrackData<T>::draw(){
    this->gui.draw();
    int offset = history_index;
    int rect_width = (width / (float)history_count);
    ofSetColor(0);
    ofDrawBitmapString("Out: "+ ofToString(output_value), this->x + 10,  + this->y + this->height - 80);
    
    for (int i = 0; i < history_count; i++){
        ofDrawRectangle(x + (i * rect_width), y+ height, rect_width, -(history[(i + offset) % history_count] * 10));
    }
}


//SET_POSITION
template <typename T>
void AbletonTrackData<T>::set_position(int index, int would_be_width){
    int num_horizontal = min(floor(ofGetWidth() / min_width), floor(ofGetWidth() / would_be_width));
    this->width = max(min_width, would_be_width);
    this->height = gui.getHeight() + 100;
    this->x = (index % num_horizontal) * width;
    this->y =floor(index / num_horizontal) * height;

    this->gui.setShape(0, 0, width, height);
    this->gui.setWidthElements(width);
    this->gui.setPosition(x, y);
    
}


//GET_GAIN
template <typename T>
ofParameter<T>* AbletonTrackData<T>::get_gain(){
    return &gain;
}


//GET_LEVEL
template <typename T>
T AbletonTrackData<T>::get_level(){
    if (&lpf != NULL){
        return lpf.get() * gain;
    } else {
        return -1;
    }
}


//GET_ADDRESS
template <typename T>
std::string AbletonTrackData<T>::get_address(){
    return address_param.get();
}


//SEND_OSC
template <typename T>
bool AbletonTrackData<T>::send_osc(){
    if (!bool_send_osc){
        bool_send_osc = address_param.get().compare("/none") != 0;
    }
    
    return bool_send_osc;
}



//COMPARE_NAME
template <typename T>
bool AbletonTrackData<T>::compare_name(std::string other_name){
    if (name.size() == other_name.size()){
        return memcmp(&name, &other_name, name.size()) == 0;
    }
    
    return false;
}



//HAS_GAIN_LISTENER
template <typename T>
bool AbletonTrackData<T>::has_gain_listener(){
    return this->gain_listener != nullptr;
}



//SET_NAME
template <typename T>
void AbletonTrackData<T>::set_name(std::string name){
    this->name = name;
    this->level.setName(name);
    this->params.setName(name);
}



//SET_FILTER_SPEED
template <typename T>
void AbletonTrackData<T>::set_filter_speed(float filter_speed){
    this->lpf.set_filter_speed(filter_speed);
}


#endif /* AbletonTrackData_hpp */
