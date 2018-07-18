#include <usbhid.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <hidboot.h>
#include <SPI.h>
#include <EtherCard.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

#define display 0x3F

LiquidCrystal_I2C lcd(display,2,1,0,4,5,6,7,3, POSITIVE);  




//boolean flag_op = false;
byte Ethernet::buffer[300];
static uint32_t timer;
const char website[] PROGMEM = "192.168.1.10";
String codbarras;
byte multiplo = 0;
byte contagem = 0;
int contpin = 7;
int co = 2;  //tempotario

bool flag_cont;

class MyParser : public HIDReportParser {
  public:
    MyParser();
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
  protected:
    uint8_t KeyToAscii(bool upper, uint8_t mod, uint8_t key);
    virtual void OnKeyScanned(bool upper, uint8_t mod, uint8_t key);
    virtual void OnScanFinished();
};

MyParser::MyParser() {}

void MyParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  // If error or empty, return
  if (buf[2] == 1 || buf[2] == 0) return;

  for (uint8_t i = 7; i >= 2; i--) {
    // If empty, skip
    if (buf[i] == 0) continue;

    // If enter signal emitted, scan finished
    if (buf[i] == UHS_HID_BOOT_KEY_ENTER) {
      OnScanFinished();
    }

    // If not, continue normally
    else {
      // If bit position not in 2, it's uppercase words
      OnKeyScanned(i > 2, buf, buf[i]);
    }

    return;
  }
}

uint8_t MyParser::KeyToAscii(bool upper, uint8_t mod, uint8_t key) {
  // Letters
  if (VALUE_WITHIN(key, 0x04, 0x1d)) {
    if (upper) return (key - 4 + 'A');
    else return (key - 4 + 'a');
  }

  // Numbers
  else if (VALUE_WITHIN(key, 0x1e, 0x27)) {
    return ((key == UHS_HID_BOOT_KEY_ZERO) ? '0' : key - 0x1e + '1');
  }

  return 0;
}

void MyParser::OnKeyScanned(bool upper, uint8_t mod, uint8_t key) {
  uint8_t ascii = KeyToAscii(upper, mod, key);
 

 codbarras += (char)ascii;


  
}

void MyParser::OnScanFinished() {

         lcd.setCursor(0,1); // Column, line
         lcd.print(F( "OP:"));
         lcd.setCursor(3,1);
         lcd.print(codbarras);
        exit;
}

USB          Usb;
USBHub       Hub(&Usb);
HIDUniversal Hid(&Usb);
MyParser     Parser;




 //called when the client request is complete
static void my_callback (byte status, word off, word len) {
    
  int checkResp = 0;
  Ethernet::buffer[off+len] = 0;
  String fullResponse;
  fullResponse = (char*) Ethernet::buffer + off;
  
   //Serial.println(fullResponse);

   //delay(3000);

    checkResp = fullResponse.indexOf('{');
   if(checkResp != -1){
     //  fullResponse.remove(0, checkResp - 1);
     StaticJsonBuffer<200> jsonBuffer;
     JsonObject& root = jsonBuffer.parseObject(fullResponse);

      if (root.success()) {

       

     if( root["erro"].as<char*>()){

          lcd.clear();
          lcd.setCursor(4,0); // Column, line
          lcd.print(F( "ATENCAO !!!!"));
          lcd.setCursor(0,2); // Column, line
          lcd.print( root["erro"].as<char*>());
     
        delay(5000);

        limpa_var();


       
      }else{
        
        
        lcd.clear();
        lcd.setCursor(0,0); // Column, line
        lcd.print(F( "OP:"));
        lcd.setCursor(3,0);
        lcd.print( root["op"].as<char*>());
        lcd.setCursor(10,0);
        lcd.print(F( "Total:"));
        lcd.setCursor(16,0);
        lcd.print( root["qt"].as<char*>());
        lcd.setCursor(0,1); // Column, line
        lcd.print(F( "Cod:"));
        lcd.setCursor(4,1);
        lcd.print( root["pr"].as<char*>());
        lcd.setCursor(15,1);
        lcd.print(F( "UM:"));
        lcd.setCursor(18,1); 
        lcd.print( root["um"].as<char*>());
        lcd.setCursor(0,2); // Column, line
        lcd.print(F( "P:"));
        lcd.setCursor(2,2);
        lcd.print( root["qp"].as<char*>());
        lcd.setCursor(7,2);
        lcd.print(F( "S:"));
        lcd.setCursor(9,2);
        lcd.print( root["sl"].as<char*>());
        lcd.setCursor(14,2);
        lcd.print(F( "E:"));
        lcd.setCursor(16,2);
        lcd.print( root["et"].as<char*>());
        lcd.setCursor(0,3);
        lcd.print(F("|||||||"));
        lcd.setCursor(8,3);
        lcd.print(F("0"));
        lcd.setCursor(13,3);
        lcd.print(F("|||||||"));
        flag_op = true;
        multiplo = root["cx"];

     

      }

     }

   }

}



void setup () {
  Serial.begin(57600);
  Serial.println(F("\n[INICIANDO]"));
   
   pinMode(contpin, INPUT);

    pinMode(co, OUTPUT);

   
   lcd.begin (20,4);
   lcd.setBacklight(HIGH);
   lcd.clear();
   lcd.setCursor(0,1); // Column, line
   lcd.print(F("AROTUBI  COMPONENTES"));
   lcd.setCursor(6,2); // Column, line
   lcd.print(F("NO-FROST"));
   delay(3000);
   
   
   //INICIANDO REDE/////////////////////////////////////////////////////////////////////////////////////////

    const byte mymac[] PROGMEM = { 0x72,0x68,0x58,0x2D,0x34,0x36 };
    const static byte ip[]  = {192,168,0,82};
    const static byte gw[]  = {192,168,1,254};
    const static byte dns[]  = {192,168,1,222};
  

  if (ether.begin(sizeof Ethernet::buffer, mymac,8) == 0) {       // Initialize ethercard.
    Serial.println(F("ERRO - REDE"));

    lcd.setCursor(0,2);
    lcd.print(F("ERRO - REDE"));
  }
  if(!ether.staticSetup(ip, gw, dns)){
    
     lcd.setCursor(0, 2);
    lcd.print(F("ERRO - CONEXAO"));                   // the ethercard an IP address and get the gateway and
    lcd.setCursor(0,3);
    lcd.print(F("VERIFIQUE O CABO"));
    Serial.println(F("ERRO - IP"));                // DNS server addresses.
    
    }
   

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip); 
  ether.printIp("DNS: ", ether.dnsip); 

  if (!ether.dnsLookup(website)){
    Serial.println(F("DNS failed"));
  }
     ether.printIp("SRV: ", ether.hisip);






  if (Usb.Init() == -1) {
   Serial.println(F("OSC did not start."));
  }

 // delay( 200 );

  Hid.SetReportParser(0, &Parser);


//  inicia_sistema();
 

}

void loop () {

   int val = digitalRead(contpin);  // read input value
   ether.packetLoop(ether.packetReceive());

   digitalWrite(co, HIGH); //temporario

if(flag_op == false){

     if( codbarras.length() == 0) (ler_codbar());


    char cx[12];
    codbarras.toCharArray(cx,12);

 
     
  if (millis() > timer) {
    timer = millis() + 5000;
    ether.browseUrl(PSTR("/wscorpcloud/nofrost/"), cx, website, my_callback);
    ether.persistTcpConnection(true);
 }
}else{

  if( contagem == multiplo ){
    
        // aponta produção
       
           Serial.println(F("apontou produção"));
           delay(2000);
           contagem = 0;
           lcd.setCursor(8,3);
           lcd.print("0 ");
         
    
    }else{ 
         if (val == LOW) {        
    
             if (flag_cont == HIGH){

                 flag_cont = LOW;
                 conta_peca();

                   Serial.println(contagem);
                   Serial.println(multiplo);
    
           } 
           }else{
      
                flag_cont = HIGH;
    
           }
    }


}

 
}

   
void inicia_sistema(){
  
 
         lcd.clear();
         lcd.setCursor(4,1); // Column, line
         lcd.print(F( "AROTUBI LTDA"));
         lcd.setCursor(6,2); // Column, line
         lcd.print(F( "NO-FROST"));
         delay(3000);
  
  
  }

  void ler_codbar(){
       
        lcd.clear();
         lcd.setCursor(6,0); 
         lcd.print(F( "NO-FROST"));
         lcd.setCursor(0,1);  
         lcd.print(F( "OP:"));
    while( codbarras.length() != 11){
      
         Usb.Task();
         lcd.setCursor(3,1); 
         lcd.print(codbarras);
        
       } delay(3000);
    }
 void conta_peca(){

  

        contagem = contagem + 1;
       
        lcd.setCursor(8,3);
        lcd.print(contagem);

     
        
    }

   void limpa_var(){
    
        codbarras = "";
        multiplo = 0;
        contagem = 0;
        flag_op = false;
    }
    

