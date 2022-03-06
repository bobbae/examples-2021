import React from 'react';
import ServiceGraph from '../ServiceGraph'
import './style.css';

const ServiceDisplay = ({ lanView, setLanView, serviceView, serviceData }) => {
  if (serviceView !== -1) {
    return (
      <div className={lanView ? 'service-column-third' : 'service-column-full'}>
        <div className="service-column-full-inner">
          <div className="service-image-label-container">
            <div className="service-image-container">
              <img className="service-image" src={require('../../../assets/images/' + serviceData.logo + '-logo-service.png')} alt={serviceData.service + ' logo'} />
            </div>
            <div className="service-label">
              <p>{serviceData.service}</p>
            </div>
          </div>
          <div className="service-traffic-box">
            <div className="service-gray-label">
              <p>TRAFFIC (MBIT)</p>
            </div>
            <div className="service-box" style={{paddingBottom: '10px'}}>
              <ServiceGraph />
            </div>
          </div>
          <div className="service-access-data-box">
            <div className="service-gray-label">
              <p>ACCESS DATA</p>
            </div>
            <div className="service-box">
              <div className="reveal-data-text">
                <p>Reveal more data by accessing this user’s network.</p>
              </div>
              <div onClick={() => setLanView(true)} className="request-lan-button">
                <p>Request LAN view</p>
              </div>
            </div>
            <div className="ask-customer-label">
              <p>Ask Customer to keep the app opened to receive the request</p>
            </div>
          </div>
          <div className="service-list-container">
            <ul className="service-list">
              <li>
                <div className="service-list-item">
                  <p>Source IP</p>
                </div>
                <div className="service-list-item-value">
                  <p>Data</p>
                </div>
              </li>
              <li>
                <div className="service-list-item">
                  <p>Source Port</p>
                </div>
                <div className="service-list-item-value">
                  <p>Data</p>
                </div>
              </li>
              <li>
                <div className="service-list-item">
                  <p>Destination IP</p>
                </div>
                <div className="service-list-item-value">
                  <p>Data</p>
                </div>
              </li>
              <li>
                <div className="service-list-item">
                  <p>Destination Port</p>
                </div>
                <div className="service-list-item-value">
                  <p>Data</p>
                </div>
              </li>
            </ul>
          </div>
        </div>
      </div>
    )
  }
  return null;
}

export default ServiceDisplay;