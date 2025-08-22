/*
  Firmware para ESP32 - Receptor de Navegação Wi-Fi

  Este código cria um servidor web em um ESP32 que aguarda por instruções de 
  navegação enviadas pelo aplicativo. As instruções são recebidas via HTTP POST
  em formato JSON.

  COMO USAR:
  1. Instale a biblioteca "ArduinoJson" através do Library Manager da IDE do Arduino.
     (Sketch -> Include Library -> Manage Libraries... -> procure por "ArduinoJson")
  2. Altere as variáveis `ssid` e `password` abaixo com as credenciais da sua rede Wi-Fi.
  3. Carregue o código para o seu ESP32.
  4. Abra o Serial Monitor com a velocidade (baud rate) de 115200.
  5. Anote o endereço IP que aparecer no Serial Monitor.
  6. Insira este endereço IP na variável `ESP32_IP_ADDRESS` no arquivo `App.tsx` do aplicativo.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// ##################################################################
// # CONFIGURAÇÃO DA SUA REDE WI-FI LOCAL                           #
// ##################################################################
const char* ssid = "NOME_DA_SUA_REDE_WIFI";       // <-- COLOQUE O NOME DA SUA REDE AQUI
const char* password = "SENHA_DA_SUA_REDE_WIFI"; // <-- COLOQUE A SENHA DA SUA REDE AQUI
// ##################################################################


WebServer server(80); // O servidor web escutará na porta 80

// --- Opcional: Bibliotecas para Display ---
// Descomente as linhas abaixo se for usar um display OLED, por exemplo.
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// ------------------------------------------

void handleNavigate() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body not received");
    return;
  }
  
  String body = server.arg("plain");
  
  Serial.println("Recebida nova instrução:");
  Serial.println(body);

  // Usando ArduinoJson para extrair a instrução
  StaticJsonDocument<256> doc; // Reduzido o tamanho, já que esperamos um JSON menor
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    Serial.print(F("Falha no deserializeJson(): "));
    Serial.println(error.c_str());
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }

  // Mudança aqui: pegamos "instruction" em vez de "instructions"
  const char* instruction = doc["instruction"]; 

  if (instruction == nullptr) {
    server.send(400, "text/plain", "JSON is missing 'instruction' key");
    return;
  }

  Serial.print("Instrução processada: ");
  Serial.println(instruction);


  // Limpa o display antes de mostrar a nova instrução
  // display.clearDisplay(); 
  // display.setCursor(0,0);
  
  // --- Opcional: Enviar para o Display ---
  // Aqui você pode adicionar o código para mostrar a instrução no seu display
  // Exemplo para display OLED:
  // display.println(instruction);
  // display.display(); // Mostra o conteúdo no display
  // ---------------------------------------

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

// Nova função para lidar com as requisições "preflight" do CORS
void handleCorsPreflight() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(204);
}

void setup() {
  Serial.begin(115200);
  
  // --- Opcional: Inicialização do Display ---
  // if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
  //   Serial.println(F("Falha ao iniciar display SSD1306"));
  //   for(;;);
  // }
  // display.clearDisplay();
  // display.setTextSize(1);
  // display.setTextColor(WHITE);
  // display.setCursor(0,0);
  // display.println("Iniciando...");
  // display.display();
  // ------------------------------------------

  // MUDANÇA: Voltando para o modo de cliente Wi-Fi
  Serial.println("\nConectando ao Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado com sucesso!");
  Serial.print("Endereço IP do ESP32: ");
  Serial.println(WiFi.localIP());

  // Definindo a rota do servidor
  server.on("/navigate", HTTP_POST, handleNavigate);

  // Adicionando a nova rota para o CORS Preflight
  server.on("/navigate", HTTP_OPTIONS, handleCorsPreflight);

  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  server.handleClient();
} 