//Proyecto Examen Parcial 2 IOT -- Sebastian Monsalve Gómez Integración de End Device TTGO(GPS) con Sensores de Humedad y Temperatura
//Se incluyen las librerías necesarias para el proyecto

#include <Arduino.h>
#include <TinyGPSplus.h>
#include <ClosedCube_HDC1080.h>
#include <WiFi.h>
#include <Wire.h>

// se instancias las lcases y se inicializan las variables
WiFiClient cliente;
const char* ssid = "UPBWifi";
const char* password = "";
ClosedCube_HDC1080 Sensor;
String state;
float MeasureT[5],MeasureH[5],PromT,PromH,Latitud,Long,H;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;

//Declaración de Función
static void smartDelay(unsigned long ms);

void setup() {
Serial.begin(115200);
Sensor.begin(0x40);
Wire.begin(21,22);//SDA,SCL
Serial1.begin(9600, SERIAL_8N1, 34, 12);
delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

// Se inicializa la máquina de estados en el estado B

state = "B";
Serial.println("Bienvenidos al Sensor de Temperatura XD el estado es:  " + state);

//Nos conectamos a la red Wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

}

void loop() {



if(state=="B"){ 
//En este estado se realiza la recolección de 5 mediciones por sensor y se guardan en un vector
Serial.println("Ha ingresado al estado: " + state);
int i;
for(i=0;i<5;i++){
MeasureT[i]= Sensor.readTemperature();
smartDelay(10);
MeasureH[i]= Sensor.readHumidity();
smartDelay(10);
}
if(i>=5) {
state="C";
}
}

if(state=="C"){
//Se promedian las mediciones y de recolecta la latitud, longitu y altura.
int i;
float sumaT=0;
float sumaH=0;
for(i=0;i<5;i++){
   smartDelay(0);
 sumaT=MeasureT[i]+sumaT;
 sumaH=MeasureH[i]+sumaH;
}
PromT= sumaT/5;
PromH= sumaH/5;
Latitud= gps.location.lat();
Long= gps.location.lng();
H= gps.altitude.meters();

state="D";
}

if(state=="D"){
  Serial.println("Ha ingresado al estado: " + state);
  Serial.println("La temperatura medida es: " + String(PromT)+ "La Humedad Medida es: "+ String(PromH )+ "La posicion global es: "+ String(Latitud )+", "+String(Long )+", "+String(H ));
 
 String message = "{\"id\": \"point07\", \"lat\":"+ String(Latitud )+ " , \"lon\": " +String(Long )+ ", \"temperatura\" : "+String(PromT)+", \"humedad\": "+String(PromH )+"}";
 
  // Internet Transmisión de la información en formato Json  http://10.38.32.137/update_data
  if(cliente.connect("10.38.32.137",80)){
cliente.println("POST /update_data HTTP/1.1");
cliente.println("Host: 10.38.32.137");
cliente.println("Content-Type: application/json");
cliente.println("Content-Length: "+String(message.length()));
cliente.println("");
cliente.println(message);
delay(500);
while(cliente.available()){
  String line = cliente.readStringUntil('\n');
  Serial.println(line);
}
}else{ Serial.println("No conectó con el Server");}
  
  
  state="E";
}

if(state=="E"){
  //SmartDelay 10 Segundos
  Serial.println("Ha ingresado al estado: " + state);
  for(int i=0;i<20;i++)
  {
  smartDelay(500);
  }
  state="B";
}
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}