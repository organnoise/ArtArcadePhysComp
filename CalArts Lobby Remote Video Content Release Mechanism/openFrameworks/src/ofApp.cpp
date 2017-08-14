#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofHideCursor();
    
    //some path, may be absolute or relative to bin/data
    string path = ".";
    ofDirectory dir(path);
    //only show png files
    dir.allowExt("mov");
    //populate the directory object
    dir.listDir();
    
    //go through and print out all the paths
    for(int i = 0; i < dir.size(); i++){
       
        video.push_back(dir.getName(i));
        ofLogNotice(video.at(i));
    }
    cout << video.size();
    
    player.setLoopState(OF_LOOP_NONE);
    player.load(video[1]);
    player.play();
    
    cout << player.getLoopState();
    
    recieve.setup(PORT);
    send.setup(HOST, PORT);
    
    ofxOscMessage m;
    
    numVids = video.size();
    
    m.setAddress("/numMovies");
    m.addIntArg(numVids);
    send.sendMessage(m,false);
    
    logo.load("image/" + images[0]);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    while(recieve.hasWaitingMessages()){
        ofxOscMessage m;
        ofxOscMessage s;
        recieve.getNextMessage(&m);
        
        
        //Listen for messages prepended with "/counter"
        if(m.getAddress() == "/counter"){
            
            s.setAddress("/currentMovie");
            s.addIntArg(counter);
            send.sendMessage(s,false);
            
            //Increment counter then play the new video
            counter = m.getArgAsInt32(0);
            player.load(video[counter]);
            player.play();
            done = false;
            change = true;
            //flag to try and stop multiple done messages
            doneSend = false;
        }

        if(m.getAddress() == "/toggle"){
            ofToggleFullscreen();
        }
        if(m.getAddress() == "/get"){
            s.setAddress("/numMovies");
            s.addIntArg(numVids);
            send.sendMessage(s,false);
        }
        
        if(m.getAddress() == "/randomVid"){
            cout << "Rando" << endl;
            while(rand == pRand){
                rand = (int)ofRandom(video.size());
            }
            
            s.setAddress("/currentMovie");
            s.addIntArg(rand);
            send.sendMessage(s,false);
            
            player.load(video[rand]);
            player.setLoopState(OF_LOOP_NONE);
            player.play();
            done = false;
            change = true;
            //flag to try and stop multiple done messages
            doneSend = false;
            pRand = rand;
        }
        if(m.getAddress() == "/changehost"){
            hostToggle = !hostToggle;
            
            //if(hostToggle == 1) HOST = "organnoise.local";
            if(hostToggle == 1) HOST = "10.40.12.66";
            else HOST = "videoHost.local";
            cout << HOST << endl;
            send.setup(HOST, PORT);
        }
        if(m.getAddress() == "/changehostIP"){
            string ip = m.getArgAsString(0);
            cout << ip << endl;
            send.setup(ip, PORT);
        }
        
        
    }
    
    //cout << player.getCurrentFrame() << " , "<< player.getTotalNumFrames() << endl;
    
    //if(player.getIsMovieDone()) cout << "DONE!!!!!" << endl;
    
    
    if(player.getPosition() >= 0.98f){

        player.setPaused(true);
        done = true;
        //player.stop();
        ofxOscMessage m;
        if(doneSend == false){
            cout << "Done" << endl;
            doneSend = true;
            m.setAddress("/done");
            m.addIntArg(1);
            send.sendMessage(m,false);
            }
        }
    player.update();
    
    fade();
    switchLogo();
    
    //if(fadeVal == 0) player.stop();

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0);
    ofSetColor(255,255,255,fadeVal);
    player.draw(0, 0, ofGetWidth(), ofGetHeight());
    
    ofSetColor(255,255,255,fadeValFlip);
    
    w = ofGetWindowWidth() * 0.8;
    h = w / 2.65;
    
    logo.draw(ofGetWindowWidth() * 0.5 - w * 0.5,
              ofGetWindowHeight() * 0.5 - h * 0.5,
              w, h);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key){
        case 'f':
            ofToggleFullscreen();
            break;
        case 's':
            counter++;
            counter %= video.size();
            
            player.load(video[counter]);
            player.setLoopState(OF_LOOP_NONE);
            ofHideCursor();
            player.setFrame(0);
            player.play();
            done = false;
            change = true;
            //flag to try and stop multiple done messages
            doneSend = false;
            break;
        case 'c':
            hostToggle = !hostToggle;
            
            if(hostToggle == 1) HOST = "organnoise.local";
            else HOST = "videoHost.local";
            cout << HOST << endl;
            send.setup(HOST, PORT);

            break;
    }

}

void ofApp::fade(){
    if(done && fadeVal > 0) fadeVal -= 5;
    else if (done == false && fadeVal < 255) fadeVal += 10;
    
    fadeValFlip = ofMap(fadeVal, 0, 255, 255, 0);
}

void ofApp::switchLogo(){
    if(change && fadeValFlip == 0){
        change == false;
        logo.load("image/" + images[int(ofRandom(3))]);
    }
}
