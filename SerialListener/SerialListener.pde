import processing.serial.*;
import java.util.*;
import java.lang.StringBuilder; 

// Flag for DEBUG Mode
final boolean DEBUG = false;

// Settings of Serial Buffer & Serial Port
final char OUTER_KEY = '\n';
final int BAUDRATE = 115200;
Serial serialPort;      // the serial port instance
String serialBuffer;    // the content that current transfer into serial
boolean isNewDataComing;    // the flag of whether a new data is coming into serial or not

// Multi-Windows Settings
Plotter plotter_A = new Plotter("A");  // plotter for Acceleration
Plotter plotter_V = new Plotter("V");  // plotter for Velocity
Plotter plotter_D = new Plotter("D");  // plotter  for Displacement

// Integral Info
IntegralInfo preA, preV, preD;
IntegralInfo curA, curV, curD;
Integral integratorA2D;

// PRE-TREATMENT
boolean isFirstMaximal = true;
int Trend;
// float bias = 0.035;    // Acceleration Wave Bias
IntegralInfo preMaximal;

// PRE-TREATMENT : Exponential Moving Average
float beta = 0.4;

// ========================================================================== //

// Settings of Canvas
final int BG_COL = 255;

// Plotting Variables
int xPos = 0;           // Plotting's X Position
float preY = 0, curY = 0;   // Plotting's Y Position: Raw Data

void setup() {       
    
    // ================================== //
    // Canvas Initialization
    size(800, 400);
    textSize(24);
    background(BG_COL);
    surface.setResizable(true);
    frameRate(60);
    
    // ================================== //
    // Serial Connection
    tryConnect();
    isNewDataComing = false;    
    
    // ================================== //
    // Integral
    integratorA2D = new Integral();
    
    // ================================== //
    // Multi-Windows Configuration
    String[] args_A = {"Acceleration"};
    PApplet.runSketch(args_A, plotter_A);
    String[] args_V = {"Velocity"};
    PApplet.runSketch(args_V, plotter_V);
    String[] args_D = {"Displacement"};
    PApplet.runSketch(args_D, plotter_D);
    
    //
    logManager = new LogManager();
}

// ========================================================================== //

long timeStamp = 0;

// DEBUG : Cosine Simulation Wave
float cosValue = 0.0;
float inc = TWO_PI / 160.0;

// 
boolean isFirstNewData = true;

void draw() {
    // check if there comes a newData from serial port
    // only run draw() when there is new data available
    if (!isNewDataComing)    return;
    else    isNewDataComing = false;
    
    // Neglect the first info, since we'll use firstNewData to become first preX
    // we have to gete correct timeStamp 
    if (isFirstNewData) {
        // set isFirstNewData flag to false, won't use trigger this flag any more
        isFirstNewData = false;
        
        // Initialization preX 
        preA = new IntegralInfo(curA.getTimeStamp(), 0.0);
        preV = new IntegralInfo(curA.getTimeStamp(), 0.0);
        preD = new IntegralInfo(curA.getTimeStamp(), 0.0);
    }
    
    try {
        // DEBUG : Constant Acceleration
        // curA = new IntegralInfo(++timeStamp, 1);
        
        // DEBUG : Cosine Wave
        // curA = new IntegralInfo(++timeStamp, cos(cosValue));
        // cosValue += inc;
        
        // DEBUG
        println("\n\n=== SECTION START ===");
        println("preA : " + preA);
        println("curA : " + curA);
        
        // plot the original data from serial, for debug use
        plotOnSelfCanvas();
        
        // integral the data & show the result on diff windows
        processThenRefresh();
        
        // write the info to log, for recording
        log();
        
    } catch(Exception e) {
        logMessage("Exception on connect: " + e.toString(), true);	
    }
}

// ========================================================================== //
// LOG
// connect with LogManager

// LogManager
LogManager logManager;

// write log to logManager
void log() {
    // log
    
    StringBuilder sb = new StringBuilder();
    sb.append(curA.getTimeStamp());
    sb.append(",");
    sb.append(curA.getValue());
    sb.append(",");
    sb.append(curV.getValue());
    sb.append(",");
    sb.append(curD.getValue());
    
    // call LogManager to handle the log to file
    logManager.writeLog(sb.toString());
}

void keyPressed() {
    logManager.flush();
    logManager.close();
    exit();
}

// ========================================================================== //

float bias = -0.0088;

void preTreatment() {
    // PRE-TREATMENT
    // Exponential Moving Average
    curA.setValue(curA.getValue() + bias);
    curA.setValue(beta * curA.getValue() + (1.0 - beta) * preA.getValue());
    if (curA.getValue() >= -0.10 && curA.getValue()  <= 0.10)
        curA.setValue(0.0);
}

void integralForVD() {
    // call integral to get curV, curD
    List<IntegralInfo> res = integratorA2D.getCurVD(curA, preA, preV, preD);
    // and new history appear
    curV = res.get(0);
    curD = res.get(1);
    
    // PRE-TREATMENT
    // calculate the bias of accerleration wave
}

void refreshPlot() {
    // multi-windows : call setValue() to refresh the plot display
    plotter_A.setValue(curA.getValue());
    plotter_V.setValue(curV.getValue());
    plotter_D.setValue(curD.getValue());
}

void processThenRefresh() {
    // process, and then refresh the content on each windows
    preTreatment();
    integralForVD();
    refreshPlot();
    
    // current becomes history
    preA = curA;
    preV = curV;
    preD = curD;
}

// ========================================================================== //

void plotOnSelfCanvas() {
    
    // // map the data to Canvas' height or width first
    float cur = map(curA.getValue(), 3, -3, 0, height);
    float pre = map(preA.getValue(), 3, -3, 0, height);
    
    // float cur = curA.getValue();
    // float pre = preA.getValue();
    
    // plot Coordinate
    stroke(0, 0, 0);
    strokeWeight(3);
    line(0, height / 2, width, height / 2);
    line(width / 2, 0, width / 2, height);
    
    // plot a point & line 
    stroke(255, 0, 0);
    strokeWeight(3);
    point(xPos, height / 2 + cur);
    if (xPos != 0) {
        line(xPos - 1, height / 2 + pre, xPos, height / 2 + cur);
        // preA = curA;
    }
    
    // at the edge of the screen, go back to the beginning:
    if (xPos >= width) {
        xPos = 0;
        background(BG_COL);
    } else {
        // increment the horizontal position:
        xPos++;
    }
    
}

// ========================================================================== //

void tryConnect() {
    // Serial Connection
    while(Serial.list().length <1) {
        text("No serial ports available. Waiting...", 20, 20);	
        delay(100);
    }
    
    int portIndex = 0;
    // // if there exists more than one available serial port
    // // let the user type to choose target port index
    // if (Serial.list().length() > 1) {
    //     printArray(Serial.list());
    //     logMessage("Type to choose the target Serial Port");
// }
    
    // Open the port you are using at the rate you want:
    String portName = Serial.list()[portIndex];
    serialPort = new Serial(this, portName, BAUDRATE);
    logMessage("Connected on " + portName + ". Listening for configuration...", false);
    
    // serial port communication configuration
    serialPort.bufferUntil(OUTER_KEY);
}

void serialEvent(Serial ser) {
    try{
        // read raw message into buffer fromserial port
        // a success message should contain OUTER_KEY as its last character
        String message = ser.readStringUntil(OUTER_KEY);
        if (message == null || message.isEmpty() || message.equals(OUTER_KEY)) {
            return;
        }
        // serialBuffer = trim(message);
        serialBuffer = message;
        // print(serialBuffer);
        
        // Deserialization
        String[] contents = split(message, ",");
        long col1 = Long.parseLong(contents[0]);
        // Since we're observing Z-axis Acceleration, it'll have Gravity original
        // minus 1.0 in order to get rid of the infuence of gravity
        float acc_value = Float.parseFloat(contents[1]) - 1.0;  
        
        // Append a new IntegralInfo Instance
        curA = new IntegralInfo(col1, acc_value);
        
        // set the isNewDataComing flag on. in order to let draw() to update the plot
        isNewDataComing = true;
    } catch(Exception e) {
        logMessage("Exception on connect: " + e.toString(), true);	
    }
}

// ========================================================================== //
// ========================================================================== //

// Logger helper
void logMessage(String message, boolean debugOnly) {
    if (DEBUG || !debugOnly) {
        String level = debugOnly ? "DEBUG" : "STATUS";
        println("[Time : " + millis() + " ms]" + "[" + level + "] " + message);
    }
}
