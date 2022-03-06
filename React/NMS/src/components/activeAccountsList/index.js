import './style.css';
import React from 'react';
import DefaultAvatar from '../../assets/images/default-avatar.png';
import InfoIcon from '../../assets/images/info.png'

const ActiveAccountsList = ({ }) => (

  <div>
    <div className="ui-account-list-label">
      <p>Active Account</p>
    </div>
    <ul className="ui-account-list">
      <li>
        <div className="ui-account-profile-wrapper">
          <div className="ui-account-image-wrapper">
            <img src={DefaultAvatar} alt="Profile" />
          </div>
          <div className="ui-list-label-wrapper">
            <div className="ui-list-account-name">
              <p>ofer@valleyinternet.net</p>
            </div>
            <div className="ui-list-account-text">
              <p>5 organizations</p>
            </div>
          </div>
        </div>
        <div>
          <img src={InfoIcon} alt="Info" />
        </div>
      </li>
    </ul>
  </div>
)

export default ActiveAccountsList;