import React from 'react';
import './style.css';
import BandwidthGraphSnapshot from '../../graphs/bandwidthGraphSnapshot';

const DeviceDisplay = ({ lanView, deviceServicesDropdown, setDeviceServicesDropdown, lanDeviceData }) => {
  if (lanView) {
    return (
      <div className="lan-device-column">
        <ul className="lan-device-list">
          {
            lanDeviceData.map((el, index) => (
              <li className={deviceServicesDropdown === index ? 'lan-device-list-active-dropdown' : ''} onClick={() => {
                if (deviceServicesDropdown === index) {
                  setDeviceServicesDropdown(-1)
                }
                else {
                  setDeviceServicesDropdown(index)
                }
              }
              }>
                <div className="lan-device-list-main">
                  <div className="router-traffic-list-image-label-wrapper">
                    <div className="lan-device-list-image-wrapper">
                      <img src={require('../../../assets/images/' + el.deviceIcon + '-icon.png')} alt="Device" className="router-traffic-list-image" />
                    </div>
                    <div className="router-traffic-list-label">
                      <p>{el.deviceName}</p>
                    </div>
                  </div>
                  <div className="router-graph-arrow-wrapper">
                    <div className="router-graph">
                      <BandwidthGraphSnapshot />
                    </div>
                    <div className="lan-device-list-arrow-image-container">
                      <img className="lan-device-list-arrow-image" src={deviceServicesDropdown === index ? require('../../../assets/images/arrow-gray-down.png') : require('../../../assets/images/arrow-gray-up.png')} alt="Dropdown arrow" />
                    </div>
                  </div>
                </div>
                <ul className={`lan-device-list-service-dropdown ${deviceServicesDropdown === index ? 'dropdown-visible' : 'dropdown-hidden'}`}>
                  {
                    el.deviceDropdown.map((el) => (
                      
                      <li className="router-traffic-item-hover">
                        <div className="lan-dropdown-list-item-inner">
                          <div className="router-traffic-list-image-label-wrapper">
                            <div className="lan-dropdown-image-wrapper">
                              <img src={require('../../../assets/images/' + el.logo + '-logo.png')} alt="Netflix logo" className="lan-dropdown-image" />
                            </div>
                            <div className="lan-dropdown-label">
                              <p>{el.service}</p>
                            </div>
                          </div>
                          <div className="lan-dropdown-graph">
                            <BandwidthGraphSnapshot />
                          </div>
                        </div>
                      </li>
                      
                    ))
                  }
                    </ul>
                
                
              </li>
            ))
          }
        </ul>
      </div>
    )
  }
  return null;
}

export default DeviceDisplay;