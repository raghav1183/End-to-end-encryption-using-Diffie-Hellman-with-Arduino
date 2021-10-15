#include <Arduino.h>
#include <SoftwareSerial.h>
#define G 5//publicly known
#define N 23//publicly known
#define B 6//randomly chosen by bob
#define RX 4
#define TX 3
uint64_t power(uint32_t number,uint32_t exponent){
  uint64_t result =1;
  for(uint32_t i=0; i<exponent; i++)result*=number;
  return result;
}
SoftwareSerial com(RX, TX);//initialise a software serial port 
uint8_t shared_secret;
char message_from_alice[10];
void setup()
{
  delay(10000);//wait for 10 seconds
  Serial.begin(9600);
  com.begin(9600);
  Serial.println("BOB ONLINE");
  delay(500);
  Serial.println("BOB IS RECEIVING");
  delay(500);
  while (com.available() == 0){}//while no bytes are available from ALICE, WAIT 
  shared_secret = power(com.read(),B) % N;//read what ALICE sent(g^a mod n), raise it to b and do mod n = ((g^a mod n)^b)mod n = g^ab mod n
  Serial.println("Shared secret is " + (String)shared_secret);
  delay(1000);
  Serial.println("BOB IS TRANSMITTING");
  delay(3000);
  com.write(power(G, B) % N);//send g^b mod N to alice
  Serial.println("SENT TO ALICE");
  delay(1000);
}

void loop()
{
  while (com.available() != 9){}//wait for alice's message to arrive which is 9 characters(1 char=1byte) long
  for (uint8_t i = 0; i < 9; i++){
    message_from_alice[i] = com.read() - shared_secret;//decrypt alice's message by subtracting the shared secret
  }
  Serial.println(message_from_alice);
  delay(3000);
  char message_for_alice[12] = "Hello Alice";
  Serial.println("Encrypting me for ALICE");
  delay(1000);
  for (uint8_t i = 0; i < 11; i++)//add the shared secret to all the bytes of the message except the null character
  {
    message_for_alice[i] += shared_secret;
  }
  Serial.println("Sending");
  delay(1000);
  com.write(message_for_alice);
  delay(1000);
}
