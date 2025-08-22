# 🎵 Sistema de Sons - Castrilha

## 📁 Arquivos de Som Necessários

Coloque os seguintes arquivos de áudio na pasta `public/assets/sounds/`:

### **Sons Principais:**
- `click.mp3` - Som de clique em botões
- `notification.mp3` - Notificações gerais
- `success.mp3` - Ações bem-sucedidas
- `error.mp3` - Erros e avisos
- `connect.mp3` - Conexão Bluetooth bem-sucedida
- `disconnect.mp3` - Desconexão Bluetooth
- `navigation.mp3` - Início de navegação
- `instruction.mp3` - Instruções de navegação
- `hover.mp3` - Hover em elementos (mais baixo)
- `toggle.mp3` - Alternar configurações

## 🎨 Especificações dos Sons

### **Formato:** MP3
### **Qualidade:** 128-192 kbps
### **Duração:** 0.1-2 segundos
### **Volume:** Normalizado

## 🎯 Quando Cada Som Toca

### **click.mp3**
- Clique em qualquer botão
- Interação com controles
- Navegação na interface

### **notification.mp3**
- Parar navegação
- Alertas gerais
- Notificações do sistema

### **success.mp3**
- Localização obtida com sucesso
- Rota calculada com sucesso
- Chegada ao destino
- Ações bem-sucedidas

### **error.mp3**
- Erro de geolocalização
- Falha na conexão Bluetooth
- Erro ao calcular rota
- Navegador não suportado

### **connect.mp3**
- Conexão Bluetooth estabelecida
- Dispositivo ESP32 conectado

### **disconnect.mp3**
- Desconexão Bluetooth
- ESP32 desconectado

### **navigation.mp3**
- Iniciar navegação
- Começar rota

### **instruction.mp3**
- Nova instrução de navegação
- Próximo passo da rota

### **hover.mp3**
- Hover em botões (volume reduzido)
- Passar mouse sobre elementos

### **toggle.mp3**
- Alternar configurações
- Ligar/desligar sons

## 🎛️ Controles de Som

### **Localização:** Canto superior direito do header
### **Funcionalidades:**
- 🔊/🔇 - Ligar/desligar sons
- 📊 - Controle de volume (0-100%)
- 🎵 - Testar som

## 🎨 Personalização

### **Alterar Volume:**
```javascript
// No hook useSounds
setVolume(0.7); // 70% do volume
```

### **Desabilitar Sons:**
```javascript
// No hook useSounds
setSoundEnabled(false);
```

### **Adicionar Novo Som:**
1. Adicione o arquivo em `public/assets/sounds/`
2. Adicione o tipo em `SoundType`
3. Use `playSound('novo-som')`

## 🎵 Exemplos de Sons

### **Sons Curtos (0.1-0.3s):**
- click.mp3 - "pop" suave
- hover.mp3 - "ping" baixo
- toggle.mp3 - "switch" rápido

### **Sons Médios (0.5-1s):**
- notification.mp3 - "ding" claro
- success.mp3 - "success" positivo
- error.mp3 - "error" alerta

### **Sons Longos (1-2s):**
- connect.mp3 - "connected" satisfatório
- disconnect.mp3 - "disconnected" neutro
- navigation.mp3 - "start" motivacional
- instruction.mp3 - "next" direcional

## 🚀 Dicas de Implementação

1. **Sons Curtos**: Para interações rápidas
2. **Feedback Positivo**: Sons agradáveis para sucessos
3. **Alertas Claros**: Sons distintos para erros
4. **Volume Baixo**: Hover e sons secundários
5. **Preload**: Todos os sons são pré-carregados

## 🎧 Compatibilidade

- ✅ Chrome/Edge (Web Audio API)
- ✅ Firefox (Web Audio API)
- ✅ Safari (Web Audio API)
- ⚠️ Mobile (pode precisar de interação do usuário)

## 🔧 Solução de Problemas

### **Sons não tocam:**
1. Verifique se os arquivos estão na pasta correta
2. Confirme se o navegador suporta Web Audio API
3. Verifique se o som não está desabilitado
4. Teste com o botão "Testar Som"

### **Sons muito altos/baixos:**
1. Ajuste o volume no controle
2. Normalize os arquivos de áudio
3. Verifique o volume do sistema

### **Delay nos sons:**
1. Os sons são pré-carregados automaticamente
2. Primeira interação pode ter pequeno delay
3. Sons subsequentes são instantâneos 