//
//  LowPassFilter.cpp
//  interbellum_osc_mapper
//
//  Created by Loden on 09/11/2021.
//

#include "LowPassFilter.hpp"



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
