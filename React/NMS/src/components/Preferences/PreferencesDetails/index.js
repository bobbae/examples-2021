import React from 'react';

const PreferencesDetails = () => {
  return (
    <div className="preferences-rows-container">
      <div className="preferences-gray-label">
        <p>DETAILS</p>
      </div>
      <div className="preferences-rows">
        <div className="preferences-row preferences-long-input">
          <div className="preferences-row-label">
            <p>Name</p>
          </div>
          <div className="preferences-row-input-wrapper">
            <input type="text" className="preferences-row-input" placeholder="Sea Networks" />
          </div>
        </div>
        <div className="preferences-row preferences-long-input">
          <div className="preferences-row-label">
            <p>Support Email</p>
          </div>
          <div className="preferences-row-input-wrapper">
            <input type="email" className="preferences-row-input" placeholder="grace@smart.network" />
          </div>
        </div>
        <div className="preferences-row preferences-long-input">
          <div className="preferences-row-label">
            <p>Data Cap Alert Email</p>
          </div>
          <div className="preferences-row-input-wrapper">
            <input type="email" className="preferences-row-input" placeholder="grace@smart.network" />
          </div>
        </div>
        <div className="preferences-row preferences-long-input">
          <div className="preferences-row-label">
            <p>Support URL</p>
          </div>
          <div className="preferences-row-input-wrapper">
            <input type="email" className="preferences-row-input" placeholder="forum.smart.network" />
          </div>
        </div>
      </div>
    </div>
  )
}

export default PreferencesDetails;