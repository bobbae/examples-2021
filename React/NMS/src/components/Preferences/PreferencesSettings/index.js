import React from 'react';

const PreferencesSettings = () => {
  return (
    <div className="preferences-rows-container">
      <div className="preferences-gray-label">
        <p>SETTINGS</p>
      </div>
      <div className="preferences-rows">
        <div className="preferences-row">
          <div className="preferences-row-label">
            <p>Timezone</p>
          </div>
          <div className="preferences-row-options">
            <div className="preferences-row-value">
              <p>America/Los Angeles</p>
            </div>
            <div className="arrow-right-image-wrapper">
              <img className="arrow-right-image" src={require('../../../assets/images/arrow-right.png')} />
            </div>
          </div>
        </div>
        <div className="preferences-row">
          <div className="preferences-row-label">
            <p>Weekly Email</p>
          </div>
          <div className="preferences-row-options">
            <div className="preferences-row-value">
              <p>Wednesday</p>
            </div>
            <div className="arrow-right-image-wrapper">
              <img className="arrow-right-image" src={require('../../../assets/images/arrow-right.png')} />
            </div>
          </div>
        </div>
        <div className="preferences-row">
          <div className="preferences-row-label">
            <p>Maintenance Hours</p>
          </div>
          <div className="preferences-row-options">
            <div className="preferences-row-value">
            </div>
            <div className="arrow-right-image-wrapper">
              <img className="arrow-right-image" src={require('../../../assets/images/arrow-right.png')} />
            </div>
          </div>
        </div>
      </div>
    </div>
  )
}

export default PreferencesSettings;