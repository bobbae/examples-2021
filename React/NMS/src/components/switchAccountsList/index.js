import './style.css';
import React from 'react';
import DefaultAvatar from '../../assets/images/default-avatar.png';

const SwitchAccountsList = ({ }) => (

  <div>
    <div className="ui-account-list-label">
      <p>Switch Account</p>
    </div>
    <ul className="ui-account-list ui-multi-list">
      <li>
        <div className="ui-account-profile-wrapper">
          <div className="ui-account-image-wrapper">
            <img src={DefaultAvatar} alt="Profile" />
          </div>
          <div className="ui-list-label-wrapper">
            <div className="ui-list-account-name">
              <p>ofer@smart.network</p>
            </div>
            <div className="ui-list-account-text">
              <p>5 organizations</p>
            </div>
          </div>
        </div>
        <div className="ui-account-list-blue">
          <p>Switch</p>
        </div>
      </li>
      <li>
        <div className="ui-account-profile-wrapper">
          <div className="ui-account-image-wrapper">
            <img src={DefaultAvatar} alt="Profile" />
          </div>
          <div className="ui-list-label-wrapper">
            <div className="ui-list-account-name">
              <p>test@valleyinternet.net</p>
            </div>
            <div className="ui-list-account-text">
              <p>2 organizations</p>
            </div>
          </div>
        </div>
        <div className="ui-account-list-blue">
          <p>Switch</p>
        </div>
      </li>
      <li>
        <div className="ui-account-profile-wrapper">
          <div className="ui-account-image-wrapper">
            <img src={DefaultAvatar} alt="Profile" />
          </div>
          <div className="ui-list-label-wrapper">
            <div className="ui-list-account-name">
              <p>production@smart.network</p>
            </div>
            <div className="ui-list-account-text">
              <p>4 organizations</p>
            </div>
          </div>
        </div>
        <div className="ui-account-list-blue">
          <p>Switch</p>
        </div>
      </li>
    </ul>
  </div>
)

export default SwitchAccountsList;