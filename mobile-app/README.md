# Aplicativo de Navegação (React)

Este é um aplicativo web feito com React que permite ao usuário:
1. Encontrar sua localização atual.
2. Digitar um endereço de destino com autocompletar (Google Places API).
3. Calcular e exibir uma rota no mapa (Google Directions API).
4. Conectar via Bluetooth ao ESP32 para receber instruções de navegação.
5. Enviar as instruções de rota simplificadas para o ESP32 via Bluetooth.

## Configuração e Execução

### 1. Preencha as Chaves de API

Abra o arquivo `src/App.tsx` e modifique as seguintes constantes:

- `GOOGLE_MAPS_API_KEY`: Insira aqui sua chave de API da Google Maps Platform.
  - **Importante**: Você precisa ativar as seguintes APIs no seu projeto Google Cloud:
    - **Maps JavaScript API**
    - **Directions API**
    - **Places API**
  - Siga as instruções em: [Google Maps Platform Get Started](https://developers.google.com/maps/gmp-get-started)

### 2. Configuração do ESP32 Bluetooth

- `ESP32_DEVICE_NAME`: Nome do seu dispositivo ESP32 que aparecerá na lista Bluetooth (padrão: "ESP32_Navigation")
- `ESP32_SERVICE_UUID`: UUID do serviço Bluetooth do ESP32 (padrão: "0000ffe0-0000-1000-8000-00805f9b34fb")
- `ESP32_CHARACTERISTIC_UUID`: UUID da característica para escrita (padrão: "0000ffe1-0000-1000-8000-00805f9b34fb")

**Nota**: O ESP32 deve estar configurado como dispositivo Bluetooth Low Energy (BLE) com o serviço e característica correspondentes.

### 3. Instale as Dependências

Navegue até esta pasta (`mobile-app`) no seu terminal e execute:

```bash
npm install
```

Este comando irá baixar todas as dependências necessárias, como React e a biblioteca do Google Maps.

### 4. Execute o Aplicativo

Após a instalação, inicie o servidor de desenvolvimento:

```bash
npm start
```

Isso abrirá o aplicativo no seu navegador, geralmente em `http://localhost:3000`.

## Como Usar o Bluetooth

### 1. Conectar ao ESP32
1. Clique no botão "Conectar ESP32" na seção Bluetooth
2. Selecione seu dispositivo ESP32 na lista de dispositivos Bluetooth
3. Aguarde a conexão ser estabelecida
4. O status mudará para "Conectado" com um indicador verde

### 2. Navegação
- Após conectar, você pode calcular rotas normalmente
- As instruções de navegação serão enviadas automaticamente via Bluetooth
- O ESP32 receberá as instruções em tempo real conforme você se aproxima de cada etapa

### 3. Compatibilidade
- **Navegadores suportados**: Chrome, Edge, Opera
- **Navegadores não suportados**: Firefox, Safari
- **Requisito**: HTTPS ou localhost para Web Bluetooth funcionar

## Como Transformar em um App Híbrido

Este projeto foi feito com Create React App, que é uma base excelente para criar aplicativos híbridos usando ferramentas como [Capacitor](https://capacitorjs.com/) ou [Apache Cordova](https://cordova.apache.org/).

Com o Capacitor, por exemplo, você pode adicionar as plataformas Android e iOS ao seu projeto web existente com alguns comandos e, em seguida, construir o APK (para Android) ou o projeto do Xcode (para iOS). 