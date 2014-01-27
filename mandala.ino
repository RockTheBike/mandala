// code for 12-point LED Mandala / Chobani

#define powerInletsBrightness 64 // brightness value of Turn Power Inlets ON  Low. 
#define ledPanelsLow 64 // brightness value of LED panels are on low
#define ledPanelsMedium 128 // brightness described in innerOverlay()
#define slowRandomMedium 128 // medium brightness described in slowRandomTriangleFade()
#define slowRandomLow 32 // low brightness described in slowRandomTriangleFade()

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
int aw[8] = {0}; // what analogWrite value to put
int oldAw[8] = {-1}; // last value
int fadeRandom[8]; // for random fading in slowRandomTriangleFade()
int triangle[4] = {triangle1, triangle2, triangle3, triangle4};

#define knobPin     A5  // the 

#define knobReads   12 // how many reads of the knob to average

unsigned long knobAdder = 0; // used for averaging knob reads
int knobValue = 0; // what value 0 to 1023 the knob is currently at
int knobMode = 0;  // which mode we are in right now out of 10 modes
int oldKnobMode = -1;
unsigned long startTime, progress = 0; // how far along on the current animation sequence are we compared to millis()
int sequenceStage = 0; // in automated sequence, this stores which stage we're on
int buildOrder = 0;  // which triangle to start with in triangleBuild()

/* Timing related variables:  There will be lots of 600 ms delays. I’m trying to anticipate what will look like it is timed to their music. Maybe the delays can be global variables? 
 I would like to have a single global scalar variable that makes things run faster or slower. Like “Time Fudge Factor”. */
unsigned long timeNow;

void setup() {
  randomSeed(analogRead(A3));
  for (int i=0; i < PINS; i++) pinMode(pin[i], OUTPUT);
  Serial.begin(57600);
  Serial.println("mandala arbduino program");
  startTime = millis();
  fadeRandom[triangle1] = random(2000);
  fadeRandom[triangle2] = random(2000);
  fadeRandom[triangle3] = random(2000);
  fadeRandom[triangle4] = random(2000);
}

void loop() {
  timeNow = millis();
  knobAdder = 0;
  for (int i=0; i < knobReads; i++) knobAdder += analogRead(knobPin); // averaged value over a bunch of reads
  knobValue = knobAdder / knobReads; // knobValue will be 0 to 1023
  knobMode = knobValue / 102;  // knobMode will be 0 through 9
  if (knobMode != oldKnobMode) {  // if we have changed modes from last time
    Serial.print("switched to mode "); // Debug CODE:  Please print out to the
    Serial.println(knobMode); // Serial monitor what Light Function is currently running
    oldKnobMode = knobMode;
    startTime = timeNow; // start a new sequence
    sequenceStage = 0; // make sure we start sequence at the beginning if applicable
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
  case 2:  // Begin the time counter [starting with] SlowRandomTriangleFade for 10 s
    entireSequence();
    // Serial.print(knobMode); don't do this because it's done in entireSequence()
    break;
  case 3:  // OneTriangleAtaTime, 10s
    if (progress > oneTriangle()) startTime = timeNow; // make them repeat themselves
    Serial.print(knobMode);
    break;
  case 4:  // TriangleBuild(); 8s
    if (progress > triangleBuild()) startTime = timeNow;
    Serial.print(knobMode);
    break;
  case 5:  // InnerOverlay(): 8s
    if (progress > innerOverlay()) startTime = timeNow;
    Serial.print(knobMode);
    break;
  case 6:  // VertexSweep(); 6s
    if (progress > vertexSweep()) startTime = timeNow;
    Serial.print(knobMode);
    break;
  case 7:  // TriangleBuildFast(); 6s
    if (progress > triangleBuildFast()) startTime = timeNow;
    Serial.print(knobMode);
    break;
  case 8:  // VertexSweepFast(); 6s
    if (progress > vertexSweepFast()) startTime = timeNow;
    Serial.print(knobMode);
    break;
  case 9:  // ClimacticBuild(); Let it run as shown in pseudocode. 
    if (progress > climacticBuild()) startTime = timeNow;
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

void updateAnalogWrites() { // set analogWrite only when value has changed
  if (aw[1] != oldAw[1]) { // you can't just analogWrite all the damn time
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
  switch (sequenceStage) {
  case 0:
    sequenceStage = 2;  // start the sequence
  case 2:  // Begin the time counter [starting with] SlowRandomTriangleFade for 10 s
    if (progress > slowRandomTriangleFade()) {
      startTime = timeNow; // pretend the mode is just starting
      sequenceStage++; // go to the next stage
    }
    Serial.print(sequenceStage);
    break;
  case 3:  // OneTriangleAtaTime, 10s
    if (progress > oneTriangle()) {
      startTime = timeNow; // pretend the mode is just starting
      sequenceStage++; // go to the next stage
    }
    Serial.print(sequenceStage);
    break;
  case 4:  // TriangleBuild(); 8s
    if (progress > triangleBuild()) {
      startTime = timeNow; // pretend the mode is just starting
      sequenceStage++; // go to the next stage
    }
    Serial.print(sequenceStage);
    break;
  case 5:  // InnerOverlay(): 8s
    if (progress > innerOverlay()) {
      startTime = timeNow; // pretend the mode is just starting
      sequenceStage++; // go to the next stage
    }
    Serial.print(sequenceStage);
    break;
  case 6:  // VertexSweep(); 6s
    if (progress > vertexSweep()) {
      startTime = timeNow; // pretend the mode is just starting
      sequenceStage++; // go to the next stage
    }
    Serial.print(sequenceStage);
    break;
  case 7:  // TriangleBuildFast(); 6s
    if (progress > triangleBuildFast()) {
      startTime = timeNow; // pretend the mode is just starting
      sequenceStage++; // go to the next stage
    }
    Serial.print(sequenceStage);
    break;
  case 8:  // VertexSweepFast(); 6s
    if (progress > vertexSweepFast()) {
      startTime = timeNow; // pretend the mode is just starting
      sequenceStage++; // go to the next stage
    }
    Serial.print(sequenceStage);
    break;
  case 9:  // ClimacticBuild(); Let it run as shown in pseudocode. 
    if (progress > climacticBuild()) {
      startTime = timeNow; // pretend the mode is just starting
      sequenceStage++; // go to the next stage
    }
    Serial.print(sequenceStage);
    break;
  }
}

long slowRandomTriangleFade() {
  /* SlowRandomTriangleFade();
   LED Panels OFF
   Triangles fade from  OFF-low - medium-low-OFF  at random with
   a 1200ms cycle and a 800ms pause. So OFF for 800ms then begin
   a low-medium-low fade. This takes 2 s per triangle. Stagger
   them and begin them at random so that some are increasing and
   some are decreasing at the same time. */
  aw[ledPanels] = 0;
  for (int tri = 0; tri < 4; tri++) {
    unsigned long triangleProgress = (progress + fadeRandom[triangle[tri]]) % 2000;  // 2000 ms cycle looped at random
    if (triangleProgress < 600)
      aw[triangle[tri]] = slowRandomLow + (int)((float)triangleProgress / (600.0 / (slowRandomMedium - slowRandomLow)));
    if ((triangleProgress >= 600) && (triangleProgress < 1200))
      aw[triangle[tri]] = slowRandomMedium - slowRandomLow + (int)((float)(triangleProgress - 600.0) / (600.0 / (slowRandomMedium - slowRandomLow)));
    if (triangleProgress >= 1200) aw[triangle[tri]] = 0;  // Pause for 800ms.
  }  // maybe that will work, i don't know.
  return 10000; // the number of milliseconds this routine is supposed to end at
}

long oneTriangle() {
/* OneTriangleAtaTime();
 Fade in Triangle 1 to full brightness over 300ms. Leave on for 600ms. Fade out over 300ms. Pause for 600ms.
 Repeat for Triangle 2, 3, 4. */
  aw[ledPanels] = ledPanelsLow; //   LED panels are on low 
  int whichTriangle;
  if (progress < 1800) whichTriangle = triangle1;
  if ((progress >= 1800) && (progress < 3600)) whichTriangle = triangle2;
  if ((progress >= 3600) && (progress < 5400)) whichTriangle = triangle3;
  if (progress >= 5400) whichTriangle = triangle4;
  long triangleProgress = progress % 1800; // how far into this triangle are we

  if (triangleProgress < 300) aw[whichTriangle] = (int)((float)triangleProgress / 1.178); // 0 to 255 at 300 triangleProgress milliseconds
  if ((triangleProgress >= 300) && (triangleProgress < 900)) aw[whichTriangle] = 255;  // Leave on for 600ms.
  if ((triangleProgress >= 900) && (triangleProgress < 1200)) aw[whichTriangle] = 255 - (int)((float)(triangleProgress - 900) / 1.178);  // Fade out over 300ms
  if (triangleProgress >= 1200) aw[whichTriangle] = 0;  // Pause for 600ms.

  return 10000; // the number of milliseconds this routine is supposed to end at
}

long triangleBuild() {
/* Whenever you turn on a triangle, do so with PWM fade from OFF-BRIGHT over the course of 100ms. Ditto with turning off a triangle.
 Turn on Triangle 1, then 2, then 3, then 4, with 600ms delay.
 When all are ON, make LED panels bright. When you start turning them OFF make LED panels low again. This is to send a pulse of LED to wash over the pedalers at this mini peak.
 Turn them OFF in the same order (1, then 2, then 3, then 4).
 Start again, but with Triangle 2 as the first one. So it would be Triangle, 2, 3, 4, 1 (with 600ms delay) then OFF in that order.
 Then do it again with Triangle 3 as the first one, and so forth.
*/
  aw[ledPanels] = ledPanelsLow; //   LED panels are on low [unless made bright]
  long triangleProgress = progress % 4000; // how far into this build are we
  buildOrder = progress / 4000;
  if (triangleProgress < 100) aw[triangle[buildOrder % 4]] = triangleProgress * 2.575; // turn on in 100ms
  if ((triangleProgress >= 600) && (triangleProgress < 700)) aw[triangle[(buildOrder +1) % 4]] = (triangleProgress - 600) * 2.575; // turn on in 100ms
  if ((triangleProgress >= 1200) && (triangleProgress < 1300)) aw[triangle[(buildOrder +2) % 4]] = (triangleProgress - 1200) * 2.575; // turn on in 100ms
  if ((triangleProgress >= 1800) && (triangleProgress < 1900)) aw[triangle[(buildOrder  +3) % 4]] = (triangleProgress - 1800) * 2.575; // turn on in 100ms
  if ((triangleProgress >= 1900) && (triangleProgress < 2000)) aw[ledPanels] = 255;  // When all are ON, make LED panels bright.
  if ((triangleProgress >= 2000) && (triangleProgress < 2100)) aw[triangle[(buildOrder % 4)]] = 255 - (triangleProgress - 2000) * 2.575; // turn off in 100ms
  if ((triangleProgress >= 2600) && (triangleProgress < 2700)) aw[triangle[(buildOrder +1) % 4]] = 255 - (triangleProgress - 2600) * 2.575; // turn off in 100ms
  if ((triangleProgress >= 3200) && (triangleProgress < 3300)) aw[triangle[(buildOrder +2) % 4]] = 255 - (triangleProgress - 3200) * 2.575; // turn off in 100ms
  if ((triangleProgress >= 3800) && (triangleProgress < 3900)) aw[triangle[(buildOrder +3) % 4]] = 255 - (triangleProgress - 3800) * 2.575; // turn off in 100ms
  return 8000; // the number of milliseconds this routine is supposed to end at
}

long     triangleBuildFast() {//  Same as Triangle build but all delays are halved.
  aw[ledPanels] = ledPanelsLow; //   LED panels are on low [unless made bright]
  long triangleProgress = (progress *2) % 4000; // how far into this build are we
  buildOrder = progress / 8000; // this line and above are only differences from triangleBuild() !!!!!!!!!!!!!!!
  if (triangleProgress < 100) aw[triangle[buildOrder % 4]] = triangleProgress * 2.575; // turn on in 100ms
  if ((triangleProgress >= 600) && (triangleProgress < 700)) aw[triangle[(buildOrder +1) % 4]] = (triangleProgress - 600) * 2.575; // turn on in 100ms
  if ((triangleProgress >= 1200) && (triangleProgress < 1300)) aw[triangle[(buildOrder +2) % 4]] = (triangleProgress - 1200) * 2.575; // turn on in 100ms
  if ((triangleProgress >= 1800) && (triangleProgress < 1900)) aw[triangle[(buildOrder  +3) % 4]] = (triangleProgress - 1800) * 2.575; // turn on in 100ms
  if ((triangleProgress >= 1900) && (triangleProgress < 2000)) aw[ledPanels] = 255;  // When all are ON, make LED panels bright.
  if ((triangleProgress >= 2000) && (triangleProgress < 2100)) aw[triangle[(buildOrder % 4)]] = 255 - (triangleProgress - 2000) * 2.575; // turn off in 100ms
  if ((triangleProgress >= 2600) && (triangleProgress < 2700)) aw[triangle[(buildOrder +1) % 4]] = 255 - (triangleProgress - 2600) * 2.575; // turn off in 100ms
  if ((triangleProgress >= 3200) && (triangleProgress < 3300)) aw[triangle[(buildOrder +2) % 4]] = 255 - (triangleProgress - 3200) * 2.575; // turn off in 100ms
  if ((triangleProgress >= 3800) && (triangleProgress < 3900)) aw[triangle[(buildOrder +3) % 4]] = 255 - (triangleProgress - 3800) * 2.575; // turn off in 100ms
  return 6000; // the number of milliseconds this routine is supposed to end at
}

long innerOverlay() {
/* LED panels on medium
 Turn on Triangle 1 and Vertex 1 at the same time. Delay 600ms
 ON Triangle2 and Vertex2
 Triangle3 and Vertex3
 Triangle4 and Vertex4
 ON Vertex 5 and CrissCross:
 At this point all lights in the mandala are ON. Make LED panels bright. 
 delay 1000ms
 repeat */
  int stage = (progress % 4000) / 600;
  if (stage == 0) allOff(); // if it's the beginning of the sequence
  aw[ledPanels] = ledPanelsMedium; // except the panels are medium
  aw[triangle1] = 255; // and the triangle
  digitalWrite(vertex1,HIGH); // and vertex1 are on
  if (stage > 0) {
    aw[triangle2] = 255;
    digitalWrite(vertex2,HIGH);
  }
  if (stage > 1) {
    aw[triangle3] = 255;
    digitalWrite(vertex3,HIGH);
  }
  if (stage > 2) {
    aw[triangle4] = 255;
    digitalWrite(vertex4,HIGH);
  }
  if (stage > 3) {
    digitalWrite(vertex5,HIGH);
    digitalWrite(crissCross,HIGH);
  }
  if (stage > 4) {
      aw[ledPanels] = 255;
  }
  return 8000; // the number of milliseconds this routine is supposed to end at
}

long     vertexSweep() {
/* Turn Vertex1 ON, then Vertex2, 3, 4, 5, CrissCross, with 600ms delay
 Leave all on for 1200ms
 Then turn off in the reverse order: CrissCross, 5, 4, 3, 2, 1.
 Beyond ON,   Repeat*/
  int stage = (progress / 600) % 13;
  if (stage == 0) {
    allOff();
    digitalWrite(vertex1,HIGH);
  }
  digitalWrite(beyondRelay,((progress / 600) / 13) & 1);

  aw[ledPanels] = ledPanelsMedium; // LED panels on medium
  if (stage > 0) digitalWrite(vertex2,HIGH);
  if (stage > 1) digitalWrite(vertex3,HIGH);
  if (stage > 2) digitalWrite(vertex4,HIGH);
  if (stage > 3) digitalWrite(vertex5,HIGH);
  if (stage > 4) digitalWrite(crissCross,HIGH);
  if (stage > 6) digitalWrite(crissCross,LOW);
  if (stage > 7) digitalWrite(vertex5,LOW);
  if (stage > 8) digitalWrite(vertex4,LOW);
  if (stage > 9) digitalWrite(vertex3,LOW);
  if (stage > 10) digitalWrite(vertex2,LOW);
  if (stage > 11) digitalWrite(vertex1,LOW);
  return 6000; // the number of milliseconds this routine is supposed to end at
}

long     vertexSweepFast() {
  return 10000; // the number of milliseconds this routine is supposed to end at  
}

long     climacticBuild() {
  return 10000000; // the number of milliseconds this routine is supposed to end at
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


