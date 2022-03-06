import './style.css';
import React from 'react';

const RouterTrafficHeader = ({ requestLanView, closeLanView, lanView }) => (
  <div className="ui-router-traffic-header">
    <div className="ui-router-traffic-back-label">
      <p>Miller-40242</p>
    </div>
    <div className="ui-router-traffic-main-label">
      <p>Traffic</p>
    </div>
    <div onClick={requestLanView} style={lanView ? { display: 'none' } : {}} className="ui-router-traffic-request-lan-label">
      <p>Request LAN</p>
    </div>
    <div style={{ display: 'none' }} className="ui-cancel-lan-request">
      <p>Cancel Request</p>
    </div>
    <div onClick={closeLanView} style={lanView ? {} : { display: 'none' }} className="ui-stop-lan">
      <div className="stop-lan-icon-wrapper">
        <img className="stop-lan-icon" src={require('../../../assets/images/stop-lan-eye.png')} alt="Stop Lan" />
      </div>
      <div className="stop-lan-label">
        <p>Stop LAN</p>
      </div>
    </div>
  </div>
);

export default RouterTrafficHeader;