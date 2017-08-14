// HID
Hid hi;
HidMsg key;

Serial serial;
serial.setup(3);


"film-mac-3.local" => string hostname;
6000 => int port;
// send object
OscOut osc;

// aim the transmitter
osc.dest( hostname, port );


OscIn oin;
OscMsg msg;

6000 => oin.port;
oin.listenAll();
// which keyboard
0 => int device;

//The Shred ID for auto-reseting
Shred autoResetId;

//Set Auto-Reset Timeout here!
20::second => dur autoResetTimeout;

int counter;
0 => int numMovies;
int autoResetBool;

Shred s;
0 => int currentMovie;
1 => int pCurrentMovie;
int movieChanged;

// open keyboard (get device number from command line)
if( !hi.openKeyboard( device ) ) me.exit();
<<< "keyboard '" + hi.name() + "' ready", "" >>>;

spork~ oscListener();
//spork~ serial.loop();
spork~ coinListener();
spork~ movieNumWaiter();

main();

fun void main(){
    // infinite event loop
    while( true )
    {
        // wait for event
        hi => now;
        
        // get message
        while( hi.recv( key ) )
        {
            // check
            if( key.isButtonDown() )
            {
                //<<< key.which >>>;
                if(key.which == 44){
                    
                    killAutoReset();
                    //If space bar, increment movie
                    counter++;
                    counter % numMovies  => counter;
                    <<<"Playing ", counter>>>;
                    osc.start("/counter");
                    osc.add(counter);
                    osc.send();
                    
                    
                }
                //"F" means Toggle Fullscreen
                if(key.which == 9){
                    <<<"Toggle Fullscreen">>>;
                    osc.start("/toggle");
                    osc.add(1);
                    osc.send(); 
                }
                
                if(key.which == 10){
                    getMovies();
                }
                
                if(key.which == 21){
                    killAutoReset();
                    randomVid();
                }
                
                if(key.which == 7) {
                    <<<"Manuel Reset">>>;
                    resetMech();
                }
                
            }
        }
    }
}

fun void oscListener(){
    while ( true )
    {
        // wait for event to arrive
        oin => now;
        
        // grab the next message from the queue. 
        while ( oin.recv(msg) != 0 )
        { 
            if(msg.address == "/done"){
                // print
                <<< "Done", msg.getInt (0) >>>;
                resetMech();
                0 => autoResetBool;
                killAutoReset();
            }
            if(msg.address == "/numMovies"){
                // print
                <<< "Number of Movies: ", msg.getInt(0) >>>;
                msg.getInt(0) => numMovies;
            }
            if(msg.address == "/currentMovie"){
                // print
                <<< "Current Movie: ", msg.getInt(0) >>>;
                msg.getInt(0) => currentMovie;
                1 => movieChanged;
                killAutoReset();
            }
        }
    }
    
}

fun void coinListener(){
    while(true){
        
        serial.serialNotify => now;
        
        if(serial.data[1] == 1){
            randomVid();
        }
    }
    
}

fun void randomVid(){
    while(movieChanged == 0){
        <<<"Random Movie...">>>;
        osc.start("/randomVid");
        osc.add(1);
        osc.send();
        200::ms => now;
    }
    
    spork ~ autoReset() @=> autoResetId;
    <<<"Auto reset Id: ",autoResetId.id() >>>;
    0 => movieChanged;
}

fun void getMovies(){
    <<<"Getting Movies">>>;
    osc.start("/get");
    osc.add(1);
    osc.send(); 
}

fun void movieNumWaiter(){
    while(numMovies == 0){
        getMovies();
        500::ms => now;
    }
}

fun void autoReset(){
    1 => autoResetBool;
    autoResetTimeout => now;
    <<<"Auto Reset">>>;
    if(autoResetBool == 1) resetMech();
    else <<<"no auto-reset needed">>>;
    
}
fun void resetMech(){
    <<<"Reseting Coin Mech...">>>;
    serial.send(0,1);
}

fun void killAutoReset(){
   
    if (autoResetId.id() > 0) {
         <<<"Killing Auto Reset...">>>;
        Machine.remove(autoResetId.id());
    }
    else <<<"No Auto Reset to Kill...">>>;
}


//INLINE CLASS DEFINITION:
class Serial {
    SerialIO cereal;
    Event serialNotify;
    
    2 => int numBytes;
    
    int bytes[];
    string line;
    string stringInts[numBytes];
    int data[numBytes];
    numBytes => int digits;
    
    fun void setup(int dev){
        // list cereal devices
        SerialIO.list() @=> string list[];
        
        // no cereal devices available
        if(list.cap() == 0)
        {
            cherr <= "no cereal devices available\n";
            me.exit(); 
        }
        
        // print list of cereal devices
        chout <= "Available devices\n";
        for(int i; i < list.cap(); i++)
        {
            chout <= i <= ": " <= list[i] <= IO.newline();
        }
        
        // parse first argument as device number
        dev => int device;
        if(me.args()) me.arg(0) => Std.atoi => device;
        
        if(device >= list.cap())
        {
            cherr <= "cereal device #" <= device <= "not available\n";
            me.exit(); 
        }
        
        
        if(!cereal.open(device, SerialIO.B9600, SerialIO.ASCII))
        {
            chout <= "unable to open cereal device '" <= list[device] <= "'\n";
            me.exit();
        }
        // pause to let cereal device finish opening
        2::second => now;
        spork~ poller();
    } 
    
    fun void poller(){
        while(true){
            // Grab Serial data
            cereal.onLine()=>now;
            cereal.getLine()=>line;
            
            if( line$Object == null ) continue;
            if( line == "\n" ) continue;
            
            0 => stringInts.size;
            
            // Line Parser
            string pattern;
            "\\[" => pattern;
            for(int i;i<digits;i++){
                "([0-9]+)" +=> pattern;
                if(i<digits-1){
                    "," +=> pattern;
                }
            }
            "\\]" +=> pattern;
            if (RegEx.match(pattern, line , stringInts))
            {
                for(1 =>int i; i<stringInts.cap(); i++)  
                {
                    // Convert string to Integer
                    Std.atoi(stringInts[i])=>data[i-1];
                }
            }
            
            serialNotify.signal();
            //<<< data[0], data[1]>>>;
            5::ms => now;
        } 
    }
    
    fun void send(int a, int b){
        [255, a, b] @=> bytes;
        // write to cereal device
        cereal.writeBytes(bytes);
    }
    
    fun void loop(){
        while(true){
            //Wait for a serial notify event
            serialNotify => now;
            
            //<<<"DATA">>>;
            <<< data[0], data[1]>>>;
            
        }
    }
}