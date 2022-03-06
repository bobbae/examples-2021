import React from 'react';
import TrafficProgressBars from '../routerTraffic/SpeedProgressBars';
import ModalSearchHeader from './ModalSearchHeader';
import NoLocationAssigned from './NoLocationAssigned';
import './style.css';

const modalSearchData = [
  {
    networkName: `Bridant 403203`, isLocationAssigned: true, status: `😃`
  },
  {
    networkName: `Miller 11123`, isLocationAssigned: true, status: `😐`
  },
  {
    networkName: `Grace 5665`, isLocationAssigned: false, status: `🙂`
  },
  {
    networkName: `Antonio`, isLocationAssigned: false, status: `😰`
  },
  {
    networkName: `Smart Network`, isLocationAssigned: true, status: `😰`
  },
  {
    networkName: `London 111`, isLocationAssigned: true, status: `🙂`
  },
  {
    networkName: `Antonio`, isLocationAssigned: false, status: `😰`
  },
];

const ModalSearch = () => {
  return (
    <div className="modal-search-wrapper">
      <ModalSearchHeader />
      <div className="modal-search-body">
        <ul className="modal-search-list">
          {
            modalSearchData.map(el => (
              <li>
                <div className="modal-search-list-item">
                  <div className="modal-search-name-status-container">
                    <div className="modal-search-label">
                      <p>{el.networkName}</p>
                    </div>
                    <NoLocationAssigned isLocationAssigned={el.isLocationAssigned} />
                    <div className="modal-search-status-container">
                      <div className="modal-search-status-field active-field">
                        <p>ACTIVE</p>
                      </div>
                      <div className="modal-search-status-field vip-clients-field">
                        <p>VIP CLIENTS</p>
                      </div>
                      <div className="modal-search-status-field more-field">
                        <p>5 MORE...</p>
                      </div>
                    </div>
                  </div>
                  <div className="modal-search-bars-smiley-container">
                    <div className="modal-search-bars-container">
                      <TrafficProgressBars />
                    </div>
                    <div className="modal-search-smiley-container">
                      <p className="modal-search-smiley">{el.status}</p>
                    </div>
                  </div>
                </div>
              </li>
            ))
          }
        </ul>
      </div>
    </div>
  )
};

export default ModalSearch;