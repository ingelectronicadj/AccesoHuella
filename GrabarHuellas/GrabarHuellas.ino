/************************************************* **
   Este es un esquema de ejemplo para nuestro sensor óptico de huellas dactilares

   Diseñado específicamente para trabajar con el Adafruit BMP085 Breakout
   ----> Http://www.adafruit.com/products/751
  *************************************************/

#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

uint8_t id;

uint8_t getFingerprintEnroll();

// Software serial for when you dont have a hardware serial port
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// On Leonardo/Micro/Yun, use pins 8 & 9. On Mega, just grab a hardware serialport 
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// On Leonardo/Micro or others with hardware serial, use those! #0 is green wire, #1 is white
//Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);


void setup()  
{
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(500);
  
  Serial.begin(9600);
  Serial.println("Registro de nueva huella dactilar");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Registre su Huella dactilar");
  } else {
    Serial.println("El sensor no opera correctamente, verifique conexiones");
    while (1);
  }
}

uint8_t readnumber(void) {
  uint8_t num = 0;
  boolean validnum = false; 
  while (1) {
    while (! Serial.available());
    char c = Serial.read();
    if (isdigit(c)) {
       num *= 10;
       num += c - '0';
       validnum = true;
    } else if (validnum) {
      return num;
    }
  }
}

void loop()                     // run over and over again
{
  Serial.println("Listo para registrar una huella digital! Por favor escriba el ID # que desea asociar a la huella...");
  id = readnumber();
  Serial.print("Almacenando ID #");
  Serial.println(id);
  
  while (!  getFingerprintEnroll() );
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Esperando una nueva huella #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Captura realizada");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicacion");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Error en captura de imagen");
      break;
    default:
      Serial.println("Error Desconocido");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Captura de imagen convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imagen con muchas impurezas");
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
      Serial.println("Error Desconocido");
      return p;
  }
  
  Serial.println("Retire la huella");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Coloque la misma huella de nuevo");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen Capturada");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicacion");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Error en la imagen");
      break;
    default:
      Serial.println("Error desconocido");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen procesada");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imagen con muchas impurezas");
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
      Serial.println("Error Desconocido");
      return p;
  }
  
  // OK converted!
  Serial.print("Creación de modelo dactilar para #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Almacenamiento exitoso");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicacion");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Las huellas dactilares no coinciden");
    return p;
  } else {
    Serial.println("Error Desconocido");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Almacenado!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicacion");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("No se pudo almacenar en esa ubicación");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error escribiendo en la flash del sensor");
    return p;
  } else {
    Serial.println("Error Desconocido");
    return p;
  }   
}
