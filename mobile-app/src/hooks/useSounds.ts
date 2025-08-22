import { useCallback, useRef } from 'react';

// Tipos de som disponíveis
export type SoundType = 
  | 'click'
  | 'notification'
  | 'success'
  | 'error'
  | 'connect'
  | 'disconnect'
  | 'navigation'
  | 'instruction'
  | 'hover'
  | 'toggle';

// Interface para configuração de som
interface SoundConfig {
  volume: number;
  enabled: boolean;
}

// Hook para gerenciar sons
export const useSounds = () => {
  const audioRefs = useRef<Record<SoundType, HTMLAudioElement | null>>({
    click: null,
    notification: null,
    success: null,
    error: null,
    connect: null,
    disconnect: null,
    navigation: null,
    instruction: null,
    hover: null,
    toggle: null,
  });

  const configRef = useRef<SoundConfig>({
    volume: 0.5,
    enabled: true,
  });

  // Função para carregar um som
  const loadSound = useCallback((type: SoundType) => {
    if (audioRefs.current[type]) return;

    const audio = new Audio(`/assets/sounds/${type}.mp3`);
    audio.preload = 'auto';
    audio.volume = configRef.current.volume;
    audioRefs.current[type] = audio;
  }, []);

  // Função para tocar um som
  const playSound = useCallback((type: SoundType) => {
    if (!configRef.current.enabled) return;

    const audio = audioRefs.current[type];
    if (audio) {
      audio.currentTime = 0;
      audio.volume = configRef.current.volume;
      audio.play().catch(error => {
        console.warn(`Erro ao tocar som ${type}:`, error);
      });
    } else {
      // Carregar e tocar se não estiver carregado
      loadSound(type);
      setTimeout(() => playSound(type), 100);
    }
  }, [loadSound]);

  // Função para tocar som de hover (com debounce)
  const playHoverSound = useCallback(() => {
    if (!configRef.current.enabled) return;
    
    const audio = audioRefs.current.hover;
    if (audio && audio.paused) {
      audio.currentTime = 0;
      audio.volume = configRef.current.volume * 0.3; // Mais baixo para hover
      audio.play().catch(() => {});
    }
  }, []);

  // Função para configurar volume
  const setVolume = useCallback((volume: number) => {
    configRef.current.volume = Math.max(0, Math.min(1, volume));
    
    // Atualizar volume de todos os áudios carregados
    Object.values(audioRefs.current).forEach(audio => {
      if (audio) {
        audio.volume = configRef.current.volume;
      }
    });
  }, []);

  // Função para habilitar/desabilitar sons
  const setSoundEnabled = useCallback((enabled: boolean) => {
    configRef.current.enabled = enabled;
  }, []);

  // Função para pré-carregar todos os sons
  const preloadSounds = useCallback(() => {
    const soundTypes: SoundType[] = [
      'click',
      'notification', 
      'success',
      'error',
      'connect',
      'disconnect',
      'navigation',
      'instruction',
      'hover',
      'toggle'
    ];

    soundTypes.forEach(type => {
      loadSound(type);
    });
  }, [loadSound]);

  return {
    playSound,
    playHoverSound,
    setVolume,
    setSoundEnabled,
    preloadSounds,
    isEnabled: () => configRef.current.enabled,
    getVolume: () => configRef.current.volume,
  };
}; 