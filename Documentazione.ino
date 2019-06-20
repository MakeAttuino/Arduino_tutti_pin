/****************************************

  Descrizione del progetto

  Il programma è costruito poco per volta
  con l'obiettivo di utilizzare turri i pin
  disponibili in Scheda Arduino UNO rev.3 SMD
  senza un particolare criterio.
  Inizio con delle funzioni semplici nel
  pilotare dei led con un pulsante, emettendo
  in parallelo un suono tramite un buzzer.
  
  Acquisire dagli ingressi analogici dei valori 
  tra fotoresistenza, sensore luminosita TSL250,
  sensore di temperatura TMP36 e in funzione
  del valore acquisito, pilotare dei led in PWM.
  Gestione di datario con un RTC (Real_Time)
  Visualizzare su display 20x2
  (Newhaven NHD-0220D3Z-FL-GBW-V3),
  tramite linea seriale aggiuntiva dei valori
  acquisiti di temperatura, data e ora.
  Gioco di luce con piccolo dischetto di
  led tipo Neopixel.
  
  Utilizzo dei pin di I/O
  pin  2  =  RX Seriale display
  pin  3  =  TX Seriale display
  pin  4  =  Rele'
  pin  5  =  RST RTC
  pin  6  =  DAT RTC
  pin  7  =  CLK RTC
  pin  8  =  Buzzer
  pin  9  =  Led verde PWM
  pin  10 =  Led strip NeoPixel
  pin  11 =  Led rosso
  pin  12 =  Pulsante
  pin  13 =  Led Arduino
  
  Utilizzo dei pin analogici
  A1  =  TSL250
  A2  =  +3,3V
  A3  =  TMP36
  A4  =  SDA TSL2561
  A5  =  SCL TSL2561
  
****************************************/
