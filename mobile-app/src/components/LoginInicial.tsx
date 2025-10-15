import React, { useState } from 'react';

interface LoginProps {
  onLogin: (tipo: 'usuario' | 'responsavel', dados: any) => void;
}

export default function LoginInicial({ onLogin }: LoginProps) {
  const [tipo, setTipo] = useState<'usuario' | 'responsavel' | null>(null);
  const [nome, setNome] = useState('');
  const [usuario, setUsuario] = useState('');
  const [senha, setSenha] = useState('');
  const [responsavel, setResponsavel] = useState('');

  function handleLogin() {
    if (tipo === 'usuario') {
      onLogin('usuario', { nome, usuario, senha, responsavel });
    } else if (tipo === 'responsavel') {
      onLogin('responsavel', { nome, usuario, senha });
    }
  }

  return (
    <div style={{ maxWidth: 400, margin: '40px auto', padding: 20, border: '1px solid #ccc', borderRadius: 8 }}>
      <h2>Login Inicial</h2>
      <div style={{ marginBottom: 16 }}>
        <button onClick={() => setTipo('usuario')} style={{ marginRight: 8, padding: '8px 16px' }}>Sou Usuário</button>
        <button onClick={() => setTipo('responsavel')} style={{ padding: '8px 16px' }}>Sou Responsável</button>
      </div>
      {tipo && (
        <div>
          <input type="text" placeholder="Nome" value={nome} onChange={e => setNome(e.target.value)} style={inputStyle} />
          <input type="text" placeholder="Usuário" value={usuario} onChange={e => setUsuario(e.target.value)} style={inputStyle} />
          <input type="password" placeholder="Senha" value={senha} onChange={e => setSenha(e.target.value)} style={inputStyle} />
          {/* CPF removido - campo opcional não necessário */}
          {tipo === 'usuario' && (
            <input type="text" placeholder="Responsável" value={responsavel} onChange={e => setResponsavel(e.target.value)} style={inputStyle} />
          )}
          <button onClick={handleLogin} style={{ marginTop: 16, padding: '10px 20px', fontWeight: 'bold' }}>Entrar</button>
        </div>
      )}
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
