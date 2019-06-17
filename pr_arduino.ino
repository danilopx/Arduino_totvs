#include <usbhid.h>
//#include <usbhub.h>
#include <hiduniversal.h>
#include <hidboot.h>
#include <SPI.h>
#include <EtherCard.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
//#include <avr/wdt.h>

#define display 0x3F

LiquidCrystal_I2C lcd(display, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


byte Ethernet::buffer[300];
static uint32_t timer;
const char website[] PROGMEM = "192.168.1.10";
String codbarras;
byte multiplo = 0;
byte contagem = 0;
int contpin = 7; // button de contagem temporario
int pause = 6;   // button para pausar contagem
int ledgr = 4;   // led verde
int ledrd = 5;   // led vermelho
int temp_pause = 0; // tempo de pressionamento do button de pause 5 sec

bool flag_conect = true; // flag de conexão de rede
bool flag_pause = false; // flag de parada de contagem

bool flag_cont; // flag de controle do sinal (bit) contagem

bool flag_apont = false; // flag para o apontamento de produção
bool flag_op = false;   //  flag para buscar via rest informações da Ordem de Produção
bool flag_print = false;

bool flag_menu = true;

int WhichScreen =1;   // This variable stores the current Screen number
boolean hasChanged = true;
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin


////////////////////////// inicia classe do USBHOST e função para ler codigo de barras ///////////////////////////////////////

class MyParser : public HIDReportParser {
public:
    MyParser();
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
protected:
    uint8_t KeyToAscii(bool upper, uint8_t mod, uint8_t key);
    virtual void OnKeyScanned(bool upper, uint8_t mod, uint8_t key);
    virtual void OnScanFinished();
};

MyParser::MyParser() {
}

void MyParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
    // If error or empty, return
    if (buf[2] == 1 || buf[2] == 0) return;

    for (uint8_t i = 7; i >= 2; i--) {
        // If empty, skip
        if (buf[i] == 0) continue;

        // If enter signal emitted, scan finished
        if (buf[i] == UHS_HID_BOOT_KEY_ENTER) {
            OnScanFinished();
        }            // If not, continue normally
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
    }        // Numbers
    else if (VALUE_WITHIN(key, 0x1e, 0x27)) {
        return ((key == UHS_HID_BOOT_KEY_ZERO) ? '0' : key - 0x1e + '1');
    }

    return 0;
}

void MyParser::OnKeyScanned(bool upper, uint8_t mod, uint8_t key) {
    uint8_t ascii = KeyToAscii(upper, mod, key);

    codbarras += (char) ascii;
}

void MyParser::OnScanFinished() {
}

USB Usb;
//USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
MyParser Parser;

//////////////////////////////// final classe USBHOST //////////////////////////////////////////////////////////

//////////////////////////////// inicio da função para receber a resposta rest do servidor /////////////////////
static void my_callback(byte status, word off, word len) {

    int checkResp = 0;
    int x = 0;
    Ethernet::buffer[off + len] = 0;
    String fullResponse;
    fullResponse = (char*) Ethernet::buffer + off;
    
    // Serial.println(fullResponse);


    checkResp = fullResponse.indexOf('{');
    if (checkResp != -1) {
        //  fullResponse.remove(0, checkResp - 1);
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(fullResponse);

        if (root.success()) {

            if (root["erro"].as<char*>()) {

                lcd.clear();
                lcd.setCursor(4, 0); // Column, line
                lcd.print(F("ATENCAO !!!!"));
                lcd.setCursor(0, 2); // Column, line

                if (root["erro"].as<char*>()) {
                    lcd.print(root["erro"].as<char*>());
                } else {

                    lcd.print(root["errorMessage"].as<char*>());
                }

                digitalWrite(ledrd, HIGH);
                delay(5000);

                limpa_var(); // função para limpar varaveis 



            } else {
                     

                if (atoi(root["get"].as<char*>()) == 1) {

                    lcd.clear();
                    lcd.setCursor(0, 0); // Column, line
                    lcd.print(F("OP:"));
                    lcd.setCursor(3, 0);
                    lcd.print(root["op"].as<char*>());
                    lcd.setCursor(10, 0);
                    lcd.print(F("Total:"));
                    lcd.setCursor(16, 0);
                    lcd.print(root["qt"].as<char*>());
                    lcd.setCursor(0, 1); // Column, line
                    lcd.print(F("Cod:"));
                    lcd.setCursor(4, 1);
                    lcd.print(root["pr"].as<char*>());
                    lcd.setCursor(15, 1);
                    lcd.print(F("UM:"));
                    lcd.setCursor(18, 1);
                    lcd.print(root["um"].as<char*>());
                    lcd.setCursor(0, 2); // Column, line
                    lcd.print(F("P:"));
                    lcd.setCursor(2, 2);
                    lcd.print(root["qp"].as<char*>());
                    lcd.setCursor(7, 2);
                    lcd.print(F("S:"));
                    lcd.setCursor(9, 2);
                    lcd.print(root["sl"].as<char*>());
                    lcd.setCursor(15, 2);
                    lcd.print(F("E:"));
                    lcd.setCursor(17, 2);
                    lcd.print(root["et"].as<char*>());
                    lcd.setCursor(0, 3);
                    lcd.print(F("|||||||"));
                    lcd.setCursor(9, 3);
                    lcd.print(contagem);
                    lcd.setCursor(13, 3);
                    lcd.print(F("|||||||"));
                    flag_op = true;
                    multiplo = root["cx"];
                    digitalWrite(ledgr, HIGH);

                    do{
                           digitalWrite(ledgr,LOW);
                            digitalWrite(ledrd,HIGH);
                            delay(200);
                           digitalWrite(ledgr,HIGH);
                           digitalWrite(ledrd,LOW);
                           delay(200);       
                         x += 1;  

                  } while (x < 2);
                  

                } else {

                   if (root["999"].as<char*>()){

                       flag_apont = true;
                       return;

                    }

                    if (root["100"].as<char*>()){

                    //delay(1000);
                    flag_apont = false;
                    flag_op = false;
                    contagem = 0;


                      //Serial.println(F("^XA^FO50,50^AQN,50,50^FDSAMPLE ARIALI^FS ^XZ"));

                      if(flag_print == false){

                      Serial.println(F("^XA"));
                      Serial.println(F("^PR1"));
                      Serial.println(F("^PQ1"));
                      Serial.println(F("^FO42,12^GB768,1152,3,B^FS"));
                     // Serial.println(F("^FO48,18^A0B,150,250^FR^FH_^FDQ^FS"));
                     // Serial.println(F("^FO84,72^A0B,50,50^FR^FH_^FD98^FS"));
                      Serial.println(F("^FO48,24^A0B,035,055^FR^FH_^FDAROTUBI^FS"));
                      
                      Serial.println(F("^FO48,1020^A0B,025,045^FH_^FDCodigo:^FS"));
                      Serial.println(F("^FO78,450^A0B,085,130^FH_^FD"));
                      Serial.println(root["pr"].as<char*>());
                      Serial.println(F("^FS"));
                      
                      Serial.println(F("^FO150,966^A0B,025,045^FH_^FDDescricao:^FS"));
                      Serial.println(F("^FO180,18^A0B,093,093^FH_^FD"));
                      Serial.println( root["dc"].as<char*>());
                      Serial.println(F("^FS"));
                      //Serial.println(F("^FO270,828^A0B,093,093^FH_^FD"));
                      //Serial.println( root["dc"].as<char*>());
                     // Serial.println(F("^FS"));
                      
                      Serial.println(F("^FO360,12^GB0,1152,3,B^FS"));
                      Serial.println(F("^FO462,12^GB0,1152,3,B^FS"));
                      Serial.println(F("^FO564,12^GB0,1152,3,B^FS"));
                      Serial.println(F("^FO360,168^GB204,0,3,B^FS"));
                      Serial.println(F("^FO360,450^GB204,0,3,B^FS"));
                      Serial.println(F("^FO360,606^GB204,0,3,B^FS"));
                      Serial.println(F("^FO360,822^GB204,0,3,B^FS"));
                      Serial.println(F("^FO360,930^GB204,0,3,B^FS"));
                      
                      
                      Serial.println(F("^FO372,948^A0B,095,095^FH_^FDQTDE^FS"));
                      Serial.println(F("^FO474,954^A0B,097,097^FH_^FD"));
                      Serial.println(multiplo);
                      Serial.println(F("^FS"));


                      
                      Serial.println(F("^FO372,834^A0B,045,065^FH_^FDUM^FS"));
                      Serial.println(F("^FO504,840^A0B,045,065^FH_^FD"));
                      Serial.println(root["um"].as<char*>());
                      Serial.println(F("^FS"));
                      
                      Serial.println(F("^FO372,666^A0B,045,065^FH_^FDLote^FS"));
                      Serial.println(F("^FO504,624^A0B,045,065^FH_^FD")) ;
                      Serial.println(codbarras.substring(0,6));
                      Serial.println(F("^FS"));
                      
                      
                      Serial.println(F("^FO372,462^A0B,045,065^FH_^FDCaixa^FS"));
                      Serial.println(F("^FO504,468^A0B,045,065^FH_^FD"));
                      Serial.println(root["se"].as<char*>());
                      Serial.println(F("^FS"));
                      
                      Serial.println(F("^FO372,264^A0B,045,065^FH_^FDData^FS"));
                      Serial.println(F("^FO504,180^A0B,040,060^FH_^FD"));
                      Serial.println(root["dt"].as<char*>());
                       Serial.println(F("^FS"));
                      
                      Serial.println(F("^FO372,18^A0B,045,065^FH_^FDTurno^FS"));
                      Serial.println(F("^FO504,72^A0B,050,065^FH_^FD"));
                      Serial.println(root["tn"].as<char*>());
                      Serial.println(F("^FS"));
                      
                      Serial.println(F("^FO636,72^BY3,2,108^B3B,N,108,Y,N^FD"));
                      Serial.println(root["pr"].as<char*>());
                      Serial.println("0000");
                      Serial.println(multiplo);
                      Serial.println(codbarras.substring(0,6));
                      Serial.println(root["se"].as<char*>());
                      Serial.println(F("^FS"));
                      Serial.println(F("^XZ"));


                      flag_print = true;
                      }

                      flag_apont = false;
                      flag_op = false;

                    return;
                        //emp_etiqueta()
                      
                    }
                }
            }
        }
    }
}

////////////////////////////////inicio da função  setup /////////////////////////////////////////////

void setup() {
    Serial.begin(9600);
    Serial.println(F("\n[INICIANDO]"));

    pinMode(contpin, INPUT);
    pinMode(pause, INPUT);

    pinMode(ledgr, OUTPUT);
    digitalWrite(ledgr, LOW);
    pinMode(ledrd, OUTPUT);

    lcd.begin(20, 4);
    lcd.setBacklight(HIGH);
    lcd.clear();
    lcd.setCursor(0, 0); // Column, line
    lcd.print(F("    TI - AROTUBI"));
  
    lcd.setCursor(0, 2); // Column, line
    lcd.print(F("     APONTAMENTO"));
     lcd.setCursor(0, 3); // Column, line
    lcd.print(F(" AUTO. DE PRODUCAO"));
    delay(5000);


    //INICIANDO REDE/////////////////////////////////////////////////////////////////////////////////////////

    const byte mymac[] PROGMEM = {0x72, 0x68, 0x58, 0x2D, 0x34, 0x36};
    const static byte ip[] = {192, 168, 0, 82};
    const static byte gw[] = {192, 168, 1, 254};
    const static byte dns[] = {192, 168, 1, 222};


    if (ether.begin(sizeof Ethernet::buffer, mymac, 8) == 0) { // Initialize ethercard.
        Serial.println(F("ERRO - REDE"));

        lcd.setCursor(0, 2);
        lcd.print(F("ERRO - REDE"));
    }
    if (!ether.staticSetup(ip, gw, dns)) {

        lcd.setCursor(0, 2);
        lcd.print(F("ERRO - CONEXAO")); // the ethercard an IP address and get the gateway and
        lcd.setCursor(0, 3);
        lcd.print(F("VERIFIQUE O CABO"));
        Serial.println(F("ERRO - IP")); // DNS server addresses.

    }


    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip);
    ether.printIp("DNS: ", ether.dnsip);

    if (!ether.dnsLookup(website)) {
        Serial.println(F("DNS failed"));
        flag_conect = false;
    }

    ether.printIp("SRV: ", ether.hisip);

    if (Usb.Init() == -1) {
        Serial.println(F("OSC did not start."));
    }

    // delay( 200 );

    Hid.SetReportParser(0, &Parser);


    //  inicia_sistema();


}

void loop() {

    int val = digitalRead(contpin); // read input value
    int parar = digitalRead(pause); 
    char cx[12];
    int rc;



    
    if (flag_conect == false) {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(F("ERRO - CONEXAO")); // the ethercard an IPl address and get the gateway and
        lcd.setCursor(0, 2);
        lcd.print(F("VERIFIQUE A REDE"));
        Serial.println(F("ERRO - IP")); // DNS server addresses.
        delay(5000);

    } else {

        if(flag_menu == true){
          
          menu( val,  parar);
            
          }else{
            
         ether.packetLoop(ether.packetReceive());

        //  digitalWrite(co, HIGH); //temporario

        if (flag_op == false) {

            if (codbarras.length() == 0) (ler_codbar());

            codbarras.toCharArray(cx, 12);
            
             if (millis() > timer) {
                         
                timer = millis() + 5000;
               // ether.browseUrl(PSTR("/wscorpcloud/nofrost/1/"), cx, website, my_callback);
                 ether.browseUrl(PSTR("/wscorpcloud/rampas/1/"), cx, website, my_callback);
                ether.persistTcpConnection(true);
            }
        }
        if (flag_op == true && flag_apont == false) {


           // Serial.println(val);
         //   Serial.println(flag_pause);


            if (parar == LOW) {

                delay(1000);
                temp_pause += 1;

                if (temp_pause == 5 && flag_pause == false) {

                    flag_pause = true;
                    digitalWrite(ledrd, HIGH);
                    temp_pause = 0;


                     lcd.clear();
                     lcd.setCursor(0, 1); // Column, line
                     lcd.print(F("   SISTEMA PARADO   "));
                     lcd.setCursor(0, 2); // Column, line
                     lcd.print(F("  PARA  CALIBRACAO  "));

                }

                if (temp_pause == 5 && flag_pause == true) {

                    flag_pause = false;
                    digitalWrite(ledrd, LOW);
                    temp_pause = 0;
                    flag_op = false;


                }

                if (parar == HIGH) {
                    temp_pause = 0;

                }

            }else{
              
               temp_pause = 0;
              };

            if (contagem == multiplo) {

                // aponta produção
                flag_apont = true;


            } else {


                if (val == LOW && flag_pause != true) {

                    if (flag_cont == HIGH) {

                        flag_cont = LOW;
                        conta_peca();

                       // Serial.println(contagem);
                       // Serial.println(multiplo);

                    }
                } else {

                    flag_cont = HIGH;

                }
            }
        }

        if (flag_apont == true) {

            codbarras.toCharArray(cx, 12);

           flag_print = false;
          
            if (millis() > timer) {
               timer = millis() + 10000;
               // ether.browseUrl(PSTR("/wscorpcloud/nofrost/2/"), cx, website, my_callback);
                ether.browseUrl(PSTR("/wscorpcloud/rampas/2/"), cx, website, my_callback);
                ether.persistTcpConnection(true);
                 
            }

        }

    }
    }
}

void menu(int alt, int selec ){
  
   
  if (hasChanged == true) {
   
  switch(WhichScreen) {
    case 1:
    {

      lcd.clear();
      lcd.setCursor(0,0); // Column, line
      lcd.print(F("--------Menu--------"));
      lcd.setCursor(0,1); // Column, line
      lcd.print(F(">> Ap. Auto Producao"));
      lcd.setCursor(0,2);
      lcd.print(F("   Teste Impressora"));
     
    }
      break;
   
    case 2:
      {
      lcd.clear();
      lcd.setCursor(0,0); // Column, line
       lcd.print(F("--------Menu--------"));
      lcd.setCursor(0,1); // Column, line
      lcd.print(F("   Ap. Auto Producao"));
      lcd.setCursor(0,2);
      lcd.print(F(">> Teste Impressora"));
       
      }
      break;
   
 
    }
}

    int reading = alt;
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        hasChanged = true;
        WhichScreen++;
      }
    } else {
      hasChanged = false;
    }

  lastButtonState = reading;
  if (WhichScreen > 2){
    WhichScreen = 1;
  }


  if(WhichScreen == 2){
    
     if(selec == LOW){
       
           Serial.println(F("^XA^FO50,50^AQN,50,50^FDTESTE DE IMPRESSÃO^FS ^XZ"));
           delay(5000);
        } 
    
    }
    if(WhichScreen == 1){
    
     if(selec == LOW){
       
           flag_menu = false;
        } 
    
    }
  
  
  }



void ler_codbar() {

    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print(F("LER COD BARRAS"));
    lcd.setCursor(0, 2);
    lcd.print(F("OP:"));
    while (codbarras.length() != 11) {

        Usb.Task();
        lcd.setCursor(3, 2);
        lcd.print(codbarras);

    } // delay(3000);
}

void conta_peca() {

    contagem = contagem + 1;
    lcd.setCursor(9, 3);
    lcd.print(contagem);

}

void limpa_var() {

    digitalWrite(ledrd, LOW);
    digitalWrite(ledgr, LOW);
    codbarras = "";
    multiplo = 0;
    contagem = 0;
    flag_op = false;
}
