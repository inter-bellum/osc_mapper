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
#include <tuple>
#include "ofxGui.h"
#include "LowPassFilter.hpp"
#include "ofApp.h"


template <typename T>
class AbletonTrackData {
private:
    LowPassFilter<T> lpf;
    std::string name = "dummy";
    ofxGuiGroup gui;
    int min_width = 100;
    int x = 0, y = 0, width = 0, height = 0;
    
    ofParameterGroup params;
    ofParameter<T> level;
    ofParameter<T> gain;
    ofParameter<float> lpf_speed;
    ofParameter<std::string> address_param;
    bool bool_send_osc = false;
    
    ofParameterGroup thresh;
    ofParameter<T> lo;
    ofParameter<T> hi;
    
    static const int history_count = 50;
    T history[history_count];
    int history_index = 0;
    
    T output_value = 0;
    
    uint8_t index;
    
    bool init_completed = false;
    
public:
    AbletonTrackData<T>() : AbletonTrackData(0.99, "dummy"){
    };
    
    AbletonTrackData<T>(float filter_speed, std::string name) : lpf(filter_speed){
        this->name = name;
    };
    
    ~AbletonTrackData<T>() noexcept {};
    
    void update();
    void draw();
    void setup(int index);
    void setup_after_json_load();
    void de_init();
    bool compare_name(std::string name);
    T get_level();
    T get_output();
    ofJson to_json();
    void from_json(ofJson &json);
    std::string get_address();
    std::string get_name();
    bool send_osc();
    void set_level(T new_level);
    void set_gain(float &new_gain);
    void set_addr_param(std::string &addr);
    void set_filter_speed(float &speed);
    void set_name(std::string name);
    void set_position(int index, int would_be_width);
    ofParameter<T>* get_gain();
    ofParameter<std::string>* get_addr_param();
    ofParameter<float>* get_lpf_speed();
    std::tuple<T, T> get_threshold();
    T get_filtered();
    bool init_complete();
};


//SETUP
template <typename T>
void AbletonTrackData<T>::setup(int index){
    if (!this->init_completed){
        this->index = index;
        params.setName(name);
        thresh.setName("threshold");
        params.add(level.set(name, 0., -70., 6.));
        params.add(gain.set("gain", 1., 0., 10.));
        params.add(lpf_speed.set("lpf speed", 0.99, 0., 1.));
        
        thresh.add(lo.set("lo", -70., -70., 6.));
        thresh.add(hi.set("hi", 6., -70., 6.));
        
        params.add(thresh);
        params.add(address_param.set("/none"));
        gui.setup(params);
        
        gain.addListener(this, &AbletonTrackData<T>::set_gain);
        address_param.addListener(this, &AbletonTrackData<T>::set_addr_param);
        lpf_speed.addListener(this, &AbletonTrackData<T>::set_filter_speed);
        
        this->init_completed = true;
    }
}

//SETUP
template <typename T>
void AbletonTrackData<T>::setup_after_json_load(){
    params.setName(name);
    thresh.setName("threshold");
    params.add(level.set(name, 0., -70., 6.));
    params.add(gain.set("gain", gain.get(), 0., 10.));
    params.add(lpf_speed.set("lpf speed", lpf_speed.get(), 0., 1.));
    
    thresh.add(lo.set("lo", lo.get(), -70., 6.));
    thresh.add(hi.set("hi", hi.get(), -70., 6.));
    
    params.add(thresh);
    params.add(address_param.set(address_param.get()));
    gui.setup(params);
    this->init_completed = true;
}

template <typename T>
void AbletonTrackData<T>::de_init(){
    gui.clear();
    params.clear();
    thresh.clear();
    this->init_completed = false;
}

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
    ofDrawBitmapString("Out: "+ ofToString(output_value, 3), this->x + 10,  + this->y + this->height - 80);
    
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

//TO_JSON
template <typename T>
ofJson AbletonTrackData<T>::to_json(){
    ofJson this_object;
    this_object["id"] = index;
    this_object["name"] = this->get_name();
    this_object["address"] = this->get_address();
    this_object["gain"] = this->gain.get();
    this_object["lpf_speed"] = this->get_lpf_speed()->get();
    T lo, hi;
    std::tie(lo, hi) = this->get_threshold();
    this_object["threshold"]["lo"] = lo;
    this_object["threshold"]["hi"] = hi;
    
    return this_object;
}


//FROM_JSON
template <typename T>
void AbletonTrackData<T>::from_json(ofJson &json){
    gui.clear();
    params.clear();
    thresh.clear();
    
    this->index = json["id"].get<int>();
    this->name = json["name"].get<std::string>();
    address_param.set(json["address"].get<std::string>());
    gain.set(json["gain"].get<T>());
    lpf_speed.set(json["lpf_speed"].get<float>());
    lo.set(json["threshold"]["lo"].get<T>());
    hi.set(json["threshold"]["hi"].get<T>());
    
    this->setup_after_json_load();
}


//GET_GAIN
template <typename T>
ofParameter<T>* AbletonTrackData<T>::get_gain(){
    return &gain;
}

//GET ADDR_PARAM
template <typename T>
ofParameter<std::string>* AbletonTrackData<T>::get_addr_param(){
    return &address_param;
}

//GET LPF FILTER SPEED
template <typename T>
ofParameter<float>* AbletonTrackData<T>::get_lpf_speed(){
    return &lpf_speed;
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

//GET_THRESHOLD
template <typename T>
std::tuple<T, T> AbletonTrackData<T>::get_threshold(){
    return std::make_tuple(lo.get(), hi.get());
}


//GET_ADDRESS
template <typename T>
std::string AbletonTrackData<T>::get_address(){
    return address_param.get();
}

//GET_NAME
template <typename T>
std::string AbletonTrackData<T>::get_name(){
    return this->name;
}


//SEND_OSC
template <typename T>
bool AbletonTrackData<T>::send_osc(){
    if (!bool_send_osc){
        std::string addr = address_param.get();
        bool_send_osc = addr.compare("/none") != 0 && addr.compare("") != 0;
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



//SET_NAME
template <typename T>
void AbletonTrackData<T>::set_name(std::string name){
    this->name = name;
    this->level.setName(name);
    this->params.setName(name);
}

//SET_ADDR
template <typename T>
void AbletonTrackData<T>::set_addr_param(std::string &addr){
    if (addr != address_param.get()){
        bool_send_osc = false;
    }
}



//SET_FILTER_SPEED
template <typename T>
void AbletonTrackData<T>::set_filter_speed(float &filter_speed){
    this->lpf.set_filter_speed(filter_speed);
}


template <typename T>
bool AbletonTrackData<T>::init_complete(){
    return init_completed;
}

#endif /* AbletonTrackData_hpp */
