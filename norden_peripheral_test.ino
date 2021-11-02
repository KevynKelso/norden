/* Norden Peripheral Test Program
 * Jadon Wilson 2020
 * The purpose of this program is to test the Norden functionality
 * This can read the attached rotary encoders and drive the two stepper motors. Theoretically, we can implement PWM and timers to finely control the motors and know where they are.
 * Also, we should be able to communicate with a main computer over a serial terminal.
 *
 *
 *
 *  digitalWrite(MotorDrive1)=1 Turns on a stepper motor
 *  digitalWrite(MotorDrive2)=1 Turns on the other stepper motor
 *  M1Dir and M2Dir determine stepper motor direction
 *
 *
 */

//Constants For Encoders
const byte encoderpin1 = 2;
const byte encoderpin2 = 3;
const byte encoderpin3 = 4;
const byte encoderpin4 = 5;

const byte encoderdir1 = 6;
const byte encoderdir2 = 7;
const byte encoderdir3 = 8;
const byte encoderdir4 = 11;

//Current Encoder Count
int encoder_counter1=0;
int encoder_counter2=0;
int encoder_counter3=0;
int encoder_counter4=0;



//Constants For Motors
int MotorDrive1=9;
int MotorDrive2=10;
int M1Dir=18;
int M2Dir=17;

//Constants For Limit Switches
int Lim1=12;
int Lim2=19;


void setup() {
  // put your setup code here, to run once:
  //Motor Drive Pins are Outputs
   pinMode(MotorDrive1, OUTPUT);
   pinMode(MotorDrive2, OUTPUT);
   pinMode(M1Dir, OUTPUT);
   pinMode(M2Dir, OUTPUT);

   pinMode(encoderpin1, INPUT);
   pinMode(encoderpin2, INPUT);
   pinMode(encoderpin3, INPUT);
   pinMode(encoderpin4, INPUT);


   pinMode(encoderdir1, INPUT);
   pinMode(encoderdir2, INPUT);
   pinMode(encoderdir3, INPUT);
   pinMode(encoderdir4, INPUT);


   digitalWrite(MotorDrive1,LOW);
   digitalWrite(MotorDrive2,LOW);
   digitalWrite(M1Dir,HIGH);
   digitalWrite(M2Dir,HIGH);

   //We read the rotary encoders using interrupts
   attachInterrupt(digitalPinToInterrupt(encoderpin1), encoder1update, RISING);
   attachInterrupt(digitalPinToInterrupt(encoderpin2), encoder2update, RISING);
   attachInterrupt(digitalPinToInterrupt(encoderpin3), encoder3update, RISING);
   attachInterrupt(digitalPinToInterrupt(encoderpin4), encoder4update, RISING);

   //I attempt to stop the motors from turning when we hit one of the two limit switches
   attachInterrupt(digitalPinToInterrupt(Lim1), stopmotor, RISING);
   attachInterrupt(digitalPinToInterrupt(Lim2), stopmotor, RISING);

   //Begin Serial Terminal
   Serial.begin(57600);
}

void loop() {
  // put your main code here, to run repeatedly:

  //Testing rotary decoding
  //Serial.println("Counter1 is");
  //Serial.println(encoder_counter1);

  //Serial.println("Counter2 is");
  //Serial.println(encoder_counter2);

 // delay(400);


   digitalWrite(M1Dir,HIGH);
   digitalWrite(M2Dir,HIGH);
   digitalWrite(MotorDrive1,HIGH);//the one on the left when looking at the bombsight from the front
   //digitalWrite(MotorDrive2,HIGH);
   delay(500);
   digitalWrite(MotorDrive1,LOW);
    digitalWrite(MotorDrive2,HIGH);
   digitalWrite(M2Dir,LOW);

   delay(500);
   digitalWrite(MotorDrive2,LOW);



   //digitalWrite(M1Dir,HIGH);
   //digitalWrite(M2Dir,LOW);


}

void stopmotor(){

   //Instead of just stopping both motors I need to make them go the opposite direction for a little while so they stop pressing the limit switch. It kind of works as is.
   digitalWrite(MotorDrive1,LOW);
   digitalWrite(MotorDrive2,LOW);
}

void encoder1update() {
  //We've detected that an encoder has moved. We will increment based on its direction
  if(digitalRead(encoderdir1)==0)
  {
    encoder_counter1++;
    Serial.println("A");
  }
  else
  {
    encoder_counter1--;
    Serial.println("AA");
  }
}


void encoder2update() {
  //We've detected that an encoder has moved. We will increment based on its direction
  if(digitalRead(encoderdir2)==0)
  {
    encoder_counter2++;
    Serial.println("AAAAA");
  }
  else
  {
    encoder_counter2--;
    Serial.println("AAAAAA");
  }
}


void encoder3update() {
  //We've detected that an encoder has moved. We will increment based on its direction
  if(digitalRead(encoderdir3)==0)
  {
    encoder_counter3++;
  }
  else
  {
    encoder_counter3--;
  }
}


void encoder4update() {
  //We've detected that an encoder has moved. We will increment based on its direction
  if(digitalRead(encoderdir4)==0)
  {
    encoder_counter4++;
  }
  else
  {
    encoder_counter4--;
  }
}


