import React, { useState } from 'react';
import IspPlansCreateNew from '../../components/IspPlans/IspPlansCreateNew';
import IspPlansEdit from '../../components/IspPlans/IspPlansEdit';
import IspPlansHeader from '../../components/IspPlans/IspPlansHeader';
import ReactModal from 'react-modal';
import './style.css';
import BasicCounter from '../../components/BasicCounter';

const data = [
  {},
  {},
];

const IspPlans = () => {
  const [createNew, setCreateNew] = useState(false);
  return (
    <div className="ui-top-content">
      <ReactModal isOpen={createNew}>
        <IspPlansCreateNew />
      </ReactModal>
      <IspPlansHeader createNew={createNew} setCreateNew={setCreateNew} />
      <div className="ui-isp-plans-wrapper">
        <ul className="isp-plans-list">
          {
            data.map((el) =>
              <li>
                <div className="isp-plans-list-item">
                  <div className="isp-label-info-container">
                    <div className="isp-label">
                      <p>$144 Smartwave</p>
                    </div>
                    <div className="isp-info">
                      <div className="isp-networks">
                        <p>45 networks</p>
                      </div>
                      <div className="isp-download">
                        <div className="isp-download-image-wrapper">
                          <img className="isp-download-image" src={require(`../../assets/images/download-gray.png`)} />
                        </div>
                        <div className="isp-download-value">
                          <p>24 Mbit</p>
                        </div>
                      </div>
                      <div className="isp-upload">
                        <div className="isp-upload-image-wrapper">
                          <img className="isp-upload-image" src={require(`../../assets/images/upload-gray.png`)} />
                        </div>
                        <div className="isp-upload-value">
                          <p>4 Mbit</p>
                        </div>
                      </div>
                    </div>
                  </div>
                  <div className="isp-actions-button">
                    <div className="isp-actions-image-wrapper">
                      <img className="isp-actions-image" src={require(`../../assets/images/info.png`)} />
                    </div>
                    <IspPlansEdit />
                  </div>
                </div>
              </li>
            )
          }
        </ul>
      </div>
      <BasicCounter />
    </div>
  )
};

export default IspPlans;