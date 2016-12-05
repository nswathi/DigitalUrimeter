/* Include the RFID library */
#include "RFID.h"

#define SS_PIN      A2      
#define RST_PIN     D2
#define MOSI_PIN    D3     
#define MISO_PIN    D4     
#define SCK_PIN     D5   


#if defined(_USE_SOFT_SPI_)
    RFID RC522(SS_PIN, RST_PIN, MOSI_PIN, MISO_PIN, SCK_PIN);    
#else
    RFID RC522(SS_PIN, RST_PIN);                   
#endif


TCPClient client;
byte server[] = { 10,251,92,211 }; 
int PORT = 9000;
char str[20];
char num[5];

void setup()
{ 
  Serial.begin(9600);
  
#if !defined(_USE_SOFT_SPI_)
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.begin();
#endif

    if (client.connect(server, PORT)) {
        client.print("2");
		Serial.println("Connected");
	} 
	else {
		Serial.println("connection failed");          
	}
	
    RC522.init();
  	WiFi.connect();
	delay(1000);
}

void loop()
{
  uint8_t i;

  memset(str, 0, sizeof str);
 
  if (RC522.isCard())
  {
    RC522.readCardSerial();

    for(i = 0; i <= 4; i++)
    {
      if(i==4) sprintf(num,"%d",RC522.serNum[i]);
      else sprintf(num,"%d-",RC522.serNum[i]);
      strcat(str,num);
    }
    Serial.print(str);
    if(client.connected())client.print(str);
    Serial.println();
  }
  else
      Serial.println("Card NOT detected");
	
	if (!client.connected()) {
		client.connect(server, PORT);
		if (client.connect(server, PORT)) {
			Serial.println("connected");
            delay(500);
		} else {
			Serial.println("connection failed");
		}
	}
      
  delay(1000);
}