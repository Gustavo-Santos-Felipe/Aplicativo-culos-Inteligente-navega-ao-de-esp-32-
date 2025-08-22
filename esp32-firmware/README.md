# Firmware do ESP32 - Receptor de Navegação

Este firmware, escrito em C++ para a IDE do Arduino, transforma o ESP32 em um servidor web que recebe instruções de navegação via Wi-Fi.

## Funcionalidades

- Conecta-se a uma rede Wi-Fi especificada.
- Inicia um servidor web na porta 80.
- Expõe um endpoint `/navigate` que aceita requisições `HTTP POST`.
- Processa o corpo da requisição (que deve ser um JSON) para extrair um array de instruções.
- Imprime as instruções recebidas no Serial Monitor.
- Contém código de exemplo (comentado) para exibir as instruções em um display OLED SSD1306.

## Requisitos

1.  **Placa ESP32**: O código é feito para um ESP32 DevKit ou similar.
2.  **IDE do Arduino**: Com o suporte para placas ESP32 instalado.
3.  **Biblioteca `ArduinoJson`**: Esta biblioteca é necessária para processar os dados JSON recebidos do aplicativo.

## Configuração e Uso

### 1. Instale a Biblioteca `ArduinoJson`

Na sua IDE do Arduino, vá em `Sketch` > `Include Library` > `Manage Libraries...`. Na barra de busca, digite `ArduinoJson` e instale a biblioteca de Benoit Blanchon.

### 2. Configure as Credenciais de Wi-Fi

Abra o arquivo `esp32-firmware.ino`. No topo do arquivo, você encontrará as seguintes linhas. Substitua os valores com os da sua rede Wi-Fi:

```cpp
const char* ssid = "NOME_DA_SUA_REDE_WIFI";
const char* password = "SENHA_DA_SUA_REDE_WIFI";
```

### 3. Carregue o Código

Conecte seu ESP32 ao computador, selecione a placa e a porta corretas na IDE do Arduino e clique no botão de Upload.

### 4. Encontre o Endereço IP

Com o ESP32 ainda conectado, abra o **Serial Monitor** (`Tools` > `Serial Monitor`). Certifique-se de que a velocidade (baud rate) esteja configurada para **115200**.

Após conectar-se ao Wi-Fi, o ESP32 imprimirá seu endereço IP no monitor, como no exemplo abaixo:

```
Conectando ao Wi-Fi... Conectado!
Endereço IP do ESP32: 192.168.1.104
Servidor HTTP iniciado.
```

Anote este endereço IP. Você precisará inseri-lo no código do aplicativo React (`src/App.tsx`).

### 5. (Opcional) Ativando o Display

Se você tiver um display OLED I2C (SSD1306) conectado ao seu ESP32, você pode descomentar as seções de código marcadas como `Opcional: Bibliotecas para Display`, `Inicialização do Display` e `Enviar para o Display` para que as instruções apareçam nele. Pode ser necessário instalar também as bibliotecas `Adafruit GFX` e `Adafruit SSD1306`. 