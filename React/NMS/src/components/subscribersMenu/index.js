import './style.css';
import React from 'react';
import DropdownArrow from '../../assets/images/dropdown-arrow.png';

const SubscribersMenu = ({ }) => (

  <div id="subscribers-menu" className="ui-subscribers-menu ui-box ">
    <div>
      <a className="ui-green-item " href="# ">
        <div className="ui-subscribers-menu-label">
          <p>Status</p>
        </div>
        <div className="ui-subscribers-menu-text-active">
          <p>Active</p>
        </div>
      </a>
    </div>
    <div>
      <a className="ui-blue-item " href="# ">
        <div className="ui-subscribers-menu-label">
          <p>Acnowledge</p>
        </div>
        <div className="ui-subscribers-details-wrapper">
          <div className="ui-subscribers-menu-text-blue">
            <p>Indefinitely</p>
          </div>
          <div className="ui-details-image-wrapper">
            <img className="ui-details-arrow" src={DropdownArrow} alt="Details arrow" />
          </div>
        </div>
      </a>
    </div>
    <div>
      <a href="# ">
        <div className="ui-subscribers-menu-label">
          <p>Public IP</p>
        </div>
        <div className="ui-subscribers-menu-text">
          <p>99.433.212.12</p>
        </div>
      </a>
    </div>
    <div>
      <a href="# ">
        <div className="ui-subscribers-menu-label">
          <p>Serial & IP</p>
        </div>
        <div className="ui-menu-adress-container">
          <div className="ui-subscribers-menu-text">
            <p>64FE345GD</p>
          </div>
          <div className="ui-subscribers-menu-text">
            <p>172.231.121.1</p>
          </div>
        </div>
      </a>
    </div>
    <div>
      <a href="# ">
        <div className="ui-subscribers-menu-label">
          <p>Last Self Reg</p>
        </div>
        <div className="ui-subscribers-menu-text">
          <p>39 minutes ago</p>
        </div>
      </a>
    </div>
    <div>
      <a href="# ">
        <div className="ui-subscribers-menu-label">
          <p>RouterOS</p>
        </div>
        <div className="ui-subscribers-menu-text">
          <p>6.55.3</p>
        </div>
      </a>
    </div>
    <div>
      <a href="# ">
        <div className="ui-subscribers-menu-label">
          <p>RouterCRM</p>
        </div>
        <div className="ui-subscribers-menu-text">
          <p>Miller-40222</p>
        </div>
      </a>
    </div>
    <div>
      <a href="# ">
        <div className="ui-subscribers-menu-label">
          <p>ISP Plan</p>
        </div>
        <div className="ui-subscribers-menu-text">
          <p>Cloudhouse Fiberwave</p>
        </div>
      </a>
    </div>
  </div>
)

export default SubscribersMenu;