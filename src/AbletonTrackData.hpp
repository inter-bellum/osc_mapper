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
    
    ofParameterGroup thresh;
    ofParameter<T> lo;
    ofParameter<T> hi;
    
    ofEventListener* gain_listener;
    
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
        gui.setup(params);
    };
    
    ~AbletonTrackData<T>() noexcept {};
    
    void update();
    void draw();
    bool compare_name(std::string name);
    T get_level();
    void set_level(T new_level);
    void set_gain(float &new_gain);
    void set_filter_speed(float speed);
    void set_name(std::string name);
    void set_position(int index, int would_be_width);
    ofParameter<T>* get_gain();
    T get_filtered();
};

template <typename T>
void AbletonTrackData<T>::set_gain(float &new_gain){
    this->gain = new_gain;
}

template <typename T>
void AbletonTrackData<T>::set_level(T new_level){
    this->lpf.set(new_level);
}

template <typename T>
void AbletonTrackData<T>::update(){
    this->lpf.update();
    this->level = (lpf.get() * 76 - 70.);
    
    output_value = level >= lo.get() ? this->get_level() : 0.;
    
    this->history[history_index] = output_value;
    history_index = (history_index + 1) % history_count;
}

template <typename T>
void AbletonTrackData<T>::draw(){
    this->gui.draw();
    int offset = history_index;
    int rect_width = (width / (float)history_count);
    ofSetColor(0);
    
    for (int i = 0; i < history_count; i++){
        ofDrawRectangle(x + (i * rect_width), y+ height, rect_width, -(history[(i + offset) % history_count] * 10));
    }
}

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

template <typename T>
ofParameter<T>* AbletonTrackData<T>::get_gain(){
    return &gain;
}

template <typename T>
T AbletonTrackData<T>::get_level(){
    if (&lpf != NULL){
        return lpf.get() * gain;
    } else {
        return -1;
    }
}


template <typename T>
bool AbletonTrackData<T>::compare_name(std::string other_name){
    if (name.size() == other_name.size()){
        return memcmp(&name, &other_name, name.size()) == 0;
    }
    
    return false;
}

template <typename T>
void AbletonTrackData<T>::set_name(std::string name){
    this->name = name;
    this->level.setName(name);
    this->params.setName(name);
}

template <typename T>
void AbletonTrackData<T>::set_filter_speed(float filter_speed){
    this->lpf.set_filter_speed(filter_speed);
}


#endif /* AbletonTrackData_hpp */
