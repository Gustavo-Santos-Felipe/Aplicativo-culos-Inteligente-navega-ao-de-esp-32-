import React from 'react';
import ReactDOM from 'react-dom/client';
import './App.css';
import App from './App';
import * as serviceWorkerRegistration from './serviceWorkerRegistration';

const root = ReactDOM.createRoot(
  document.getElementById('root') as HTMLElement
);
root.render(
  <React.StrictMode>
    <App />
  </React.StrictMode>
);

// Ativando o Service Worker para funcionalidades PWA e offline
serviceWorkerRegistration.register(); 