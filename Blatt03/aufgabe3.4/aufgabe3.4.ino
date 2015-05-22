#include <Servo.h>

int LEDPin = 13;


int servoOutputPin = 3;
Servo servo;
//int servoPos = 0;
int servoMin = 23;
int servoMax = 157;
//int servoAdjust = 23; // 23 - 157°

//Serial Input is saved here
char serialData[100];


void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  
  pinMode(LEDPin, OUTPUT);
  LED_off();  
  
  servo.attach(servoOutputPin);
}


void loop() {
  // put your main code here, to run repeatedly:
  
  //Got characters?
  if(0 < Serial.available()) {
	//Clear the data and read it 
    memset(serialData, '\0', 100);
    Serial.readBytesUntil('\n', serialData, 100);
    
    Serial.print("Received: ");
    Serial.println(serialData);
    
    
    
	//Parse for commands and run them
    if (strcmp(serialData, "LED_on") == 0) {
      //Run it
	  LED_on();
          Serial.println("LED_on DONE");
    }
    else if (strcmp(serialData, "LED_off") == 0) {
      //Run it
	  LED_off();
          Serial.println("LED_off DONE");
    }
    else if (strstr(serialData, "moveTo(") == serialData) {
	  char* start = NULL;
          char* ending = NULL;
	  start = strchr(serialData, '(')+1;
	  ending= strrchr(serialData, ')')-1;


	  if(start != NULL && ending != NULL && start < ending && ending[1+1] == '\0') {
		//We have found a 'valid' beginning and ending
                //The start is before the end
                //After the ending are no more characters
		
		//Check if in between the brackets are only digits
		while(start != ending) {
		  if( !isdigit(*ending)) {
		    break;
		  }
		  ending--;
		}
		
                //First check character is not covered by the loop
                //Did the loop run successfully (start == ending) ?
		if(isdigit(*start) && start == ending) {
		  //So, there are only digits
		  
		  //Convert the text to a number
		  int value = atoi(start);
		  //Run it
		  moveTo(value);
		}
		else {
		  Serial.println("moveTo-Kommando falsch. Richtig: moveTo(ZAHL), wobei ZAHL im Bereich von 23 bis 157 liegt (-5.1)");
		}
	  }
	  else {
	    Serial.println("moveTo-Kommando falsch. Richtig: moveTo(ZAHL), wobei ZAHL im Bereich von 23 bis 157 liegt (-5.0)");
	  }
    }
	else {
	  //No command found...
	  Serial.print("Kommando nicht verstanden (-1): ");
	  Serial.println(serialData);
	}
  }
}


void LED_on() {
  digitalWrite(LEDPin, HIGH);
}

void LED_off() {
  digitalWrite(LEDPin, LOW);
}

void moveTo(int angle) {
  if(servoMin <= angle && angle <= servoMax) {
    Serial.print("Servo new Pos: ");
    Serial.println(angle);
    
    servo.write(angle);
  }
  else {
	Serial.println("moveTo-Kommando falsch. angle is either to big or too small.");
  }
}
