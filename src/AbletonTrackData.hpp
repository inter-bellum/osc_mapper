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
        gui.add(level.set(name, -70, -70, 60));
        gui.add(gain.set("gain", 1., 0., 10.));
        gui.add(threshold.set("threshold", {-70., -70.}, {-70., -70.}, {60., 60.}));
    };
    
//    AbletonTrackData<T>(const AbletonTrackData<T> & other) noexcept : gainListener(std::move(other.gainListener)){
//        this->index = other.index;
//        this->level = other.level;
//        this->gain = other.gain;
//        this->name = other.name;
//        this->lpf = other.lpf;
//        this->gui = other.gui;
//        this->threshold = other.threshold;
//        memcpy(this->history, other.history, sizeof(T) * history_count);
//        this->history_index = other.history_index;
//    };
    
    ~AbletonTrackData<T>() noexcept {};
    
    void update();
    void draw();
    bool compare_name(std::string name);
    void set_level(T new_level);
    void set_gain(float &new_gain);
    void set_filter_speed(float speed);
    void set_name(std::string name);
    void set_position(int index, int width);
    ofParameter<T>* get_gain();
    void add_listener(ofEventListener* listener);
    T get_filtered();
};

template <typename T>
void AbletonTrackData<T>::set_gain(float &new_gain){
    std::cout << "Test" << std::endl;
    this->gain = new_gain;
}

template <typename T>
void AbletonTrackData<T>::set_level(T new_level){
    this->level = new_level * 76 - 70.;
    this->history[history_index] = new_level;
    history_index = (history_index + 1) % history_count;
}

template <typename T>
void AbletonTrackData<T>::update(){
    this->lpf.update();
}

template <typename T>
void AbletonTrackData<T>::draw(){
    this->gui.draw();
}

template <typename T>
void AbletonTrackData<T>::set_position(int x, int width){
    this->gui.setShape(0, 0, width, ofGetHeight());
    this->gui.setPosition(x, 0);
}

template <typename T>
ofParameter<T>* AbletonTrackData<T>::get_gain(){
    return &gain;
}

template <typename T>
void AbletonTrackData<T>::add_listener(ofEventListener* listener){
    this->gain_listener = listener;
}


#endif /* AbletonTrackData_hpp */
