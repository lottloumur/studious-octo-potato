#include <SoftwareSerial.h>

#define RX 10                       //Assigns the RX and TX pins of the module
#define TX 11

String AP = "W&JOpen";              // Wifi name
String PASS = "";                   // Wifi password

String API = "9T31INF5NHG2PLNJ";    // API Key
String HOST = "api.thingspeak.com"; //where we are sending the information 
String PORT = "80";                 // integer, remote port 
String field = "field1";            // What field the info will be written to on the channel

int countTrueCommand;
int countTimeCommand; 
boolean found = false;              // Can be assigned to be true or false
int valSensor = 1;

int pushButton = 2;                 // pin that the button is linked to

int buttonState = 0;                // state of button initial
int prevButtonState = 0;            // the last state of the button
                                    // both used to notice the change when button is pushed

SoftwareSerial esp8266(RX,TX);      // grabbing the module packages

void setup() {                      // put your setup code here, to run once:

 Serial.begin(9600);                // Baude rate of arduino ??
 esp8266.begin(115200);             // baude rate for the module
 sendCommand("AT",5,"OK");          // sending AT command through serial monitor 
 sendCommand("AT+CWMODE=1",5,"OK"); // CWMode - wifi mode  1- station mode (client)
 sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");   // Connect to wifi

 pinMode(pushButton,INPUT);         // button
}

void loop() {                       // put your main code here, to run repeatedly:

  
  buttonState = digitalRead(pushButton);
  if(buttonState == 0 && prevButtonState == 1){
    // transfer data to TS
    
    String getData = "GET https://api.thingspeak.com/update?api_key="+ API +"&"+ field +"="+ String(random()%100);//String(valSensor);
    //what is sent through the connection to thingspeak channel
 
    sendCommand("AT+CIPMUX=1",5,"OK");                                      //enables multiple connections
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");     //opens communication
    sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");         //how many characters are being sent
    esp8266.println(getData);delay(1500);countTrueCommand++;                //printing what is being sent
    sendCommand("AT+CIPCLOSE=0",5,"OK");                                    // closes connection
  }

  //Serial.println(buttonState);
  prevButtonState = buttonState;  
  delay(100);

}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("SUCCESS");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("FAIL");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
}
