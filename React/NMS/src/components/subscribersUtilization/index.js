import './style.css';
import React from 'react';
import DownloadArrow from '../../assets/images/arrow-dl.png';
import UploadArrow from '../../assets/images/arrow-up.png';
import UtilizationProgressBar from './UtilizationProgressBar';

const SubscribersUtilization = ({ }) => (
  <div id="utilization" className="ui-box ui-progress-box">
    <UtilizationProgressBar limit={444} usage={11.07} month={'june'}/>
    <div className="ui-speed">
      <div className="ui-speed-inner">
        <div className="ui-speed-value-container">
          <div>
            <img className="ui-speed-arrow" src={DownloadArrow} alt="Download arrow" />
          </div>
          <div>
            <p className="ui-speed-label-green">21.4 Mb/s</p>
          </div>
        </div>
        <div className="ui-progress-gradient ui-gradient-green">
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
      <div className="ui-speed-inner ">
        <div className="ui-speed-value-container">
          <div>
            <img className="ui-speed-arrow" src={UploadArrow} alt="Upload arrow" />
          </div>
          <div>
            <p className="ui-speed-label-blue">4.8 Mb/s</p>
          </div>
        </div>
        <div className="ui-progress-gradient ui-gradient-blue">
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
  </div>
)

export default SubscribersUtilization;