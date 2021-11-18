//
//  OscThread.cpp
//  interbellum_osc_mapper
//
//  Created by denotebook on 18/11/2021.
//

#include "OscThread.hpp"
#include <stdio.h>


void OscThread::setup(string name, string remote_address, int port){
    setThreadName(name);
    this->remote_address = remote_address;
    this->port = port;
    osc_send.setup(remote_address, port);
    startThread();
}

void OscThread::threadedFunction(){
    while(isThreadRunning()){
        unique_lock<std::mutex> mtx(mutex);
        
        if (osc_message_queue.empty()){
            wait_for_unlock.wait(mtx);
            if (!isThreadRunning()){
                return;
            }
        }
        
        string addr;
        float value;
        
        tie(addr, value) = osc_message_queue.front();
        osc_message_queue.pop();
        mtx.unlock();
        
        ofxOscMessage msg;
        msg.setAddress(addr);
        msg.addFloatArg(value);
        osc_send.sendMessage(msg);
    }
}

void OscThread::set_address(string address){
    lock();
    this->remote_address = address;
    unlock();
}

void OscThread::set_port(int port){
    lock();
    this->port = port;
    unlock();
}

void OscThread::send_message(string addr, float data){
    tuple<string, float> message = make_tuple(addr, data);
    lock();
    osc_message_queue.push(message);
    unlock();
}

void OscThread::signal(){
    wait_for_unlock.notify_one();
}
