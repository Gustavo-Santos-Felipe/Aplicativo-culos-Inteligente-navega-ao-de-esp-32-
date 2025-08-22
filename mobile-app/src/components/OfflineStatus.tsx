import React from 'react';
import { useOffline } from '../hooks/useOffline';

interface OfflineStatusProps {
  className?: string;
}

export const OfflineStatus: React.FC<OfflineStatusProps> = ({ className }) => {
  const { 
    isOnline, 
    isInstalled, 
    showInstallPrompt, 
    installPWA,
    syncData 
  } = useOffline();

  return (
    <div className={`offline-status ${className || ''}`}>
      {/* Status Online/Offline */}
      <div className={`status-indicator ${isOnline ? 'online' : 'offline'}`}>
        <span className="status-dot"></span>
        <span className="status-text">
          {isOnline ? 'Online' : 'Offline'}
        </span>
      </div>

      {/* Botão de Instalação PWA */}
      {showInstallPrompt && !isInstalled && (
        <button 
          className="install-pwa-btn"
          onClick={installPWA}
          title="Instalar Castrilha"
        >
          📱 Instalar
        </button>
      )}

      {/* Botão de Sincronização */}
      {isOnline && (
        <button 
          className="sync-btn"
          onClick={syncData}
          title="Sincronizar dados"
        >
          🔄 Sync
        </button>
      )}

      {/* Indicador de PWA Instalado */}
      {isInstalled && (
        <div className="pwa-installed">
          <span>📱 Instalado</span>
        </div>
      )}
    </div>
  );
}; 