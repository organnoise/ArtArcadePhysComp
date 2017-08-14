#pragma once

#include <vector>
#include <string>

#include "ofMain.h"
#include "ofxOsc.h"

//#define HOST "organnoise.local"
//#define HOST "videoHost.local"
#define PORT 6000

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void fade();
    void switchLogo();
    
    
    void keyPressed(int key);
    
private:
    
    ofxOscSender send;
    ofxOscReceiver recieve;
    ofVideoPlayer player;
    ofImage logo;
    
    
    
    //string HOST = "organnoise.local";
    string HOST = "10.40.12.66";
    int hostToggle;
    int numVids;
    int counter;
    int rand = 0;
    int pRand = 0;
    bool doneSend;
    
    int fadeVal, fadeValFlip;
    bool done, change;
    
    float w = ofGetWindowWidth() * 0.8;
    float h = w / 2.65;
    
    string images[3] = {"1.png", "2.png", "3.png"};
    
    vector<string> video;
    
};
