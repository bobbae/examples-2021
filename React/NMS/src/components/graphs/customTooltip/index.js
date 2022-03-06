import React from 'react';
import './style.css';

const CustomTooltip = ({ active, payload, label }) => {
  const checkSingle = (val) => {
    if (val.length === 1) {
      return true;
    }
  }
  if (active) {
    return (
      <div className="custom-tooltip" >
        <div className="ui-tooltip-top">
          <div className="ui-tooltip-label">
            <p>{label}</p>
          </div>
          {
            !checkSingle(payload) &&
            <div className="ui-tooltip-values">
              <div className="ui-tooltip-green">
                <p>{payload[0].value} Kb</p>
              </div>
              <div className="ui-tooltip-blue">
                <p>{payload[1].value} Mb</p>
              </div>
            </div>
          }
          {
            checkSingle(payload) &&
            <div className="ui-tooltip-values">
              <div className="ui-tooltip-blue">
                <p>{payload[0].value} Kb</p>
              </div>
            </div>
          }
        </div>
        <div className="ui-tooltip-body">
          <div className="ui-tooltip-row">
            <div className="ui-tooltip-image-wrapper">
              <img className="ui-tooltip-image" src={require('../../../assets/images/arrow-dl.png')} alt="Tooltip user" />
            </div>
            <div className="ui-tooltip-row-label">
              <p>Alice</p>
            </div>
          </div>
          <div className="ui-tooltip-row">
            <div className="ui-tooltip-image-wrapper">
              <img className="ui-tooltip-image" src={require('../../../assets/images/smiley-happy.png')} alt="Tooltip service" />
            </div>
            <div className="ui-tooltip-row-label">
              <p>Netflix Streaming Services</p>
            </div>
          </div>
        </div>
        <div className="ui-tooltip-line">
          <div className="ui-tooltip-line-bottom"></div>
        </div>
      </div>
    );
  }
  return null;
};

export default CustomTooltip;