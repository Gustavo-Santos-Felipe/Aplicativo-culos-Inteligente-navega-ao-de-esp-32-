// Exemplo de função para enviar cadastro via BLE usando Web Bluetooth API
// Coloque este código em src/hooks/useEnviarCadastroBLE.ts

export async function enviarCadastroParaESP32(dados: any) {
  try {
    if (!('bluetooth' in navigator) || !navigator.bluetooth) {
      throw new Error('Web Bluetooth não disponível neste navegador');
    }

    // Solicita dispositivo BLE
    const device = await navigator.bluetooth.requestDevice({
      filters: [{ services: ['12345678-1234-1234-1234-123456789abc'] }]
    });

    if (!device) {
      throw new Error('Nenhum dispositivo selecionado');
    }

    const server = await device.gatt?.connect();
    if (!server) {
      throw new Error('Não foi possível conectar ao GATT server do dispositivo');
    }

    const service = await server.getPrimaryService('12345678-1234-1234-1234-123456789abc');
    const characteristic = await service.getCharacteristic('12345678-1234-1234-1234-123456789abd');

    // Envia JSON como string UTF-8
    const encoder = new TextEncoder();
    const value = encoder.encode(JSON.stringify(dados));
    await characteristic.writeValue(value);
    alert('Cadastro enviado para ESP32!');
  } catch (err) {
    const message = err instanceof Error ? err.message : String(err);
    alert('Erro ao enviar para ESP32: ' + message);
    throw err;
  }
}
