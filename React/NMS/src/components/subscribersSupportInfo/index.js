import './style.css';
import React from 'react';
import ShareIcon from '../../assets/images/share-icon.png';

const SubscribersSupportInfo = ({ }) => (

  <div id="support-info" className="ui-support-info ui-box">
    <div>
      <a href="# ">
        <div className="ui-support-info-label">
          <p>Support Info</p>
        </div>
        <div className="ui-support-info-hide">
          <p>Hide</p>
        </div>
      </a>
    </div>
    <div>
      <a href="# ">
        <div className="ui-support-info-text">
          <p>Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco.</p>
        </div>
      </a>
    </div>
    <div>
      <a href="# ">
        <div className="ui-support-share-wrapper">
          <div>
            <img className="ui-support-share-icon" src={ShareIcon} alt="Share" />
          </div>
          <div className="ui-support-share-label">
            <p>Share...</p>
          </div>
        </div>
      </a>
    </div>
  </div>
)

export default SubscribersSupportInfo;