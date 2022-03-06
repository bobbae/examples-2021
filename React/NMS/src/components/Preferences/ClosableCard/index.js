import React from 'react';
import './style.css';

const ClosableCard = () => {
  return (
    <div className="ui-closable-card">
      <div className="close-container">
        <div className="close-icon-container">
          <img src={require('../../../assets/images/close-icon.png')} className="close-icon" alt="Close" />
        </div>
      </div>
      <div className="preferences-rows">
        <div className="preferences-row">
          <div className="preferences-row-label">
            <p>Type</p>
          </div>
          <div className="preferences-row-value">
            <p>Day of week</p>
          </div>
        </div>
        <div className="preferences-row">
          <div className="preferences-row-label">
            <p>Every</p>
          </div>
          <div className="preferences-row-value">
            <p>Mon, Tue, Wed, Thu, Fri</p>
          </div>
        </div>
        <div className="preferences-row">
          <div className="preferences-row-label">
            <p>From</p>
          </div>
          <div className="preferences-row-value">
            <p>10:00 AM</p>
          </div>
        </div>
        <div className="preferences-row">
          <div className="preferences-row-label">
            <p>To</p>
          </div>
          <div className="preferences-row-value">
            <p>1:00 PM</p>
          </div>
        </div>
      </div>
    </div>
  )
}

export default ClosableCard;