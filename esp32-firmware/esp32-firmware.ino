// =======================================================================
// ESP32-S3 - Navegação via BLE com Display OLED SSD1309 (U8g2lib)
// - Usa pinagem personalizada para o display
// - Usa a biblioteca U8g2 para máxima compatibilidade
// - Aceita JSON ou "direcao|mensagem" via característica BLE WRITE
// - Versão Revisada com quebra de linha automática de texto
// =======================================================================

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
// =======================================================================
// ESP32-S3 - Navegação via BLE + IA com Microfone INMP411
// - Usa pinagem personalizada para o display OLED SSD1309
// - Integra microfone INMP411 via I2S para captura de áudio
// - Botão para ativação da IA no pino 1
// - Mantém funcionalidade de navegação via BLE
// - Versão completa com IA e navegação
// =======================================================================

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <driver/i2s.h>

// =======================================================================
// CONFIGURAÇÕES DE PINOS
// =======================================================================

// Display OLED SSD1309
#define OLED_CLK   36
#define OLED_MOSI  35
#define OLED_CS    47
#define OLED_DC    21
#define OLED_RST   48

// Microfone INMP411 - I2S
#define I2S_WS_PIN    42 // Word Select
#define I2S_SCK_PIN   2  // Serial Clock
#define I2S_SD_PIN    41 // Serial Data

// Botão para ativação da IA
// Evite usar pinos TX/RX (1 é TX). Trocar para um pino GPIO disponível.
#define BUTTON_PIN    1  // Botão conectado ao GND (LOW = pressionado)

// =======================================================================
// CONFIGURAÇÕES DE REDE E SERVIDOR
// =======================================================================

const char* ssid = "LIVE TIM_Gl_304_2G";
const char* password = "@Gh2jm013";
const char* serverURL = "http://192.168.1.3:5000/ask";  // servidor agora em 192.168.1.3

// =======================================================================
// CONFIGURAÇÕES DE BLE
// =======================================================================

#define SERVICE_UUID_NAVIGATION        "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID_WRITE      "12345678-1234-1234-1234-123456789abd"
#define CHARACTERISTIC_UUID_NOTIFY     "12345678-1234-1234-1234-123456789abe"

// =======================================================================
// OBJETOS GLOBAIS
// =======================================================================

U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, OLED_CLK, OLED_MOSI, OLED_CS, OLED_DC, OLED_RST);

BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristicWrite = nullptr;
BLECharacteristic* pCharacteristicNotify = nullptr;
bool deviceConnected = false;

// =======================================================================
// VARIÁVEIS GLOBAIS
// =======================================================================

String gMensagem = "Aguardando...";
String gDirecao = "";
bool gDisplayNeedsUpdate = true;
bool isRecording = false;
bool buttonPressed = false;
unsigned long buttonPressTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;
const unsigned long RECORD_DURATION = 3000; // 3 segundos de gravação

// WiFi dinâmico via BLE
String bleWifiSSID = "";
String bleWifiPassword = "";
bool bleWifiRequested = false;
bool bleWifiScanRequested = false;
String bleWifiScanResult = "";

// =======================================================================
// CONFIGURAÇÃO DO I2S PARA MICROFONE INMP411
// =======================================================================

void setupI2S() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 16000,
        // Use 16-bit to reduce memory and simplify conversion
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK_PIN,
        .ws_io_num = I2S_WS_PIN,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD_PIN
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    Serial.println("[I2S] Microfone INMP411 inicializado!");
}

// =======================================================================
// CONFIGURAÇÃO DO WIFI
// =======================================================================

void setupWiFi() {
    // Conecta usando DHCP por padrão. Se quiser IP estático, altere manualmente.
    WiFi.mode(WIFI_STA);
    if (bleWifiRequested && bleWifiSSID.length() > 0) {
        Serial.println("[WiFi] Conectando via BLE: SSID=" + bleWifiSSID);
        WiFi.disconnect(true);
        delay(500);
        WiFi.begin(bleWifiSSID.c_str(), bleWifiPassword.c_str());
    } else {
        // Configuração de IP estático
        IPAddress local_IP(192, 168, 1, 184);     // Altere para um IP livre na sua rede
        IPAddress gateway(192, 168, 1, 1);        // IP do seu roteador
        IPAddress subnet(255, 255, 255, 0);       // Máscara de rede
        IPAddress dns(8, 8, 8, 8);                // DNS (opcional)
        WiFi.config(local_IP, gateway, subnet, dns);
        WiFi.begin(ssid, password);
    }
    Serial.print("[WiFi] Tentando conectar...");
    unsigned long start = millis();
    const unsigned long CONNECT_TIMEOUT = 20000; // 20s
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < CONNECT_TIMEOUT) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[WiFi] Conectado!");
        Serial.println("[WiFi] IP: " + WiFi.localIP().toString());
    } else {
        Serial.println("[WiFi] Falha ao conectar dentro do timeout.");
        Serial.println("[WiFi] Status code: " + String(WiFi.status()));
    }
}

// =======================================================================
// CALLBACKS BLE
// =======================================================================

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("[BLE] Cliente conectado");
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("[BLE] Cliente desconectado");
        delay(500);
        pServer->startAdvertising();
    }
};

class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        String receivedValue = pCharacteristic->getValue().c_str();
        if (receivedValue.length() > 0) {
            Serial.println("[BLE] Recebido: " + receivedValue);

            // Cadastro de usuário/responsável via JSON
            DynamicJsonDocument doc(512);
            DeserializationError error = deserializeJson(doc, receivedValue);
            if (!error && doc.containsKey("usuario") && doc.containsKey("senha")) {
                String nome = doc["nome"] | "";
                String usuario = doc["usuario"] | "";
                String senha = doc["senha"] | "";
                // CPF removido do payload possível — não é mais utilizado
                String responsavel = doc["responsavel"] | "";

                // Aqui você pode salvar os dados em variáveis globais, EEPROM, etc.
                gMensagem = "Usuário cadastrado: " + usuario;
                gDirecao = "info";
                gDisplayNeedsUpdate = true;
                Serial.println("[CADASTRO] Nome: " + nome + ", Usuário: " + usuario + ", Resp: " + responsavel);

                // Opcional: enviar resposta via Notify
                if (deviceConnected && pCharacteristicNotify) {
                    String resposta = "Cadastro recebido: " + usuario;
                    pCharacteristicNotify->setValue(resposta.c_str());
                    pCharacteristicNotify->notify();
                }
                return;
            }

            // Comando especial para escanear WiFi
            if (receivedValue == "SCAN_WIFI") {
                bleWifiScanRequested = true;
                return;
            }
            // Comando especial para conectar WiFi: WIFI|SSID|SENHA
            if (receivedValue.startsWith("WIFI|")) {
                int idx1 = receivedValue.indexOf('|', 5);
                if (idx1 > 5) {
                    bleWifiSSID = receivedValue.substring(5, idx1);
                    bleWifiPassword = receivedValue.substring(idx1 + 1);
                    bleWifiRequested = true;
                    Serial.println("[BLE] WiFi dinâmico recebido: SSID=" + bleWifiSSID + " SENHA=" + bleWifiPassword);
                }
                return;
            }
            // Comando de navegação padrão
            processNavigationCommand(receivedValue);
        }
    }
};

// =======================================================================
// PROCESSAMENTO DE COMANDOS DE NAVEGAÇÃO
// =======================================================================

void processNavigationCommand(String command) {
    // Tenta parsear como JSON primeiro
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, command);
    
    if (!error) {
        // É um JSON válido
        if (doc.containsKey("direction") && doc.containsKey("message")) {
            gDirecao = doc["direction"].as<String>();
            gMensagem = doc["message"].as<String>();
        }
    } else {
        // Tenta formato "direcao|mensagem"
        int pipeIndex = command.indexOf('|');
        if (pipeIndex > 0) {
            gDirecao = command.substring(0, pipeIndex);
            gMensagem = command.substring(pipeIndex + 1);
        } else {
            gDirecao = "info";
            gMensagem = command;
        }
    }
    
    gDisplayNeedsUpdate = true;
    Serial.println("[NAV] Direção: " + gDirecao + " | Mensagem: " + gMensagem);
}

// =======================================================================
// FUNÇÕES DE DISPLAY
// =======================================================================

void drawArrow(String direction, int cx, int cy, int size) {
    if (direction == "up" || direction == "norte") {
        // Seta para cima
        for (int i = 0; i < size; i++) {
            u8g2.drawLine(cx, cy - size + i, cx - i, cy + i);
            u8g2.drawLine(cx, cy - size + i, cx + i, cy + i);
        }
    }
    else if (direction == "down" || direction == "sul") {
        // Seta para baixo
        for (int i = 0; i < size; i++) {
            u8g2.drawLine(cx, cy + size - i, cx - i, cy - i);
            u8g2.drawLine(cx, cy + size - i, cx + i, cy - i);
        }
    }
    else if (direction == "left" || direction == "oeste") {
        // Seta para esquerda
        for (int i = 0; i < size; i++) {
            u8g2.drawLine(cx - size + i, cy, cx + i, cy - i);
            u8g2.drawLine(cx - size + i, cy, cx + i, cy + i);
        }
    }
    else if (direction == "right" || direction == "leste") {
        // Seta para direita
        for (int i = 0; i < size; i++) {
            u8g2.drawLine(cx + size - i, cy, cx - i, cy - i);
            u8g2.drawLine(cx + size - i, cy, cx - i, cy + i);
        }
    }
    else {
        // Círculo para outras direções
        u8g2.drawCircle(cx, cy, size);
    }
}

void wrapText(String text, int x, int y, int maxWidth, int lineHeight) {
    int currentY = y;
    int startPos = 0;
    
    while (startPos < text.length()) {
        int endPos = startPos;
        int lastSpace = -1;
        String testLine = "";
        
        // Encontra quantos caracteres cabem na linha
        while (endPos < text.length()) {
            char c = text.charAt(endPos);
            if (c == ' ') lastSpace = endPos;
            
            testLine += c;
            
            // Verifica se a linha atual cabe na largura máxima
            if (u8g2.getStrWidth(testLine.c_str()) > maxWidth) {
                break;
            }
            endPos++;
        }
        
        // Se não cabe tudo, quebra na última palavra completa
        if (endPos < text.length() && lastSpace > startPos) {
            endPos = lastSpace;
        }
        
        // Desenha a linha
        String line = text.substring(startPos, endPos);
        u8g2.drawStr(x, currentY, line.c_str());
        
        currentY += lineHeight;
        startPos = (endPos < text.length() && text.charAt(endPos) == ' ') ? endPos + 1 : endPos;
        
        // Para evitar sair da tela
        if (currentY > 64) break;
    }
}

void updateDisplay() {
    if (!gDisplayNeedsUpdate) return;
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tr);
    
    // Status da conexão
    if (deviceConnected) {
        u8g2.drawStr(0, 8, "BLE: Conectado");
    } else {
        u8g2.drawStr(0, 8, "BLE: Aguardando...");
    }
    
    // Status do WiFi
    if (WiFi.status() == WL_CONNECTED) {
        u8g2.drawStr(80, 8, "WiFi: OK");
    }
    
    // Status da gravação
    if (isRecording) {
        u8g2.drawStr(0, 18, "Gravando...");
    }
    
    // Desenha seta se há direção
    if (gDirecao != "") {
        drawArrow(gDirecao, 110, 40, 8);
    }
    
    // Mostra mensagem com quebra de linha
    if (gMensagem != "") {
        u8g2.setFont(u8g2_font_6x10_tr);
        wrapText(gMensagem, 2, 28, 85, 10);
    }
    
    u8g2.sendBuffer();
    gDisplayNeedsUpdate = false;
}

// =======================================================================
// FUNÇÕES DE ÁUDIO E IA
// =======================================================================

// Gravação e envio de áudio em streaming (blocos)
void recordAudio() {
    const int sampleRate = 8000;
    const int maxRecordSeconds = 3; // reduzido para evitar estouro de memória
    const int bufferSize = sampleRate * maxRecordSeconds * 2; // 16-bit samples

    // Tentar alocar buffer; se falhar, aborta gravação
    uint8_t* audioBuffer = (uint8_t*)malloc(bufferSize);
    if (!audioBuffer) {
        Serial.println("[AUDIO] Erro ao alocar memória!");
        return;
    }

    gMensagem = "Gravando...";
    gDisplayNeedsUpdate = true;
    updateDisplay();

    Serial.println("[AUDIO] Iniciando gravação (pressione e segure o botão)...");

    size_t bytesRead = 0;
    size_t totalBytesRead = 0;
    int16_t maxSample = 0;
    int16_t minSample = 0;
    bool firstSample = true;
    // Para I2S 16-bit, usamos buffer de int16_t
    int16_t* i2sBuffer = (int16_t*)malloc(1024 * sizeof(int16_t));
    if (!i2sBuffer) {
        Serial.println("[AUDIO] Erro ao alocar i2sBuffer!");
        free(audioBuffer);
        return;
    }

    unsigned long startTime = millis();
    unsigned long lastReport = startTime;

    // Enquanto o botão for mantido pressionado (LOW) e não exceder o buffer/max tempo
    while (digitalRead(BUTTON_PIN) == LOW && (millis() - startTime) < (maxRecordSeconds * 1000) && totalBytesRead < bufferSize - 2) {
        i2s_read(I2S_NUM_0, i2sBuffer, 1024 * sizeof(int16_t), &bytesRead, portMAX_DELAY);
        int samplesRead = bytesRead / 2;
        for (int i = 0; i < samplesRead && totalBytesRead < bufferSize - 1; i++) {
            int16_t sample = i2sBuffer[i];
            audioBuffer[totalBytesRead++] = sample & 0xFF;
            audioBuffer[totalBytesRead++] = (sample >> 8) & 0xFF;
            if (firstSample) {
                maxSample = sample;
                minSample = sample;
                firstSample = false;
            } else {
                if (sample > maxSample) maxSample = sample;
                if (sample < minSample) minSample = sample;
            }
        }

        // Atualiza display e log a cada 500ms
        if (millis() - lastReport > 500) {
            unsigned long elapsed = (millis() - startTime) / 1000;
            Serial.println("[AUDIO] Gravando... " + String(elapsed) + "s");
            gMensagem = "Gravando: " + String(elapsed) + "s";
            gDisplayNeedsUpdate = true;
            updateDisplay();
            lastReport = millis();
        }
    }

    free(i2sBuffer);

    Serial.println("[AUDIO] Gravação encerrada. bytes=" + String(totalBytesRead));
    Serial.println("[AUDIO] Amplitude máxima: " + String(maxSample) + ", mínima: " + String(minSample));

    // Verifica se o áudio tem amplitude diferente de zero
    if (maxSample == 0 && minSample == 0) {
        Serial.println("[AUDIO] Áudio vazio! Não será enviado.");
        gMensagem = "Erro: áudio vazio!";
        gDisplayNeedsUpdate = true;
        updateDisplay();
        free(audioBuffer);
        return;
    }

    gMensagem = "Processando...";
    gDisplayNeedsUpdate = true;
    updateDisplay();

    // Envia para o servidor de IA
    sendAudioToAI(audioBuffer, totalBytesRead);

    free(audioBuffer);
}

void sendAudioToAI(uint8_t* audioData, size_t audioSize) {
    if (WiFi.status() != WL_CONNECTED) {
        gMensagem = "WiFi desconectado!";
        gDisplayNeedsUpdate = true;
        Serial.println("[AI] WiFi não conectado!");
        return;
    }
    
    Serial.println("[AI] Tentando conectar a: " + String(serverURL));
    Serial.println("[AI] IP do ESP32: " + WiFi.localIP().toString());
    
    // --- Probe rápido: envia um POST pequeno para checar conectividade/tempo de resposta ---
    HTTPClient httpProbe;
    httpProbe.begin(serverURL);
    httpProbe.addHeader("Content-Type", "application/octet-stream");
    httpProbe.setTimeout(5000);
    int probeCode = httpProbe.POST((uint8_t*)"probe", 5);
    Serial.println("[AI] Probe HTTP response: " + String(probeCode));
    httpProbe.end();
    if (probeCode <= 0) {
        // Se probe falhar, tenta conexão TCP direta para isolar problema
        Serial.println("[AI] Probe falhou, testando conexão TCP direta...");
        WiFiClient tcp;
        const char* server_host = "192.168.1.3"; // ajuste se necessário
        const uint16_t server_port = 5000;
        if (!tcp.connect(server_host, server_port)) {
            Serial.println("[AI] Conexão TCP falhou - servidor inacessível ou porta fechada");
            gMensagem = "Servidor inacessivel";
            gDisplayNeedsUpdate = true;
            return;
        } else {
            Serial.println("[AI] Conexão TCP OK");
            tcp.stop();
        }
    }
    // Envio por chunks para maior robustez
    const size_t CHUNK_SIZE = 16000; // 16KB por pedaço
    int totalChunks = (audioSize + CHUNK_SIZE - 1) / CHUNK_SIZE;
    String sessionId = String(millis()); // simples session id

    Serial.println("[AI] Enviando PCM por chunks: totalChunks=" + String(totalChunks) + " session=" + sessionId);

    int lastResponseCode = -1;
    String lastResponseBody = "";
    for (int idx = 0; idx < totalChunks; idx++) {
        size_t offset = idx * CHUNK_SIZE;
        size_t thisSize = min(CHUNK_SIZE, audioSize - offset);

        HTTPClient httpChunk;
        httpChunk.begin(serverURL);
        httpChunk.addHeader("Content-Type", "application/octet-stream");
        httpChunk.addHeader("X-Session-Id", sessionId);
        httpChunk.addHeader("X-Chunk-Index", String(idx));
        httpChunk.addHeader("X-Chunks-Total", String(totalChunks));
        httpChunk.addHeader("Content-Length", String(thisSize));
        // Se for o último chunk, o servidor pode levar mais tempo para transcrever
        if (idx == totalChunks - 1) {
            httpChunk.setTimeout(120000); // 120s para o último chunk
        } else {
            httpChunk.setTimeout(20000);
        }

        Serial.println("[AI] Enviando chunk " + String(idx) + "/" + String(totalChunks-1) + " size=" + String(thisSize));

        // Retry simples para casos de falha de transporte (-11)
        const int MAX_RETRIES = 3;
        int attempts = 0;
        int code = -1;
        String respBody = "";
        while (attempts < MAX_RETRIES) {
            if (attempts > 0) {
                Serial.println("[AI] Retentativa " + String(attempts) + " para chunk " + String(idx));
            }
            code = httpChunk.POST((uint8_t*)(audioData + offset), thisSize);
            if (code > 0) {
                respBody = httpChunk.getString();
                Serial.println("[AI] chunk response code=" + String(code));
                Serial.println("[AI] chunk response body=" + respBody);
                break;
            } else {
                Serial.println("[AI] Erro enviando chunk " + String(idx) + ": " + String(code));
                attempts++;
                delay(1000);
            }
        }

        lastResponseCode = code;
        lastResponseBody = respBody;

        httpChunk.end();
        if (code <= 0) {
            Serial.println("[AI] Falha no envio do chunk " + String(idx) + " apos " + String(MAX_RETRIES) + " tentativas. Abortando.");
            return; // aborta se não conseguiu enviar após tentativas
        }
        delay(200); // pequeno delay entre chunks (aumentado para reduzir carga)
    }

    // ao final, lastResponseBody contém a resposta final do servidor
    if (lastResponseCode > 0) {
        Serial.println("[AI] Resposta final: " + lastResponseBody);
        Serial.println("[AI] Bytes totais enviados: " + String(audioSize));
        gMensagem = lastResponseBody;
        gDirecao = "";
        gDisplayNeedsUpdate = true;
        if (deviceConnected && pCharacteristicNotify) {
            pCharacteristicNotify->setValue(lastResponseBody.c_str());
            pCharacteristicNotify->notify();
        }
    } else {
        Serial.println("[AI] Erro HTTP final: " + String(lastResponseCode));
        gMensagem = "Erro na IA: " + String(lastResponseCode);
        gDisplayNeedsUpdate = true;
    }
    
}

// Tenta reconectar WiFi periodicamente no loop
void tryReconnectWiFi() {
    static unsigned long lastTry = 0;
    if (WiFi.status() == WL_CONNECTED) return;
    if (millis() - lastTry < 10000) return; // 10s entre tentativas
    lastTry = millis();
    Serial.println("[WiFi] Tentando reconectar...");
    WiFi.reconnect();
}

// Função de cabeçalho WAV removida, não utilizada mais

// Função sendAudioToAI removida: não utilizada no novo fluxo PCM

// =======================================================================
// CONFIGURAÇÃO DO BLE
// =======================================================================

void setupBLE() {
    BLEDevice::init("ESP32-S3_Navigation_AI");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Serviço de navegação
    BLEService *pService = pServer->createService(SERVICE_UUID_NAVIGATION);

    // Característica WRITE (receber comandos)
    pCharacteristicWrite = pService->createCharacteristic(
        CHARACTERISTIC_UUID_WRITE,
        BLECharacteristic::PROPERTY_WRITE
    );
    pCharacteristicWrite->setCallbacks(new MyCharacteristicCallbacks());

    // Característica NOTIFY (enviar respostas da IA)
    pCharacteristicNotify = pService->createCharacteristic(
        CHARACTERISTIC_UUID_NOTIFY,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristicNotify->addDescriptor(new BLE2902());

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID_NAVIGATION);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);

    BLEDevice::startAdvertising();
    Serial.println("[BLE] Serviço iniciado - ESP32-S3_Navigation_AI");
}

// =======================================================================
// CONTROLE DO BOTÃO
// =======================================================================

void checkButton() {
    bool currentButtonState = digitalRead(BUTTON_PIN) == LOW; // LOW = pressionado (pullup interno)
    unsigned long now = millis();

    if (currentButtonState && !buttonPressed && (now - buttonPressTime > DEBOUNCE_DELAY)) {
        buttonPressed = true;
        buttonPressTime = now;

        // --- Detecção de múltiplos cliques ---
        if (now - lastButtonClickTime < MULTI_CLICK_INTERVAL) {
            buttonClickCount++;
        } else {
            buttonClickCount = 1;
        }
        lastButtonClickTime = now;

        if (buttonClickCount == 3 && !cadastroMode) {
            cadastroMode = true;
            gMensagem = "Cadastro: envie dados via BLE";
            gDirecao = "info";
            gDisplayNeedsUpdate = true;
            Serial.println("[CADASTRO] Modo cadastro iniciado! Aguarde dados via BLE.");
            // Aqui pode iniciar timeout ou aguardar dados
        } else if (!isRecording && !cadastroMode) {
            Serial.println("[BUTTON] Iniciando gravação de áudio...");
            isRecording = true;
            gDisplayNeedsUpdate = true;
            recordAudio();
            isRecording = false;
            gDisplayNeedsUpdate = true;
        }
    }

    // Sai do modo cadastro após timeout (exemplo: 30s)
    if (cadastroMode && (now - lastButtonClickTime > 30000)) {
        cadastroMode = false;
        gMensagem = "Cadastro cancelado (timeout)";
        gDirecao = "info";
        gDisplayNeedsUpdate = true;
        Serial.println("[CADASTRO] Timeout do cadastro.");
    }

    if (!currentButtonState) {
        buttonPressed = false;
    }
}

// =======================================================================
// SETUP PRINCIPAL
// =======================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("===========================================");
    Serial.println("ESP32-S3 Navegação + IA com Microfone");
    Serial.println("===========================================");
    
    // Configuração do botão
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
    // Inicialização do display
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7x13B_tr);
    u8g2.drawStr(0, 15, "ESP32-S3");
    u8g2.drawStr(0, 30, "Navegacao + IA");
    u8g2.drawStr(0, 45, "Inicializando...");
    u8g2.sendBuffer();
    
    // Configuração do WiFi
    setupWiFi();
    
    // Configuração do I2S para microfone
    setupI2S();
    
    // Configuração do BLE
    setupBLE();
    
    // Display inicial
    gMensagem = "Sistema pronto!";
    gDirecao = "";
    gDisplayNeedsUpdate = true;
    updateDisplay();
    
    Serial.println("[SETUP] Sistema inicializado!");
    Serial.println("[SETUP] Pressione o botão no pino " + String(BUTTON_PIN) + " para gravar áudio");
    Serial.println("[SETUP] Use BLE para enviar comandos de navegação");
}

// =======================================================================
// LOOP PRINCIPAL
// =======================================================================

void loop() {
    // Verifica botão para gravação de áudio
    checkButton();

    // BLE WiFi scan: escaneia redes e envia lista via Notify
    if (bleWifiScanRequested && deviceConnected && pCharacteristicNotify) {
        Serial.println("[BLE] Escaneando redes WiFi...");
        int n = WiFi.scanNetworks();
        bleWifiScanResult = "";
        for (int i = 0; i < n; i++) {
            bleWifiScanResult += WiFi.SSID(i);
            if (i < n - 1) bleWifiScanResult += ",";
        }
        Serial.println("[BLE] Redes encontradas: " + bleWifiScanResult);
        pCharacteristicNotify->setValue(bleWifiScanResult.c_str());
        pCharacteristicNotify->notify();
        bleWifiScanRequested = false;
    }

    // Se WiFi dinâmico foi solicitado, tenta conectar
    if (bleWifiRequested) {
        setupWiFi();
        bleWifiRequested = false;
    }

    // Atualiza display se necessário
    updateDisplay();

    // Pequeno delay para não sobrecarregar o sistema
    tryReconnectWiFi();
    yield();
    delay(50);
}
