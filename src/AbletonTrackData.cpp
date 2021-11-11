//
//  AbletonTrackData.cpp
//  interbellum_osc_mapper
//
//  Created by Loden on 09/11/2021.
//

#include "AbletonTrackData.hpp"



template <typename T>
bool AbletonTrackData<T>::compare_name(std::string name){
    if (name.size() == this->name.size() && memcmp(name, this->name, name.size())){
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

template <typename T>
T AbletonTrackData<T>::get_filtered(){
    return this->lpf.get();
}
