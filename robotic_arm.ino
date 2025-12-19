#include <Servo.h>
//untrasonic sensor
#define TRIG_PIN 3
#define ECHO_PIN 4
#define GREEN_LED 8
#define RED_LED 7
//end ultrasonic sensor

Servo servo1;  // controlled by w/s
Servo servo2;  // controlled by q/a
Servo servo3; //controlled by e/d

int angle1 = 170;  // servo1 starting position
int angle2 = 0;    // servo2 starting position
int angle3 = 170; // servo3 starting possition


const int START1 = 170;
const int START2 = 0;
const int START3 = 170;

const int GRIP_OPEN = 170;
const int GRIP_CLOSED = 30;   
const float GRAB_DISTANCE_MIN = 4.0;
const float GRAB_DISTANCE_MAX = 8.0;

bool objectGrabbed = false; 


void close_gripper() {
  for (int i = angle3; i >= GRIP_CLOSED; i--) {
    servo3.write(i);
    delay(10);
  }
  angle3 = GRIP_CLOSED;
}

void lift_grabbed_item() {
  // Servo1
  for (int i = angle1; i <= START1; i++) {
    servo1.write(i);
    delay(10);
  }
  for (int i = angle1; i >= START1; i--) {
    servo1.write(i);
    delay(10);
  }
  angle1 = START1;

  // Servo2
  for (int i = angle2; i <= START2; i++) {
    servo2.write(i);
    delay(10);
  }
  for (int i = angle2; i >= START2; i--) {
    servo2.write(i);
    delay(10);
  }
  angle2 = START2;
}


void setup() {
  Serial.begin(9600);

  servo1.attach(5);  // pin 6
  servo2.attach(6);  // pin 7
  servo3.attach(9);  //pin 9

  servo1.write(angle1);
  servo2.write(angle2);
  servo3.write(angle3);

  Serial.println("Controls:");
  Serial.println("w = +10°, s = -10° for servo1");
  Serial.println("q = +10°, a = -10° for servo2");
  Serial.println("e = +10°, d = -10° for servo3");
  Serial.println("r = sweep servo3 down 180->0");
  Serial.println("f = sweep servo3 up 0->180");
  Serial.println("1 = reset all servos to starting positions");
  Serial.println("2 = reset all servos to starting positions except end effector");

  //ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  //end ultrasonic sensor

}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    // Servo1 controls (w/s)
    if (c == 'w') {
      angle1 += 10;
      if (angle1 > 170) angle1 = 170;
    }
    if (c == 's') {
      angle1 -= 10;
      if (angle1 < 0) angle1 = 0;
    }

    // Servo2 controls (q/a)
    if (c == 'q') {
      angle2 += 10;
      if (angle2 > 90) angle2 = 90;
    }
    if (c == 'a') {
      angle2 -= 10;
      if (angle2 < 0) angle2 = 0;
    }
    if (c == 'e') {
      angle3 +=10;
      if (angle3 > 170) angle3 = 170;
    }
    if (c == 'd') {
      angle3 -= 10;
      if (angle3 <0) angle3 = 0;
    }

        if (c == 'r') {
      for (int i = 170; i >= 0; i--) {
        servo3.write(i);
        delay(10);  // adjust for speed
      }
      angle3 = 0;
    }
    if (c == 'f') {
      for (int i = 0; i <= 170; i++) {
        servo3.write(i);
        delay(10);
      }
      angle3 = 170;
    }

        // Reset all servos to starting positions
    if (c == '1') {
      // Servo1
      for (int i = angle1; i <= START1; i++) {
        servo1.write(i);
        delay(10);
      }
      for (int i = angle1; i >= START1; i--) {
        servo1.write(i);
        delay(10);
      }
      angle1 = START1;

      // Servo2
      for (int i = angle2; i <= START2; i++) {
        servo2.write(i);
        delay(10);
      }
      for (int i = angle2; i >= START2; i--) {
        servo2.write(i);
        delay(10);
      }
      angle2 = START2;

      // Servo3
      for (int i = angle3; i <= START3; i++) {
        servo3.write(i);
        delay(10);
      }
      for (int i = angle3; i >= START3; i--) {
        servo3.write(i);
        delay(10);
      }
      angle3 = START3;
    }

            // Reset all servos except end effector to starting positions
    if (c == '2') {
      // Servo1
      for (int i = angle1; i <= START1; i++) {
        servo1.write(i);
        delay(10);
      }
      for (int i = angle1; i >= START1; i--) {
        servo1.write(i);
        delay(10);
      }
      angle1 = START1;

      // Servo2
      for (int i = angle2; i <= START2; i++) {
        servo2.write(i);
        delay(10);
      }
      for (int i = angle2; i >= START2; i--) {
        servo2.write(i);
        delay(10);
      }
      angle2 = START2;
    }


    // Apply positions
    servo1.write(angle1);
    servo2.write(angle2);
    servo3.write(angle3);

    // Print current angles
    Serial.print("Servo1: "); Serial.print(angle1);
    Serial.print("Servo2: "); Serial.print(angle2);
    Serial.print("Servo3: "); Serial.println(angle3);
  }



  //ultrasonic sensor
  long duration;
  float distance_cm;

  // Send ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read pulse (30ms timeout)
  duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    // No echo received -> Out of range
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);


  } else {
    // Calculate distance in cm
    distance_cm = duration * 0.0343 / 2;


    if (distance_cm >= GRAB_DISTANCE_MIN && distance_cm <= GRAB_DISTANCE_MAX) {
      // Object in range 
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);

      if (!objectGrabbed){
        Serial.println("Object detected...Begins grabbing");

        close_gripper();
        delay(300);
        lift_grabbed_item();
        delay(300);

        objectGrabbed = true;

      }
      
    } else {
      // Object out of range 
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      objectGrabbed = false;
    }
  }

  delay(500); // Wait 0.5s before next measurement
  //end ultrasonic sensor

}
