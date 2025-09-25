import serial
import sys

def exibir_hex_ascii(data):
    hex_data = ' '.join(f'{byte:02X}' for byte in data)
    ascii_data = ''.join(chr(byte) if 32 <= byte <= 126 else '.' for byte in data)
    print(f"HEX: {hex_data}")
    print(f"ASCII: {ascii_data}")

def main():
    if len(sys.argv) < 3:
        print("Uso: python enviar_serial.py <porta> <arquivo>")
        sys.exit(1)

    porta = sys.argv[1]
    arquivo = sys.argv[2]

    try:
        with open(arquivo, 'rb') as f:
            dados = f.read()
    except FileNotFoundError:
        print(f"Erro: Arquivo {arquivo} não encontrado.")
        sys.exit(1)

    try:
        with serial.Serial(porta, 115200, timeout=1) as ser:
            print(f"Enviando dados para {porta}...")
            ser.write(dados)
            print("Dados enviados com sucesso!")

            print("\nResposta recebida:")
            while True:
                resposta = ser.read(ser.in_waiting or 1)
                if resposta:
                    exibir_hex_ascii(resposta)
                else:
                    break
    except serial.SerialException as e:
        print(f"Erro ao acessar a porta serial: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()