import './style.css';
import React from 'react';

const TrafficDropdown = ({ }) => (
  <div className="ui-traffic-dropdown">
    <div className="ui-traffic-dropdown-inner">
      <div className="ui-traffic-dropdown-label">
        <p>
          Sort by Traffic
        </p>
      </div>
      <div className="ui-traffic-dropdown-text">
        <p>
          Absolute traffic – total traffic amount Relative traffic – percentage compared to ISP Plan
        </p>
      </div>
    </div>
    <div className="ui-traffic-dropdown-button">
      <p>
        Sort by absolute traffic
    </p>
    </div>
    <div className="ui-traffic-dropdown-button">
      <p>
        Sort by relative traffic
    </p>
    </div>
  </div>
)

export default TrafficDropdown;