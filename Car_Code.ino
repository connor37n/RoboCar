#include <ECE3.h>

uint16_t sensorValues[8]; // right -> left, 0 -> 7
double weightValues[8];

const int left_nslp_pin=31; // nslp ==> awake & ready for PWM
const int left_dir_pin=29;
const int left_pwm_pin=40;
const int right_nslp_pin=11; 
const int right_dir_pin=30;
const int right_pwm_pin=39;

const double kp = 0.015;   // Proportional error constant.
const double kd = 0.130;   // Derivative error constant.
const int minSpd = 50;     // Minimum Speed for each wheel.
const int maxSpd = 110;    // Maximum Speed for each wheel.
const int rawMax = 19100;  // Threshold needed to complete the turnaround.

int leftSpd = 40;          // Initial Speed of the left wheel.
int rightSpd = 40;         // Initial Speed of the right wheel.
double sum = 0;            // Sum of weighted sensor values (error).
double oldSum = 0;         // Last recorded sum value for calculating the derivative.
double difference = 0;     // Derivative error.
double rawSum[] = {0,0,0}; // Sum of raw sensor values.
int j = 0;                 // Index for Turnaround array.
bool turnAround = false;   // False when the car hasn't turned around yet, true after it has.
int odometer;              // Cumulative average encoder count for both wheels.

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////

void setup() {
// put your setup code here, to run once:

  pinMode(left_nslp_pin,OUTPUT);
  pinMode(left_dir_pin,OUTPUT);
  pinMode(left_pwm_pin,OUTPUT);
  pinMode(right_nslp_pin,OUTPUT);
  pinMode(right_dir_pin,OUTPUT);
  pinMode(right_pwm_pin,OUTPUT);
  digitalWrite(left_dir_pin,LOW);
  digitalWrite(left_nslp_pin,HIGH);
  digitalWrite(right_dir_pin,LOW);
  digitalWrite(right_nslp_pin,HIGH);

  pinMode(BLUE_LED,OUTPUT);
  digitalWrite(BLUE_LED,LOW);
  
//  from IR sensor code example
  ECE3_Init();
  delay(2000);
}
 
void loop() {
// put your main code here, to run repeatedly: 
  
  ECE3_read_IR(sensorValues);
// Weight Values based on NEW Scale.
  weightValues[0] = -1.875 * sensorValues[0];
  weightValues[1] = -1.750 * sensorValues[1];
  weightValues[2] = -1.500 * sensorValues[2];
  weightValues[3] = -1.000 * sensorValues[3];
  weightValues[4] =  1.000 * sensorValues[4];
  weightValues[5] =  1.500 * sensorValues[5];
  weightValues[6] =  1.750 * sensorValues[6];
  weightValues[7] =  1.875 * sensorValues[7];


// Add up all the weighted values.
  oldSum = sum;
  sum = 0;
  for (int i = 0; i < 8; i++) 
      sum = sum + weightValues[i];
  difference = sum - oldSum;
  
//  Proportional Error Response.
//  If sum is positive (right of track).
//  If sum is negative (left of track). 
   rightSpd = rightSpd + sum * kp; // the larger kp is, the more quickly it accelerates.
   leftSpd = leftSpd - sum * kp;   // '-' because sum returns a negative integer when we need faster left.

//  Derivative Error Response
   rightSpd = rightSpd + difference * kd; 
   leftSpd = leftSpd - difference * kd;   

  if (leftSpd < minSpd)
    leftSpd = minSpd;
  if (rightSpd < minSpd)
    rightSpd = minSpd;
  if (rightSpd > maxSpd)
    rightSpd = maxSpd;
  if (leftSpd > maxSpd)
    leftSpd = maxSpd;
  analogWrite(left_pwm_pin,leftSpd); 
  analogWrite(right_pwm_pin,rightSpd);
    
// TURN AROUND CODE: 365 pulses per rotation, 5.7 cm circumference*
  if (turnAround == false) {        // If car hasn't completed U-Turn.
      for (int i = 0; i < 8; i++)   // Collect the Raw Sum for this loop.
        rawSum[j] = rawSum[j] + sensorValues[i];
      if (rawSum[j] < rawMax) {     // If this loop's sum isn't large enough...
        for (int i = 0; i < 3; i++) // Clear this array...
          rawSum[i] = 0;
        j = 0;                      // And restart collecting data at the first element.
      }
      else
        j++;                        // Collect data for next array element on next loop.
        
      if (rawSum[0] > rawMax && rawSum[1] > rawMax && rawSum[2] > rawMax) { // raw sensor value when pitch black is 2400, 2000 * 8 = 17600
        //sets speed of wheels equal.
        analogWrite(left_pwm_pin,160); 
        analogWrite(right_pwm_pin,160);
        //reverses direction of one wheel causing donuts.
        digitalWrite(left_dir_pin,HIGH);
        delay(350);
        digitalWrite(left_dir_pin,LOW);
        turnAround = true;
     }
  }

  odometer = (getEncoderCount_left() + getEncoderCount_right())/2;
  if (odometer >= 15000)
    digitalWrite(BLUE_LED,HIGH);
}
