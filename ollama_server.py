from flask import Flask, request, jsonify
import subprocess
import requests
import json
import os # Importar o módulo os

app = Flask(__name__)

OLLAMA_URL = "http://localhost:11434/api/generate"  # Ajuste conforme seu Ollama

# Função para transcrever áudio usando Whisper CLI
def transcribe_audio(audio_path):
    try:
        # O Whisper salva a saída em um arquivo .txt.
        # Usamos --output_format txt e --output_dir para controlar onde ele salva.
        # check=True fará com que subprocess.run levante um erro se o Whisper falhar.
        result = subprocess.run(
            ["whisper", audio_path, "--model", "base", "--language", "pt", "--output_format", "txt", "--output_dir", os.path.dirname(audio_path)],
            capture_output=True, text=True, check=True
        )
        
        # O nome do arquivo de saída será o mesmo do áudio, mas com extensão .txt
        output_txt_path = os.path.splitext(audio_path)[0] + ".txt"
        
        # Loga a saída completa do Whisper para depuração
        print(f"Stdout do Whisper: {result.stdout}")
        print(f"Stderr do Whisper: {result.stderr}")

        # Loga o caminho do arquivo de áudio e o arquivo de saída esperado
        print(f"Processando áudio: {audio_path}")
        print(f"Arquivo de saída esperado: {output_txt_path}")

        if os.path.exists(output_txt_path):
            with open(output_txt_path, 'r', encoding='utf-8') as f:
                transcribed_text = f.read().strip()
            os.remove(output_txt_path) # Limpa o arquivo .txt gerado pelo Whisper
            return transcribed_text
        else:
            print(f"Erro: Arquivo de transcrição não encontrado em {output_txt_path}")
            return "Erro na transcrição: arquivo de saída do Whisper não encontrado."
    except subprocess.CalledProcessError as e:
        print(f"Erro ao executar Whisper (código de saída {e.returncode}): {e}")
        print(f"Stdout do Whisper: {e.stdout}")
        print(f"Stderr do Whisper: {e.stderr}")
        return f"Erro na transcrição pelo Whisper: {e.stderr.strip()}"
    except FileNotFoundError:
        print("Erro: Comando 'whisper' ou 'ffmpeg' não encontrado. Certifique-se de que estão instalados e no PATH.")
        return "Erro na transcrição: Whisper ou FFmpeg não encontrado no sistema."
    except Exception as e:
        print(f"Erro inesperado na transcrição: {e}")
        return f"Erro inesperado na transcrição: {e}"

# Função para consultar Ollama
def ask_ollama(prompt):
    payload = {
        "model": "llama3",  # Ou outro modelo disponível no seu Ollama
        "prompt": prompt
    }
    response = requests.post(OLLAMA_URL, json=payload, stream=True)
    resposta = ""
    for linha in response.iter_lines():
        if linha:
            try:
                obj = json.loads(linha.decode())
                resposta += obj.get("response", "")
            except Exception:
                # Ignora linhas que não são JSON válido (ex: mensagens de keep-alive)
                pass
    return resposta

@app.route("/ask", methods=["POST"])
def ask():
    if "audio" not in request.files:
        return jsonify({"error": "No audio file"}), 400
    audio = request.files["audio"]

    # Define um caminho temporário absoluto para salvar o áudio
    # Usar um nome de arquivo mais único para evitar colisões
    temp_audio_filename = f"temp_audio_{os.getpid()}_{os.urandom(4).hex()}.wav"
    temp_audio_path = os.path.join(os.getcwd(), temp_audio_filename)

    try:
        audio.save(temp_audio_path) # Salva o arquivo enviado
        
        # Transcreve o áudio
        texto = transcribe_audio(temp_audio_path)
        
        # Se a transcrição falhou, retorne o erro
        if texto.startswith("Erro na transcrição"):
            return jsonify({"resposta": texto}), 500

        # Consulta Ollama
        resposta = ask_ollama(texto)
        return jsonify({"resposta": resposta})

    except Exception as e:
        print(f"Erro interno no servidor Flask: {e}")
        return jsonify({"error": f"Erro interno do servidor: {e}"}), 500
    finally:
        # Garante que o arquivo temporário de áudio seja removido
        if os.path.exists(temp_audio_path):
            os.remove(temp_audio_path)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)