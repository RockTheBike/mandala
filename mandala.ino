// code for 12-point LED Mandala / Chobani

#define powerInletsBrightness 64 // brightness value of Turn Power Inlets ON  Low. 

// THESE ARE INDEX NUMBERS NOT PIN NUMBERS
#define outerRelay  0
#define triangle1   1 // PWM
#define vertex1     2
#define triangle2   3 // PWM
#define triangle3   4 // PWM
#define vertex2     5
#define vertex3     6
#define triangle4   7 // PWM
#define powerInlets 8 // PWM
#define ledPanels   9 // PWM
#define vertex4     10
#define vertex5     11
#define crissCross  12
#define beyondRelay 13
#define unusedsofar 14
#define PINS        15 // how many pins are there

//               0  1  2  3  4  5  6  7   8   9  10  11  12  13  14
int pin[PINS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A1, A2, A3};
int aw[PINS] = 0; // what analogWrite value to put
int oldAw[PINS] = {-1, -1, -1, -1, -1, -1, -1, -1, -1,  -1,  -1,  -1,  -1,  -1,  -1}; // last value


#define knobPin     A5  // the 

#define knobReads   12 // how many reads of the knob to average

unsigned long knobAdder = 0; // used for averaging knob reads
int knobValue = 0; // what value 0 to 1023 the knob is currently at
int knobMode = 0;  // which mode we are in right now out of 10 modes
int oldKnobMode = -1;
unsigned long startTime, progress; // how far along on the current animation sequence are we compared to millis()
int sequenceStage = 0; // in automated sequence, this stores which stage we're on

/* Timing related variables:  There will be lots of 600 ms delays. I’m trying to anticipate what will look like it is timed to their music. Maybe the delays can be global variables? 
 I would like to have a single global scalar variable that makes things run faster or slower. Like “Time Fudge Factor”. */
unsigned long timeNow;

void setup() {
  for (int i=2; i < 14; i++) pinMode(i, OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A3,OUTPUT);
  Serial.begin(57600);
  Serial.println("mandala arbduino program");
  startTime = millis();
}

void loop() {
  timeNow = millis();
  knobAdder = 0;
  for (int i=0; i < knobReads; i++) knobAdder += analogRead(knobPin);
  knobValue = knobAdder / knobReads; // averaged value over a bunch of reads
  knobMode = knobValue / 102;  // knobMode will be 0 through 9
  if (knobMode != oldKnobMode) {  // if we have changed modes from last time
    Serial.print("switched to mode ");
    Serial.println(knobMode);
    // Debug CODE:  Please print out to the Serial monitor what Light Function is currently running.     
    oldKnobMode = knobMode;
    startTime = timeNow; // start a new sequence
  }
  progress = timeNow - startTime; // how far along on the current animation sequence are we

    switch (knobMode) {
  case 0:  // (all the way LEFT): do nothing
    allOff();
    Serial.print(knobMode);
    break;
  case 1:  // powerInlets is on, nothing else
    powerInletsOn();  
    Serial.print(knobMode);
    break;
  case 2:  // Begin the time counter and do SlowRandomTriangleFade for 10 s
    entireSequence();
    Serial.print(knobMode);
    break;
  case 3:  // OneTriangleAtaTime, 10s
    oneTriangle();
    Serial.print(knobMode);
    break;
  case 4:  // TriangleBuild(); 8s
    triangleBuild();
    Serial.print(knobMode);
    break;
  case 5:  // InnerOverlay(): 8s
    innerOverlay();
    Serial.print(knobMode);
    break;
  case 6:  // VertexSweep(); 6s
    vertexSweep();
    Serial.print(knobMode);
    break;
  case 7:  // TriangleBuildFast(); 6s
    triangleBuildFast();
    Serial.print(knobMode);
    break;
  case 8:  // VertexSweepFast(); 6s
    vertexSweepFast();
    Serial.print(knobMode);
    break;
  case 9:  // ClimacticBuild(); Let it run as shown in pseudocode. 
    climacticBuild();
    Serial.print(knobMode);
    break;
  }
  updateAnalogWrites(); // so we only update analogWrite when they change values
}

/* Whenever you change from one light function to another reset all of these to OFF: Outer Relay, Beyond. Each light function will say if it wants these to be ON.
 // If knob moves to 2, execute a 1-minute sequence of the following light functions. 
 // If knob moves to 3 or higher positions, let the user drive. 
 
 Knob positions / sequence: 
 0 (all the way LEFT): do nothing. 
 1: powerInlets is on, nothing else
 2: Begin the time counter and do SlowRandomTriangleFade for 10 s
 3: OneTriangleAtaTime, 10s
 4. TriangleBuild(); 8s
 5. InnerOverlay(): 8s
 6. VertexSweep(); 6s
 7. TriangleBuildFast(); 6s
 8. VertexSweepFast(); 6s
 9. ClimacticBuild (); Let it run as shown in pseudocode. 
 */

void updateAnalogWrites() {
    if (aw[1] != oldAw[1]) {
    oldAw[1] = aw[1];  // update our knowledge of its status
    analogWrite(pin[1],aw[1]); // set the analogWrite value
  }
  if (aw[3] != oldAw[3]) {
    oldAw[3] = aw[3];  // update our knowledge of its status
    analogWrite(pin[3],aw[3]); // set the analogWrite value
  }
  if (aw[4] != oldAw[4]) {
    oldAw[4] = aw[4];  // update our knowledge of its status
    analogWrite(pin[4],aw[4]); // set the analogWrite value
  }
  if (aw[7] != oldAw[7]) {
    oldAw[7] = aw[7];  // update our knowledge of its status
    analogWrite(pin[7],aw[7]); // set the analogWrite value
  }
  if (aw[8] != oldAw[8]) {
    oldAw[8] = aw[8];  // update our knowledge of its status
    analogWrite(pin[8],aw[8]); // set the analogWrite value
  }
  if (aw[9] != oldAw[9]) {
    oldAw[9] = aw[9];  // update our knowledge of its status
    analogWrite(pin[9],aw[9]); // set the analogWrite value
  }
}

void allOff() {
  digitalWrite(pin[outerRelay],LOW);
  aw[triangle1] = 0;
  digitalWrite(pin[vertex1],LOW);
  aw[triangle2] = 0;
  aw[triangle3] = 0;
  digitalWrite(pin[vertex2],LOW);
  digitalWrite(pin[vertex3],LOW);
  aw[triangle4] = 0;
  aw[powerInlets] = 0;
  aw[ledPanels] = 0;
  digitalWrite(pin[vertex4],LOW);
  digitalWrite(pin[vertex5],LOW);
  digitalWrite(pin[crissCross],LOW);
  digitalWrite(pin[beyondRelay],LOW);
}

void powerInletsOn() { // Turn Power Inlets ON  Low. 
  // This program stays in effect while other programs are running unless other programs 
  // say “All Lights OFF” or “All Lights Bright”. Then do as instructed by the other programs. 
  digitalWrite(pin[outerRelay],LOW);
  aw[triangle1] = 0;
  digitalWrite(pin[vertex1],LOW);
  aw[triangle2] = 0;
  aw[triangle3] = 0;
  digitalWrite(pin[vertex2],LOW);
  digitalWrite(pin[vertex3],LOW);
  aw[triangle4] = 0;
  aw[powerInlets] = powerInletsBrightness;
  aw[ledPanels] = 0;
  digitalWrite(pin[vertex4],LOW);
  digitalWrite(pin[vertex5],LOW);
  digitalWrite(pin[crissCross],LOW);
  digitalWrite(pin[beyondRelay],LOW);
}

void entireSequence() {


}

void slowRandomTriangleFade() {
  /* SlowRandomTriangleFade();
   LED Panels OFF
   Triangles fade from  OFF-low - medium-low-OFF  at random with
   a 1200ms cycle and a 800ms pause. So OFF for 800ms then begin
   a low-medium-low fade. This takes 2 s per triangle. Stagger
   them and begin them at random so that some are increasing and
   some are decreasing at the same time. */
  digitalWrite(pin[ledPanels,LOW);
//  unsigned int triProgress = progress % 2000;
  if (progress < 2000) {
    int fade = progress / ( 800 / 64); // first 800ms is off to low
    aw[triangle1 = fade;  // how to do an analogWrite
    analogWrite(pin[triangle2,fade);
    analogWrite(pin[triangle3,fade);
    analogWrite(pin[triangle4,fade);
  }
  if ((progress >= 800) && (progress < 2800)) {

  
  }
}

void oneTriangle() {
/* OneTriangleAtaTime();
 LED panels are on low 
 Fade in Triangle 1 to full brightness over 300ms. Leave on for 600ms. Fade out over 300ms. Pause for 600ms.
 Repeat for Triangle 2, 3, 4. */
  aw[ledPanels] = 64; //   LED panels are on low 
  if (progress < 300) aw[triangle1] = (int)((float)progress / 1.178); // 0 to 255 at 300 progress milliseconds
  if ((progress >= 300) && (progress < 900)) aw[triangle1] = 255;  // Leave on for 600ms.
  
  
  
  
  
  
  
  
  if (progress < 256) analogWrite(triangle1,progress);
  if ((progress >= 256) && (progress < 856)) digitalWrite(triangle1,HIGH);
  if ((progress >= 856) && (progress < 1112)) analogWrite(triangle1,255 - (progress - 856));
  if ((progress >= 1112) && (progress < 1112))
}

void triangleBuild() {
}

void     innerOverlay() {
}

void     vertexSweep() {
}

void     triangleBuildFast() {
}

void     vertexSweepFast() {
}

void     climacticBuild() {
}

/* SlowRandomTriangleFade();
 LED Panels OFF
 Triangles fade from  OFF-low - medium-low-OFF  at random with a 1200ms cycle and a 800ms pause. So OFF for 800ms then begin a low-medium-low fade. This takes 2 s per triangle. Stagger them and begin them at random so that some are increasing and some are decreasing at the same time. 
 
 OneTriangleAtaTime();
 LED panels are on low 
 Fade in Triangle 1 to full brightness over 300ms. Leave on for 600ms. Fade out over 300ms. Pause for 600ms.
 Repeat for Triangle 2, 3, 4. 
 
 TriangleBuild();
 
 LED panels on LOW
 
 Whenever you turn on a triangle, do so with PWM fade from OFF-BRIGHT over the course of 100ms. Ditto with turning off a triangle. 
 
 Turn on Triangle 1, then 2, then 3, then 4, with 600ms delay. 
 
 When all are ON, make LED panels bright. When you start turning them OFF make LED panels low again. This is to send a pulse of LED to wash over the pedalers at this mini peak. 
 
 Turn them OFF in the same order (1, then 2, then 3, then 4). 
 
 Start again, but with Triangle 2 as the first one. So it would be Triangle, 2, 3, 4, 1 (with 600ms delay) then OFF in that order. 
 
 Then do it again with Triangle 3 as the first one, and so forth.
 
 TriangleBuildFast();
 Same as Triangle build but all delays are halved.
 
 InnerOverlay(): 
 
 LED panels on medium
 Turn on Triangle 1 and Vertex 1 at the same time. Delay 600ms
 ON Triangle2 and Vertex2
 Triangle3 and Vertex3
 Triangle4 and Vertex4
 ON Vertex 5 and CrissCross:
 At this point all lights in the mandala are ON. Make LED panels bright. 
 delay 1000ms
 repeat
 
 VertexSweep();
 LED panels on medium
 
 Turn Vertex1 ON, then Vertex2, 3, 4, 5, CrissCross, with 600ms delay
 Leave all on for 1200ms
 Then turn off in the reverse order: CrissCross, 5, 4, 3, 2, 1. 
 Beyond ON
 Repeat
 
 
 VertexSweepFast();
 
 Same as Vertex sweep but all delays are halved.
 
 ClimacticBuild();
 
 TriangleBuildFast();
 All LED Panels on medium;
 InnerOverlay();
 All LED Panels ON bright;
 OuterRelay ON
 Beyond ON
 
 VertexSweepFast();
 All lights ON.
 Outer Relay OFF 
 Outer Relay ON  Delay 300
 Outer Relay OFF Delay 300
 Outer Relay ON. Delay 300
 Outer Relay OFF. Delay 300
 
 Outer Relay ON
 
 Turn off pins in the following order with a 300ms delay 
 CrissCross
 Vertex 5, 4, 3, 2, 1 
 Beyond OFF
 Outer Relay OFF
 LED panels OFF
 Finale Lights ON (VJ needs to flip physical switch) 
 SlowRandomTriangleFade();  (stays on indefinitely) 
 */


