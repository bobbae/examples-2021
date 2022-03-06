import React from 'react';
import './style.css';

const TrafficProgressBars = () => {
  return (
    <div className="traffic-progress-bars">
      <div className="progress-inner">
        <div className="progress-value-container">
          <div>
            <img className="progress-arrow" src={require('../../../assets/images/green-download-icon.png')} alt="Download arrow" />
          </div>
          <div>
            <p className="ui-speed-label-green">21.4 Mb/s</p>
          </div>
        </div>
        <div className="router-traffic-progress-gradient ui-gradient-green">
          <div className="ui-progress-number-container">
            <div>
              <p>1</p>
            </div>
            <div>
              <p>5</p>
            </div>
            <div>
              <p>20</p>
            </div>
          </div>
          <span className="ui-progress-background"></span>
        </div>
      </div>
      <div className="progress-inner ">
        <div className="progress-value-container">
          <div>
            <img className="progress-arrow" src={require('../../../assets/images/blue-upload-icon.png')} alt="Upload arrow" />
          </div>
          <div>
            <p className="ui-speed-label-blue">4.8 Mb/s</p>
          </div>
        </div>
        <div className="router-traffic-progress-gradient ui-gradient-blue">
          <div className="ui-progress-number-container">
            <div>
              <p>1</p>
            </div>
            <div>
              <p>5</p>
            </div>
            <div>
              <p>20</p>
            </div>
          </div>
          <span className="ui-progress-background"></span>
        </div>
      </div>
    </div>
  )
}

export default TrafficProgressBars;