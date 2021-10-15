#include <Arduino.h>
#include <SoftwareSerial.h>
#define G 5//publicly known
#define N 23//publicly known
#define A 5//randomly chosen by alice
#define RX 4
#define TX 3
uint64_t power(uint32_t number,uint32_t exponent){
  uint64_t result =1;
  for(uint32_t i=0; i<exponent; i++)result*=number;
  return result;
}


SoftwareSerial com(RX, TX);//initialise a software serial port 
uint8_t shared_secret;
char message_from_bob[12];
void setup()
{
  delay(10000);//wait for 10 seconds
  Serial.begin(9600);
  com.begin(9600);
  Serial.println("ALICE ONLINE");
  delay(500);
  Serial.println("ALICE IS TRANSMITTING");
  delay(3000);
  com.write(power(G, A) % N);//send (g^a mod n) to bob
  Serial.println("SENT TO BOB");
  delay(500);
  Serial.println("ALICE IS RECEIVING");
  delay(500);
  while (com.available() == 0){}//while no bytes are available from BOB, WAIT 
  shared_secret = power(com.read(),A) % N;//read what bob sent(g^b mod n), raise it to a and do mod n= ((g^b mod n)^a)mod n = g^ab mod n
  Serial.println("Shared secret is " + (String)shared_secret);
  delay(1000);
}

void loop()
{
   char message_for_bob[10]= "Hello BOB";
  Serial.println("Encrypting message for bob");
  delay(500);
  for (uint8_t i = 0; i < 9; i++){//add the shared secret to all the bytes of the message by the shared secret except the null character
    message_for_bob[i] += shared_secret;
  }
  Serial.println("Sending");
  delay(1000);
  com.write(message_for_bob);//send encrypted message to bob
  delay(3000);
  Serial.println("Waiting for message from Bob");
  delay(100);
  while (com.available() != 11){}//wait for bob's message to arrive which is 11 characters(1 char=1byte) long
  for (uint8_t i = 0; i < 11; i++){
    message_from_bob[i] = com.read() - shared_secret;//store bob's message and decrypt it by substracting the shared secret
  }
  Serial.println(message_from_bob);//print bob's message
  delay(1000);
}