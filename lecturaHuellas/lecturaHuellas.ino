#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

int getFingerprintIDez();

// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// On Leonardo/Micro or others with hardware serial, use those! #0 is green wire, #1 is white
//Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);

void setup()  
{
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  
  Serial.begin(9600);
  Serial.println("Inicia deteccion");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Sensor de Huellas Dactilares encontrado!");
  } else {
    Serial.println("Revice conexiones, el sensor no opera correctamente");
    while (1);
  }
  Serial.println("A la espera de una huella...");
}

void loop()                     // run over and over again
{
  getFingerprintIDez();
  digitalWrite(12,HIGH);
  delay(50);            //don't ned to run this at full speed.
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen Detectada");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("Huella no detectada");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicacion");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Error en la imagen");
      return p;
    default:
      Serial.println("Error Desconocido");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen procesada");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imagen distorcionada, limpie el lector");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicacion");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("No se pudo encontrar características de huellas dactilares");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("No se pudo encontrar características de huellas dactilares");
      return p;
    default:
      Serial.println("Error desconocido");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Se encontro una coincidencia!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicacion");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("No se encuentran coincidencias con las huellas almacenadas");
    return p;
  } else {
    Serial.println("Error desconocido");
    return p;
  }   
  
  // found a match!
  Serial.print("Huella encontrada ID #"); Serial.print(finger.fingerID); 
  Serial.print(" con nivel de confianza de "); Serial.println(finger.confidence); 
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  digitalWrite(12,LOW); //Led rojo apagado
  digitalWrite(11,HIGH); //Led verde encendido
  delay(1000);
  digitalWrite(11,LOW);//Led verde apagado
  delay(1000);
  digitalWrite(12,HIGH);//Led rojo apagado
  Serial.print("Huella dactilar encontrada con el ID #"); Serial.print(finger.fingerID); 
  Serial.print(" con nivel de confianza de "); Serial.println(finger.confidence);
  return finger.fingerID; 
}
