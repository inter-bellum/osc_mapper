//
//  OscThread.hpp
//  interbellum_osc_mapper
//
//  Created by denotebook on 18/11/2021.
//

#ifndef OscThread_hpp
#define OscThread_hpp

#include <stdio.h>
#include <ofThread.h>
#include <ofxOsc.h>
#include <queue>
#include <string>
#include <tuple>
#include <condition_variable>

using namespace std;

class OscThread : public ofThread {
public:
    OscThread(){};
    ~OscThread(){
        osc_send.clear();
    }
    
    void setup(string name, string remote_address, int port);
    void threadedFunction();
    void set_address(string remote_address);
    void set_port(int port);
    void send_message(string addr, float data);
    void signal();
    
private:
    queue<tuple<string, float>> osc_message_queue;
    ofxOscSender osc_send;
    string name;
    string remote_address;
    int port;
    condition_variable wait_for_unlock;
};

#endif /* OscThread_hpp */
