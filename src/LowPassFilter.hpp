//
//  LowPassFilter.hpp
//  interbellum_osc_mapper
//
//  Created by denotebook on 09/11/2021.
//

#ifndef LowPassFilter_hpp
#define LowPassFilter_hpp

#include <stdio.h>
#include <iostream>

template <typename T>
class LowPassFilter {
private:
    float filter_speed;
    T value;
    T filtered_value;
    
public:
    LowPassFilter(){
        this->filter_speed = 0.9;
        this->value = 0;
        this->filtered_value = 0;
    };
    
    LowPassFilter(float filter_speed){
        this->filter_speed = filter_speed;
        this->value = 0;
        this->filtered_value = 0;
    };
    
    ~LowPassFilter(){};
    
    void update();
    T get();
    void set(T new_value);
    void set_filter_speed(float filter_speed);
};

template <typename T>
void LowPassFilter<T>::update(){
    this->filtered_value = (this->filtered_value * filter_speed) + (this->value * (1. - filter_speed));
}

template <typename T>
T LowPassFilter<T>::get(){
    return this->filtered_value;
}

template <typename T>
void LowPassFilter<T>::set(T new_value){
    this->value = new_value;
}

template <typename T>
void LowPassFilter<T>::set_filter_speed(float filter_speed){
    this->filter_speed = filter_speed;
}

#endif /* LowPassFilter_hpp */
