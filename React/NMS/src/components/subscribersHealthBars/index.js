import './style.css';
import React from 'react';
import SmileyHappy from '../../assets/images/smiley-happy.png';

const SubscribersHealthBars = ({ }) => (

  <div id="info-bars" className="ui-box ui-info-box ">
    <div className="ui-bar-container ">
      <div className="ui-bar-container-label ">
        <p>OVERALL INTERNET HEALTH</p>
      </div>
      <div className="ui-smiley-bar ">
        <div>
          <img className="ui-smiley-image " src={SmileyHappy} alt="Happy smiley face" />
        </div>
        <div className="ui-bar-green ">
        </div>
        <div className="ui-bar-green ">
        </div>
        <div className="ui-bar-green ">
        </div>
        <div className="ui-bar-green ">
        </div>
        <div className="ui-bar-green ">
        </div>
        <div className="ui-bar-green ">
        </div>
        <div className="ui-bar-green ">
        </div>
        <div className="ui-bar-green ">
        </div>
        <div className="ui-bar-green ">
        </div>
        <div className="ui-bar-green ">
        </div>
        <div className="ui-bar-green ">
        </div>
        <div className="ui-bar-green ">
        </div>
      </div>
    </div>
    <div className="ui-bar-container ">
      <div className="ui-bar-container-label ">
        <p>SHORT TERM PACKET LOSS</p>
      </div>
      <div>
        <div className="ui-data-info ">
          <div>
            <p>2%</p>
          </div>
          <div>
            <p>0%</p>
          </div>
        </div>
        <div className="ui-data-bar ">
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
        </div>
      </div>
    </div>
    <div className="ui-bar-container ">
      <div className="ui-bar-container-label">
        <p>LONG TERM PACKET LOSS</p>
      </div>
      <div>
        <div className="ui-data-info ">
          <div>
            <p>2%</p>
          </div>
          <div>
            <p>0%</p>
          </div>
        </div>
        <div className="ui-data-bar ">
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
          <div className="ui-bar-green ">
          </div>
        </div>
      </div>
    </div>
    <div className="ui-bar-container">
      <div className="ui-bar-container-label">
        <p>LATENCY</p>
      </div>
      <div>
        <div className="ui-data-info">
          <div>
            <p>80 ms</p>
          </div>
          <div>
            <p>20 ms</p>
          </div>
        </div>
        <div className="ui-data-bar ">
          <div className="ui-bar-orange ">
          </div>
          <div className="ui-bar-orange ">
          </div>
          <div className="ui-bar-orange ">
          </div>
          <div className="ui-bar-orange ">
          </div>
          <div className="ui-bar-orange ">
          </div>
          <div className="ui-bar-orange ">
          </div>
          <div className="ui-bar-orange ">
          </div>
          <div className="ui-bar-orange ">
          </div>
          <div className="ui-bar-orange ">
          </div>
          <div className="ui-bar-orange-empty ">
          </div>
          <div className="ui-bar-orange-empty ">
          </div>
          <div className="ui-bar-orange-empty ">
          </div>
        </div>
      </div>
    </div>
    <div className="ui-bar-container ">
      <div className="ui-dot-container ">
        <div className="ui-dot ui-dot-red ">
        </div>
        <div>
          <p>MTU</p>
        </div>
      </div>
      <div className="ui-dot-container ">
        <div className="ui-dot ui-dot-red ">
        </div>
        <div>
          <p>DNS 1</p>
        </div>
      </div>
      <div className="ui-dot-container ">
        <div className="ui-dot ui-dot-green ">
        </div>
        <div>
          <p>DNS 2</p>
        </div>
      </div>
    </div>
  </div>
)

export default SubscribersHealthBars;