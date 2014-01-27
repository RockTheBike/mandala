// code for 12-point LED Mandala / Chobani

#define powerInletsBrightness 64 // brightness value of Turn Power Inlets ON  Low. 

#define outerRelay  2
#define triangle1   3 // PWM pin
#define vertex1     4
#define triangle2   5 // PWM pin
#define triangle3   6 // PWM pin
#define vertex2     7
#define vertex3     8
#define triangle4   9 // PWM pin
#define powerInlets 10 // PWM pin
#define ledPanels   11 // PWM pin
#define vertex4     12
#define vertex5     13
#define crissCross  A1 // there is no 14
#define beyondRelay A2 // there is no 15
// note we haven't used A3 for anything yet

#define knobPin A5  // the 
#define knobReads 12 // how many reads of the knob to average
unsigned long knobAdder = 0; // used for averaging knob reads
int knobValue; // what value 0 to 1023 the knob is currently at
int knobMode,oldKnobMode = 0;  // which mode we are in right now out of 10 modes
int progress = -1; // how far along on the current animation sequence are we?

/* Timing related variables:  There will be lots of 600 ms delays. I’m trying to anticipate what will look like it is timed to their music. Maybe the delays can be global variables? 
   I would like to have a single global scalar variable that makes things run faster or slower. Like “Time Fudge Factor”. */

void setup() {
  for (int i=2; i < 14; i++) pinMode(i, OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A3,OUTPUT);
  Serial.begin(57600);
  Serial.println("mandala arbduino program");
}

void loop() {
  knobAdder = 0;
  for (int i=0; i < knobReads; i++) knobAdder += analogRead(knobPin);
  knobValue = knobAdder / knobReads;
  knobMode = knobValue / 102;  // knobMode will be 0 through 9
  if (knobMode != oldKnobMode) {
    Serial.print("switched to mode ");
    Serial.println(knobMode);
    oldKnobMode = knobMode;
    progress = -1; // start a new sequence
  }
  switch (knobMode) {
  case 0:  // (all the way LEFT): do nothing
    allOff();
    Serial.print(knobMode);
    break;
  case 1:  // powerInlets is on, nothing else
    powerInlets();  
    Serial.print(knobMode);
    break;
  case 2:  // Begin the time counter and do SlowRandomTriangleFade for 10 s
    Serial.print(knobMode);
    break;
  case 3:  // OneTriangleAtaTime, 10s
    Serial.print(knobMode);
    break;
  case 4:  // TriangleBuild(); 8s
    Serial.print(knobMode);
    break;
  case 5:  // InnerOverlay(): 8s
    Serial.print(knobMode);
    break;
  case 6:  // VertexSweep(); 6s
    Serial.print(knobMode);
    break;
  case 7:  // TriangleBuildFast(); 6s
    Serial.print(knobMode);
    break;
  case 8:  // VertexSweepFast(); 6s
    Serial.print(knobMode);
    break;
  case 9:  // ClimacticBuild (); Let it run as shown in pseudocode. 
    Serial.print(knobMode);
    break;
  }
}
// Debug CODE:  Please print out to the Serial monitor what Light Function is currently running. 
PowerInlets()  -- This stays in effect even if knob is all the way LEFT. 

Listen for knob motion 

Whenever you change from one light function to another reset all of these to OFF: Outer Relay, Beyond. Each light function will say if it wants these to be ON.

If knob moves to 2, execute a 1-minute sequence of the following light functions. 
If knob moves to 3 or higher positions, let the user drive. 
/*
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

void allOff() {
  digitalWrite(outerRelay,LOW);
  digitalWrite(triangle1,LOW);
  digitalWrite(vertex1,LOW);
  digitalWrite(triangle2,LOW);
  digitalWrite(triangle3,LOW);
  digitalWrite(vertex2,LOW);
  digitalWrite(vertex3,LOW);
  digitalWrite(triangle4,LOW);
  digitalWrite(powerInlets,LOW);
  digitalWrite(ledPanels,LOW);
  digitalWrite(vertex4,LOW);
  digitalWrite(vertex5,LOW);
  digitalWrite(crissCross,LOW);
  digitalWrite(beyondRelay,LOW);
}
  
void powerInlets() { // Turn Power Inlets ON  Low. 
// This program stays in effect while other programs are running unless other programs 
// say “All Lights OFF” or “All Lights Bright”. Then do as instructed by the other programs. 
  digitalWrite(outerRelay,LOW);
  digitalWrite(triangle1,LOW);
  digitalWrite(vertex1,LOW);
  digitalWrite(triangle2,LOW);
  digitalWrite(triangle3,LOW);
  digitalWrite(vertex2,LOW);
  digitalWrite(vertex3,LOW);
  digitalWrite(triangle4,LOW);
  analogWrite(powerInlets,powerInletsBrightness);
  digitalWrite(ledPanels,LOW);
  digitalWrite(vertex4,LOW);
  digitalWrite(vertex5,LOW);
  digitalWrite(crissCross,LOW);
  digitalWrite(beyondRelay,LOW);
}

SlowRandomTriangleFade();
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

 






