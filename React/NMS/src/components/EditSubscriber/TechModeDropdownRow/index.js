import React, { useState } from 'react';
import './style.css';

const TechModeDropdownRow = ({ }) => {
  const [switchActive, setSwitchActive] = useState(false);
  return (
    <div className="modal-row-multi">
      <div className={`modal-dropdown-trigger-row ${switchActive ? 'modal-row-border-animate' : ''}`}>
        <div className="modal-row-label">
          <p>Tech Mode</p>
        </div>
        <div>
          <label class="form-switch">
            <input type="checkbox" value={switchActive}
              onChange={(event) => setSwitchActive(event.target.checked)} />
            <i></i>
          </label>
        </div>
      </div>
      <div className={`modal-dropdown ${switchActive ? 'modal-switch-appear' : 'modal-switch-hide'}`}>
        <div className="modal-row">
          <div className="modal-row-label">
            <p>Tech Mode Details</p>
          </div>
          <div className="modal-row-value">
            <p>No Details</p>
          </div>
        </div>
      </div>
    </div>
  );
}

export default TechModeDropdownRow;