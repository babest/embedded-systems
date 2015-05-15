#include <Servo.h>

#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) ((A),STRINGIZE_NX(A))


int servoOutputPin = 3;
Servo servo;
//int servoPos = 0;
int servoMax = 180;
//int servoAdjust = 23; // 23 - 157°

//Serial Input is saved here
char serialData[100];


void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  
  servo.attach(servoOutputPin);
}


void loop() {
  // put your main code here, to run repeatedly:
  
  //Got characters?
  if(0 < Serial.available()) {
	//Read them 
    Serial.readBytesUntil('\n', serialData, 100);
    
	//Parse for commands and run them
    if (strstr(serialData, "LED_on") != NULL) {
      //Run it
	  LED_on();
    }
    else if (strstr(serialData, "LED_off") != NULL) {
      //Run it
	  LED_off();
    }
    else if (strstr(serialData, "moveTo") != NULL) {
	  char* start, end;
	  start = strchr(serialData, '(')+1;//TODO: recheck if +1
	  end   = strchr(serialData, ')')-1;//TODO: recheck if -1
	  
	  if(start != NULL && end != NULL && start < end) {
		//We have found a 'valid' beginning and end
		
		//Check if in between the brackets are only digits
		while(start != end) {
		  if( !isdigit(end[0])) {
		    break;
		  }
		  end--;
		}
		
		if(start == end) {
		  //So, there are only digits
		  
		  //Convert the text to a number
		  int value = atoi(start);
		  //Run it
		  moveTo(value);
		}
		else {
		  Serial.println("moveTo-Kommando falsch. Richtig: moveTo(ZAHL), wobei ZAHL im Bereich von 0 bis " STRINGIZE(servoMax) " liegt (-5.1)");
		}
	  }
	  else {
	    Serial.println("moveTo-Kommando falsch. Richtig: moveTo(ZAHL), wobei ZAHL im Bereich von 0 bis " STRINGIZE(servoMax) " liegt (-5.0)");
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
  if(0 <= angle && angle <= servoMax) {
    servo.write(angle);
  }
  else {
	Serial.println("moveTo-Kommando falsch. angle is either to big or too small.");
  }
}
