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
class AbletonTrackData;

template<typename ParameterType>
class AbletonLevelParam: public ofReadOnlyParameter<ParameterType,AbletonTrackData<ParameterType>>{
    friend class AbletonTrackData<ParameterType>;
};

template <typename T>
class AbletonTrackData {
private:
    LowPassFilter<T> lpf;
    std::string name = "dummy";
    ofxPanel gui;
    int x = 0, width = 0;
    
    ofParameterGroup params;
    ofParameter<T> level;
    ofParameter<T> gain;
    ofParameter<glm::vec2> threshold;
    
    ofEventListener* gain_listener;
    
    static const int history_count = 50;
    T history[history_count];
    int history_index = 0;
    
    uint8_t index;
    
public:
    AbletonTrackData<T>() : AbletonTrackData(0, 0.9, "dummy"){
    };
    
    AbletonTrackData<T>(uint8_t index, float filter_speed, std::string name) : lpf(filter_speed){
        this->index = index;
        params.setName(name);
        params.add(level.set(name, 0., -70., 6.));
        params.add(gain.set("gain", 1., 0., 10.));
        params.add(threshold.set("threshold", {-70., -70.}, {-70., -70.}, {60., 60.}));
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
    void set_position(int index, int width);
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
    this->history[history_index] = this->get_level();
    history_index = (history_index + 1) % history_count;
}

template <typename T>
void AbletonTrackData<T>::draw(){
    this->gui.draw();
    int offset = history_index;
    int y = ofGetHeight();
    int rect_width = (width / (float)history_count);
    ofSetColor(0);
    
    for (int i = 0; i < history_count; i++){
        ofDrawRectangle(x + (i * rect_width), y, rect_width, -(history[(i + offset) % history_count] * 10));
    }
}

template <typename T>
void AbletonTrackData<T>::set_position(int x, int width){
    this->gui.setShape(0, 0, width, ofGetHeight());
    this->gui.setPosition(x, 0);
    this->x = x;
    this->width = width;
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
bool AbletonTrackData<T>::compare_name(std::string name){
    if (name.size() == this->name.size() && memcmp(name, this->name, name.size()) == 0){
        return true;
    }
    
    return false;
}

template <typename T>
void AbletonTrackData<T>::set_name(std::string name){
    this->name = name;
    this->level.setName(name);
}

template <typename T>
void AbletonTrackData<T>::set_filter_speed(float filter_speed){
    this->lpf.set_filter_speed(filter_speed);
}


#endif /* AbletonTrackData_hpp */
