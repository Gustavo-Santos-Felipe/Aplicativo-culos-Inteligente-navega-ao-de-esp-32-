import React, { useState } from 'react';

interface CadastroUsuarioProps {
  onEnviarBLE: (dados: {
    nome: string;
    usuario: string;
    senha: string;
    responsavel: string;
  }) => void;
}

export default function CadastroUsuario({ onEnviarBLE }: CadastroUsuarioProps) {
  const [nome, setNome] = useState('');
  const [usuario, setUsuario] = useState('');
  const [senha, setSenha] = useState('');
  const [responsavel, setResponsavel] = useState('');

  const handleEnviar = () => {
    const dados = {
      nome,
      usuario,
      senha,
      responsavel
    };
    if (onEnviarBLE) onEnviarBLE(dados);
  };

  return (
    <div style={{ maxWidth: 400, margin: '40px auto', padding: 20, border: '1px solid #ccc', borderRadius: 8 }}>
      <h2>Cadastro de Usuário</h2>
      <input type="text" placeholder="Nome" value={nome} onChange={e => setNome(e.target.value)} style={inputStyle} />
      <input type="text" placeholder="Usuário" value={usuario} onChange={e => setUsuario(e.target.value)} style={inputStyle} />
      <input type="password" placeholder="Senha" value={senha} onChange={e => setSenha(e.target.value)} style={inputStyle} />
  {/* CPF removido - não coletamos CPF no cadastro por agora */}
      <input type="text" placeholder="Responsável" value={responsavel} onChange={e => setResponsavel(e.target.value)} style={inputStyle} />
      <button onClick={handleEnviar} style={{ marginTop: 16, padding: '10px 20px', fontWeight: 'bold' }}>Cadastrar e Enviar para ESP32</button>
    </div>
  );
}

const inputStyle = {
  display: 'block',
  width: '100%',
  marginBottom: 10,
  padding: 8,
  borderRadius: 5,
  border: '1px solid #ccc'
};
