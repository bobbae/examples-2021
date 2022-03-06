import React, { useState } from 'react';
import './style.css';
import RouterIconBig from '../../../assets/images/router-icon-big.png';
import DownloadArrow from '../../../assets/images/arrow-dl.png';
import UploadArrow from '../../../assets/images/arrow-up.png';
import SmileyHappy from '../../../assets/images/smiley-happy.png';
import DropdownArrow from '../../../assets/images/dropdown-arrow.png';
import InfoWindowAcknowledge from './infoWindowAcknowledge';

const InfoWindowContent = ({ }) => {
  const [acknowledge, setAcknowledge] = useState(false);
  return (
    <div>
      {
        !acknowledge &&
        <div className="info-window-main">
          <div className="ui-info-window-header">
            <div className="ui-pseudo-flex"></div>
            <div className="ui-info-window-main-label">
              <p>Details</p>
            </div>
            <div className="ui-info-window-done-label">
              <p>Done</p>
            </div>
          </div>
          <div className="ui-info-window-body">
            <div className="ui-info-window-router-row">
              <div className="router-image-container">
                <img className="router-image" src={RouterIconBig} alt="Router" />
              </div>
              <div>
                <div className="router-label-container">
                  <p>Router Name</p>
                </div>
                <div className="router-status">
                  <div className="router-active-field">
                    <p>ACTIVE</p>
                  </div>
                </div>
              </div>
            </div>
            <div className="ui-info-window-traffic-row">
              <div className="ui-info-window-row-label">
                <p>TRAFFIC</p>
              </div>
              <div className="traffic-speed-container">
                <div className="traffic-speed-inner">
                  <div className="traffic-speed-value-container">
                    <div>
                      <img className="traffic-speed-arrow" src={DownloadArrow} alt="Download arrow" />
                    </div>
                    <div>
                      <p className="traffic-speed-label-green">21.4 Mb/s</p>
                    </div>
                  </div>
                  <div className="traffic-progress-gradient traffic-gradient-green">
                    <div className="traffic-progress-number-container">
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
                    <span className="traffic-progress-background"></span>
                  </div>
                </div>
                <div className="traffic-speed-inner ">
                  <div className="traffic-speed-value-container">
                    <div>
                      <img className="traffic-speed-arrow" src={UploadArrow} alt="Upload arrow" />
                    </div>
                    <div>
                      <p className="traffic-speed-label-blue">4.8 Mb/s</p>
                    </div>
                  </div>
                  <div className="traffic-progress-gradient traffic-gradient-blue">
                    <div className="traffic-progress-number-container">
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
                    <span className="traffic-progress-background"></span>
                  </div>
                </div>
              </div>
            </div>
            <div className="ui-info-window-health-row">
              <div className="ui-info-window-row-label">
                <p>HEALTH</p>
              </div>
              <div className="health-container">
                <div className="health-smiley-bar ">
                  <div>
                    <img className="health-smiley-image " src={SmileyHappy} alt="Happy smiley face" />
                  </div>
                  <div className="health-bar-green">
                  </div>
                  <div className="health-bar-green">
                  </div>
                  <div className="health-bar-green">
                  </div>
                  <div className="health-bar-green">
                  </div>
                  <div className="health-bar-green">
                  </div>
                  <div className="health-bar-green">
                  </div>
                  <div className="health-bar-green">
                  </div>
                  <div className="health-bar-green">
                  </div>
                  <div className="health-bar-green">
                  </div>
                  <div className="health-bar-green">
                  </div>
                  <div className="health-bar-green">
                  </div>
                  <div className="health-bar-green">
                  </div>
                </div>
              </div>
            </div>
            <div className="ui-info-window-acknowledge-row">
              <div>
                <a href="# " onClick={() => setAcknowledge(true)}>
                  <div className="acknowledge-row-label">
                    <p>Acknowledge</p>
                  </div>

                  <div className="acknowledge-value-wrapper">
                    <div className="acknowledge-row-value-text">
                      <p>Indefinitely</p>
                    </div>
                    <div className="acknowledge-value-image-wrapper">
                      <img className="acknowledge-details-arrow" src={DropdownArrow} alt="Details arrow" />
                    </div>
                  </div>
                </a>
              </div>
            </div>
            <div className="ui-info-window-notes-row">
              <div className="ui-info-window-row-label">
                <p>NOTES</p>
              </div>
              <div className="notes-items">
                <div className="notes-items-row">
                  <a href="# ">
                    <div className="notes-items-label">
                      <p>Technical issue</p>
                    </div>
                  </a>
                </div>
                <div className="notes-items-row">
                  <a href="# ">
                    <div className="notes-items-gray-text">
                      <p>Ofer Tenenbaum</p>
                    </div>
                  </a>
                </div>
              </div>
            </div>
          </div>
        </div>}
      {
        acknowledge &&
        <InfoWindowAcknowledge onAction={() => setAcknowledge(false)} />
      }
    </div>
  );
}

export default InfoWindowContent;