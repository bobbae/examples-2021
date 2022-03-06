import React, { useState } from 'react';
import './style.css';
import BlueCheckIcon from '../../assets/images/check-blue.png';

const ModalDropdownRow = ({ }) => {
  const [switchActive, setSwitchActive] = useState(false);
  return (
    <div className="ui-modal-row-multi">
      <div className={`dropdown-trigger-row ${switchActive ? 'border-animate' : ''}`}>
        <div className="ui-modal-row-label">
          <p>
            Default
        </p>
        </div>
        <div>
          <label class="form-switch">
            <input type="checkbox" value={switchActive}
              onChange={(event) => setSwitchActive(event.target.checked)} />
            <i></i>
          </label>
        </div>
      </div>
      <div className={`ui-modal-dropdown ${switchActive ? 'switch-appear' : 'switch-hide'}`}>
        <div className="ui-modal-row">
          <div className="ui-modal-row-label">
            <p>
              Enable
          </p>
          </div>
          <div>
            <img src={BlueCheckIcon} alt="Blue check" />
          </div>
        </div>
        <div className="ui-modal-row">
          <div className="ui-modal-row-label">
            <p>
              Disable
          </p>
          </div>
          <div className="ui-modal-row-value">
            <img src={BlueCheckIcon} alt="Blue check" />
          </div>
        </div>
      </div>
    </div>
  );
}

export default ModalDropdownRow;