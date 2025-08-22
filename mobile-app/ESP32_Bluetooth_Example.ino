/*
 * ESP32 Bluetooth Navigation Receiver
 * 
 * Este código implementa um servidor Bluetooth Low Energy (BLE) no ESP32
 * que recebe instruções de navegação do aplicativo web.
 * 
 * Funcionalidades:
 * - Servidor BLE com serviço personalizado
 * - Recebe instruções de navegação via característica
 * - Exibe instruções no Serial Monitor
 * - Pode ser expandido para exibir em display OLED ou outros dispositivos
 */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UUIDs do serviço e característica
#define SERVICE_UUID        "0000ffe0-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "0000ffe1-0000-1000-8000-00805f9b34fb"

// Nome do dispositivo Bluetooth
#define DEVICE_NAME "ESP32_Navigation"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// String para armazenar a instrução recebida
String receivedInstruction = "";

// Callback para quando um dispositivo se conecta
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Dispositivo conectado!");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Dispositivo desconectado!");
    }
};

// Callback para quando uma característica é escrita
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Nova instrução recebida: ");
        
        // Processa cada caractere recebido
        for (int i = 0; i < rxValue.length(); i++) {
          char c = rxValue[i];
          
          if (c == '\n') {
            // Fim da instrução, processa
            if (receivedInstruction.length() > 0) {
              processInstruction(receivedInstruction);
              receivedInstruction = "";
            }
          } else {
            // Adiciona caractere à instrução atual
            receivedInstruction += c;
          }
        }
        
        Serial.println("*********");
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando ESP32 Navigation Bluetooth...");

  // Configura o nome do dispositivo Bluetooth
  BLEDevice::init(DEVICE_NAME);

  // Cria o servidor BLE
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Cria o serviço BLE
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Cria a característica BLE
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Adiciona descritor para notificações
  pCharacteristic->addDescriptor(new BLE2902());

  // Inicia o serviço
  pService->start();

  // Inicia a publicidade
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  
  Serial.println("Esperando conexão Bluetooth...");
  Serial.print("Nome do dispositivo: ");
  Serial.println(DEVICE_NAME);
  Serial.print("Endereço MAC: ");
  Serial.println(BLEDevice::getAddress().toString().c_str());
}

void loop() {
  // Gerencia reconexão
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("Iniciando publicidade...");
    oldDeviceConnected = deviceConnected;
  }
  
  // Se conectado, mas antes não estava
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
  
  delay(100);
}

// Função para processar a instrução recebida
void processInstruction(String instruction) {
  Serial.println("=== INSTRUÇÃO DE NAVEGAÇÃO ===");
  Serial.println(instruction);
  Serial.println("===============================");
  
  // Aqui você pode adicionar código para:
  // - Exibir em um display OLED
  // - Reproduzir áudio via alto-falante
  // - Ativar vibração
  // - Acender LEDs indicativos
  // - etc.
  
  // Exemplo: Piscar LED integrado para indicar recebimento
  digitalWrite(2, HIGH);
  delay(200);
  digitalWrite(2, LOW);
}

/*
 * INSTRUÇÕES DE CONFIGURAÇÃO:
 * 
 * 1. Carregue este código no seu ESP32
 * 2. Abra o Serial Monitor (115200 baud)
 * 3. Anote o endereço MAC exibido
 * 4. No aplicativo web, conecte via Bluetooth
 * 5. As instruções aparecerão no Serial Monitor
 * 
 * PERSONALIZAÇÕES POSSÍVEIS:
 * 
 * - Adicionar display OLED para mostrar instruções
 * - Adicionar alto-falante para áudio
 * - Adicionar vibrador para feedback tátil
 * - Adicionar LEDs coloridos para direções
 * - Implementar reconhecimento de voz
 * 
 * EXEMPLO COM DISPLAY OLED:
 * 
 * #include <Wire.h>
 * #include <Adafruit_GFX.h>
 * #include <Adafruit_SSD1306.h>
 * 
 * #define SCREEN_WIDTH 128
 * #define SCREEN_HEIGHT 64
 * #define OLED_RESET -1
 * 
 * Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 * 
 * void setup() {
 *   // ... código Bluetooth ...
 *   
 *   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
 *     Serial.println(F("Falha no display SSD1306"));
 *   }
 *   display.clearDisplay();
 *   display.setTextSize(1);
 *   display.setTextColor(SSD1306_WHITE);
 * }
 * 
 * void processInstruction(String instruction) {
 *   display.clearDisplay();
 *   display.setCursor(0,0);
 *   display.println("NAVEGACAO");
 *   display.println("----------");
 *   display.println(instruction);
 *   display.display();
 * }
 */ 