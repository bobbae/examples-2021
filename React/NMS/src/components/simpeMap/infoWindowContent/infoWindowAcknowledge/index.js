import React from 'react';
import './style.css';
import CheckIconBlue from '../../../../assets/images/check-blue.png';
import ArrowLeft from '../../../../assets/images/arrow-left.png';

const InfoWindowAcknowledge = ({ onAction }) => {
  return (
    <div className="info-window-main">
      <div className="ui-info-window-header">
        <div className="ui-acknowledge-back">
          <div>
            <img className="ui-back-arrow" src={ArrowLeft} alt="Back arrow" />
          </div>
          <div className="ui-acknowledge-window-back-label" onClick={onAction}>
            <p>Details</p>
          </div>
        </div>
        <div className="ui-info-window-main-label">
          <p>Acknowledge</p>
        </div>
        <div className="ui-acknowledge-pseudo-flex"></div>
      </div>
      <div className="ui-info-window-body">
        <div className="ui-acknowledge-window-row">
          <div>
            <a href="# ">
              <div className="acknowledge-row-label">
                <p>Acknowledge</p>
              </div>
              <div className="acknowledge-value-wrapper">
                <div className="acknowledge-row-value-text">
                  <label className="form-switch">
                    <input type="checkbox" />
                    <i></i>
                  </label>
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
                <div className="notes-items-note-label">
                  <p>Note</p>
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
        <div className="ui-info-window-inactivity-row">
          <div className="ui-info-window-row-label">
            <p>ACKNOWLEDGE INACTIVITY</p>
          </div>
          <div className="inactivity-items">
            <div className="inactivity-items-row">
              <a href="# ">
                <div className="inactivity-items-label">
                  <p>Until status change</p>
                </div>
              </a>
            </div>
            <div className="inactivity-items-row">
              <a href="# ">
                <div className="inactivity-items-label">
                  <p>Indefinitely</p>
                </div>
                <div>
                  <img className="inactivity-check-image" src={CheckIconBlue} alt="Blue check" />
                </div>
              </a>
            </div>
            <div className="inactivity-items-row">
              <a href="# ">
                <div className="inactivity-items-label">
                  <p>Until</p>
                </div>
              </a>
            </div>
          </div>
        </div>
      </div>
    </div>
  )
};

export default InfoWindowAcknowledge;