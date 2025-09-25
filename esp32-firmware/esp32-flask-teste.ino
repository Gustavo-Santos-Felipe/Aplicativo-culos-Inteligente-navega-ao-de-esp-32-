// =======================================================================
// ESP32-S3 - Exemplo de Envio de Áudio para Servidor Flask (sem IA)
// Apenas envia arquivo .wav via serial para o servidor e exibe resposta
// =======================================================================

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <U8g2lib.h>
#include <driver/i2s.h>

// --- Config Wi-Fi ---
const char* ssid = "LIVE TIM_Gl_304_2G";
const char* password = "Gh2jm013";

// --- Config Display OLED ---
#define OLED_CLK   36
#define OLED_MOSI  35
#define OLED_CS    47
#define OLED_DC    21
#define OLED_RST   48
U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, OLED_CLK, OLED_MOSI, OLED_CS, OLED_DC, OLED_RST);

String gMensagem = "Aguardando...";
bool gDisplayNeedsUpdate = true;

void setupWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado!");
}

void exibirRespostaDisplay(String resposta) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7x13B_tr);
    u8g2.drawStr(0, 15, "Resposta:");
    int y = 40;
    int maxWidth = u8g2.getWidth();
    while (resposta.length() > 0) {
        String linha = resposta.substring(0, maxWidth / 7);
        resposta = resposta.substring(linha.length());
        u8g2.drawStr(0, y, linha.c_str());
        y += 15;
    }
    u8g2.sendBuffer();
}

void receberEEnviarAudioSerial() {
    const int bufferSize = 16000; // Ajuste conforme necessário
    uint8_t buffer[bufferSize];
    int bytesRecebidos = 0;
    Serial.println("Envie o arquivo de áudio pelo monitor serial...");
    while (Serial.available() && bytesRecebidos < bufferSize) {
        buffer[bytesRecebidos++] = Serial.read();
    }
    if (bytesRecebidos == 0) {
        Serial.println("Nenhum dado recebido!");
        return;
    }
    Serial.println("Arquivo recebido, enviando ao servidor...");
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin("http://192.168.1.3:5000/ask");
        http.addHeader("Content-Type", "audio/wav");
        int httpResponseCode = http.POST(buffer, bytesRecebidos);
        if (httpResponseCode > 0) {
            String resposta = http.getString();
            Serial.println("Resposta do servidor: " + resposta);
            exibirRespostaDisplay(resposta);
        } else {
            Serial.println("Erro na requisição HTTP: " + String(httpResponseCode));
        }
        http.end();
    } else {
        Serial.println("Wi-Fi não está conectado!");
    }
}

void setup() {
    Serial.begin(115200);
    delay(200);
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7x13B_tr);
    u8g2.drawStr(0, 15, "ESP32 Flask Teste");
    u8g2.drawStr(0, 40, "Aguardando...");
    u8g2.sendBuffer();
    setupWiFi();
}

void loop() {
    if (Serial.available()) {
        receberEEnviarAudioSerial();
    }
    delay(100);
}
