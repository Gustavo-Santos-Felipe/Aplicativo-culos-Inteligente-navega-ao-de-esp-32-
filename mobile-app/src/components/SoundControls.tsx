import React, { useState } from 'react';
import { useSounds } from '../hooks/useSounds';

interface SoundControlsProps {
  className?: string;
}

export const SoundControls: React.FC<SoundControlsProps> = ({ className }) => {
  const { playSound, setVolume, setSoundEnabled, isEnabled, getVolume } = useSounds();
  const [isOpen, setIsOpen] = useState(false);

  const handleToggle = () => {
    const newEnabled = !isEnabled();
    setSoundEnabled(newEnabled);
    playSound('toggle');
    setIsOpen(!isOpen);
  };

  const handleVolumeChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const volume = parseFloat(e.target.value);
    setVolume(volume);
    playSound('click');
  };

  const handleTestSound = () => {
    playSound('notification');
  };

  return (
    <div className={`sound-controls ${className || ''}`}>
      <button 
        className="sound-toggle-btn"
        onClick={handleToggle}
        title={isEnabled() ? "Desabilitar Som" : "Habilitar Som"}
      >
        {isEnabled() ? '🔊' : '🔇'}
      </button>
      
      {isOpen && (
        <div className="sound-panel">
          <div className="sound-control-group">
            <label htmlFor="volume-slider">Volume</label>
            <input
              id="volume-slider"
              type="range"
              min="0"
              max="1"
              step="0.1"
              value={getVolume()}
              onChange={handleVolumeChange}
              className="volume-slider"
            />
            <span className="volume-value">{Math.round(getVolume() * 100)}%</span>
          </div>
          
          <button 
            className="test-sound-btn"
            onClick={handleTestSound}
          >
            Testar Som
          </button>
        </div>
      )}
    </div>
  );
}; 