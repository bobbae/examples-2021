import React from 'react';
import Switch from '../../Switch';

const PreferencesOther = () => {
  return (
    <div className="preferences-rows-container">
      <div className="preferences-rows">
        <div className="preferences-row">
          <div className="preferences-row-label">
            <p>Calibration</p>
          </div>
          <div className="preferences-row-options">
            <Switch />
          </div>
        </div>
        <div className="preferences-row">
          <div className="preferences-row-label">
            <p>Speedtest</p>
          </div>
          <div className="preferences-row-options">
            <Switch />
          </div>
        </div>
        <div className="preferences-row">
          <div className="preferences-row-label">
            <p>Shaping</p>
          </div>
          <div className="preferences-row-options">
            <Switch />
          </div>
        </div>
      </div>
    </div>
  )
}

export default PreferencesOther;