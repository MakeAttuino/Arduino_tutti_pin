/**************************************************

  Programma per testare alcune funzionalita
  utilizzando quasi tutti i PIN di Arduino UNO
  Questo Software e' fornito "cosi' com'e'" senza nessuna
  garanzia espressa o implicita, e' Freeware (ossia fornito GRATIS).
  Potete usare questo Software gratis per scopi
  personali o didattici. Un utilizzo di tipo commerciale
  e' di totale responsabilita' di chi lo commercializza.
  In nessun caso l'Autore e' da ritenersi responsabile
  per qualunque tipo di danno possa derivare dall'utilizzo
  proprio o improprio dello programma realizzato.
  Firmaware =  V.1.0
  Data modifica  =  8/3/2019
  Autore L.Mariani
  
**************************************************/

#include <SoftwareSerial.h>
#include <Tone.h>
#include <Wire.h>
#include <DS1302.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

  // Sistema
boolean debug = false;
byte old_tasto;
byte new_tasto;
boolean analog = false;
int riferimento;
int luce_value;
int analog_value;
float temp_value;

  // mappa I/O
const byte rele = 4;
const byte buzzer = 8;
const byte pwm_V = 9;
const byte led_R = 11;
const byte tasto = 12;
const byte led_vivo = 13;

const byte sens_luce = A1;
const byte TMP36 = A3;

  // display LCD seriale
#define PIN_PIXEL_rx 2
#define PIN_PIXEL_tx 3
SoftwareSerial lcd(PIN_PIXEL_rx,PIN_PIXEL_tx);
String seriale = "";
const byte lcd_clear[2]={254,81};
const byte lcd_setline1[3]={254,69,0};
const byte lcd_setline2[3]={254,69,64};
const byte lcd_setlum[3]={254,83,3};
const byte lcd_setcontr[3]={254,82,50};
//const byte lcd_baudrate[3]={254,97,4};
char store_linea1[20] = {'T','e','s','t',' ','a','l','l',' ','p','i','n',' ','A','r','d','u','i','n','o'};
char visual[20] = {};
byte lungo;
boolean visualizza = false;

// RTC DS1302
DS1302 rtc(5,6,7);
char buffer1[20];
char buffer2[20];

// Toni per buzzer
int nota[] = { NOTE_A3,
              NOTE_B3,
              NOTE_C4,
              NOTE_D4,
              NOTE_E4,
              NOTE_F4,
              NOTE_G4 };
Tone suono;

// Crepuscolare TSL2561
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 123);
float luminosita;

// Neopixel
#define PIN_PIXEL 10
#define NUMPIXELS 3
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN_PIXEL, NEO_GRB + NEO_KHZ800);
byte count_pix = 0;

//  Timer
int tempo_base = 100;       // tempo confronto di 1 secondo
unsigned long int start_time;
unsigned long int end_time;
byte counter = 0;
byte anti_rim = 0;
byte secondo = 0;           //per conteggiare i secondi

/***************************************/

void configureSensor(void)
{
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
}

void setup()
{
  // leggi matricola e parametri
  leggi_parametri();
  // inizializzo PIN_PIXEL I/O
  pinMode(rele,OUTPUT);digitalWrite(rele,LOW);
  pinMode(tasto,INPUT_PULLUP);old_tasto = digitalRead(tasto);
  pinMode(buzzer,OUTPUT);digitalWrite(buzzer,LOW);
  pinMode(pwm_V,OUTPUT);digitalWrite(pwm_V,LOW);
  pinMode(led_R,OUTPUT);digitalWrite(led_R,HIGH);
  pinMode(led_vivo,OUTPUT);digitalWrite(led_vivo,LOW);
  // inizializzo buzzer
  suono.begin(8);
  // inizializzo seriali
  Serial.begin(19200);
  lcd.begin(9600);
  // inizializzo RTC
  rtc.halt(false);
  rtc.writeProtect(false);
  // impostare la data/ora solo la prima volta poi commentare
//  rtc.setDOW(SATURDAY);        // Imposta il giorno della settimana a SUNDAY
//  rtc.setTime(16, 19, 0);     // Imposta l'ora come ora:minuti:secondi (Formato 24hr)
//  rtc.setDate(23, 3, 2019);   // Imposta la data cone giorno-mese-anno
  // inizializzo lcd
  lcd.write(lcd_clear,2);
  lcd.write(lcd_setline1,3);
  lungo = sizeof(store_linea1)/sizeof(store_linea1[0]);
  lungo = lungo - 1;
  for(byte y = 0; y <= lungo; y++)
  {
    visual[y] = ' ';
  }
  for(byte y = 0; y <= lungo; y++)
  {
    for(byte w = 0; w <= y; w++)
    {
      visual[lungo] = store_linea1[w];
      visual[(lungo+w)-y] = visual[lungo];delay(30);
    }
    lcd.write(lcd_setline1,3);
    for(byte j = 0; j <= lungo; j++)
    {
      lcd.print(visual[j]);delay(1);
    }
  }
  lcd.write(lcd_setline2,3);
  for(byte j = 0; j <= lungo; j++)
  {
    lcd.write ('.');suono.play(nota[0]);
    digitalWrite(led_R,HIGH);
    delay(100);suono.stop();delay(100);
    digitalWrite(led_R,LOW);
  }
  digitalWrite(led_R,HIGH);delay(3000);
  lcd.write(lcd_clear,2);
  // inizializzo sensore crepuscolare
  configureSensor();
  if (!tsl.begin())
  {
    if(debug == true)Serial.println("Trovato Sensore");
  }
  else
  {
    if(debug == true)Serial.println("Sensore assente?");
    while (1);
  }
  // inizializzo strip Neopixel
  strip.begin();
  strip.show(); // to 'off'
  // inizializzo timer  
  start_time = millis();
}

void loop()
{
  conta_tempo();
  gest_in();
  gest_analog();
  gest_visualizza();
}

// conteggio tempo
void conta_tempo()
{
  end_time=millis();
  if((end_time-start_time) >= tempo_base)
  {
    byte R= random(255);
    byte G= random(255);
    byte B= random(255);
    strip.setPixelColor(count_pix, strip.Color(R,G,B));
    strip.show();
    count_pix+=1;
    if(count_pix == NUMPIXELS)count_pix = 0;
    start_time = end_time;
    if(anti_rim != 0)
    {
      anti_rim-=1;
      if(anti_rim == 0)
      {
        suono.stop();
        digitalWrite(led_R,HIGH);
      }
    }
    counter+=1;
    if(counter == 10)
    {
      counter = 0;
      analog = true;
      visualizza = true;
      digitalWrite(led_vivo, !digitalRead(led_vivo));
      secondo+=1;
      if (secondo == 60 )secondo = 0;
    }
  }
}

void gest_in()// controllo stato Start
{
  if(anti_rim == 0)
  {
    new_tasto = digitalRead(tasto);
    if(new_tasto != old_tasto)
    {
      if(new_tasto == LOW)
      {
      digitalWrite(led_R,LOW);
      anti_rim = 2;suono.play(nota[5]);
      }
      old_tasto = new_tasto;
    }
  }
}

void gest_analog()
{
  if(analog == true)
  {
    analog = false;
    sensors_event_t event;
    tsl.getEvent(&event);
    luminosita = event.light;
    if (event.light)
    {
      if(debug == true)
      {
        Serial.print("Sens crepuscolo = ");
        Serial.print(event.light); Serial.println(" lux");
      }
    }
    else
    {
      if(debug == true)Serial.println("Sensor overload");
    }
    if(event.light <= 100)
    {
      digitalWrite(rele,LOW);
    }
    else
    {
      digitalWrite(rele,HIGH);
    }
    luce_value = analogRead(sens_luce);
    analogWrite(pwm_V,luce_value);
    
    //Legge il valore dalla porta analogica
    temp_value = 0;float tmp = 0;
    for(byte j = 0; j < 5; j++)
    {
      tmp = analogRead(TMP36);temp_value+=tmp;
    }
    temp_value = temp_value/5;
    //Converte il valore letto in tensione
    temp_value = (temp_value /1024.0) * 5.0;
    //Converte il valore di tensione in temperatura
    temp_value = (temp_value - .5) * 100;
  }
}

void gest_visualizza()
{
  if(visualizza == true)
  {
    if(debug == true)
    {
      Serial.print("Sens luce = ");Serial.println(luce_value,2);
      Serial.print("Temp = ");Serial.println(temp_value,2);
      // Invia giorno della settimana
      Serial.print(rtc.getDOWStr());
      Serial.print(" ");
      // Invia data
      Serial.print(rtc.getDateStr());
      Serial.print(" -- ");
      // Invia ora
      Serial.println(rtc.getTimeStr());
    }
    visualizza = false;lcd.write(lcd_clear,2);
//    lcd.write(lcd_setline1,3);
//    lcd.print(rtc.getDOWStr());
    if(new_tasto == HIGH)
    {
      lcd.write(lcd_setline1,3);
      lcd.print(rtc.getDOWStr());
      lcd.print(" ");
      lcd.print(rtc.getDateStr());
      lcd.write(lcd_setline2,3);
      lcd.print("Ora ");lcd.print(rtc.getTimeStr());
    }
    else
    {
      lcd.write(lcd_setline1,3);
      lcd.print("Luminosi = ");lcd.print(luminosita);
      lcd.write(lcd_setline2,3);
      lcd.print("Temperat = ");lcd.print(temp_value,2);lcd.print((char) 223);lcd.print("C");
    }
  }
}

// identificativo numero seriale 
void leggi_parametri()
{
  String seriale = "";
  for(byte j = 0; j <= 8; j++)
  {
    seriale = seriale + char (EEPROM.read(j));
  }
  if (debug == true)
  {
    Serial.print("Numero seriale = ");Serial.println(seriale);
  }
}
