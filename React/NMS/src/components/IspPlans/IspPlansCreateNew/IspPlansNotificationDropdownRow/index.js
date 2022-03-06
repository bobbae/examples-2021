import React, { useState } from 'react';
import './style.css';
import BlueCheckIcon from '../../../../assets/images/check-blue.png';

const IspPlansNotificationDropdownRow = ({ }) => {
  const [switchActive, setSwitchActive] = useState(false);
  return (
    <div className="isp-modal-row-multi">
      <div className={`dropdown-trigger-row ${switchActive ? 'border-animate' : ''}`}>
        <div className="isp-modal-row-label">
          <p>Data Cap Notification</p>
        </div>
        <div>
          <label class="form-switch">
            <input type="checkbox" value={switchActive}
              onChange={(event) => setSwitchActive(event.target.checked)} />
            <i></i>
          </label>
        </div>
      </div>
      <div className={`isp-modal-dropdown ${switchActive ? 'switch-appear' : 'switch-hide'}`}>
        <div className="isp-modal-row">
          <div className="isp-modal-row-label">
            <p>Notify Subscriber</p>
          </div>
          <div>
            <img src={BlueCheckIcon} alt="Blue check" />
          </div>
        </div>
        <div className="isp-modal-row">
          <div className="isp-modal-row-label">
            <p>Notify Organization</p>
          </div>
          <div className="isp-modal-row-value">
            <img src={BlueCheckIcon} alt="Blue check" />
          </div>
        </div>
        <div className="isp-modal-row">
          <div className="isp-modal-row-label">
            <p>Notify Both</p>
          </div>
          <div className="isp-modal-row-value">
            <img src={BlueCheckIcon} alt="Blue check" />
          </div>
        </div>
      </div>
      <div className="notification-subtext">
        <p>Notifications are sent to email adresses</p>
      </div>
    </div>
  );
}

export default IspPlansNotificationDropdownRow;