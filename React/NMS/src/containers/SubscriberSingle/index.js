import React, { useState } from 'react';
import './style.css';
import MainHeader from '../../components/MainHeader';
import SimpeMap from '../../components/simpeMap';
import IspPlansEdit from '../../components/IspPlans/IspPlansEdit';
import HeaderBackArrow from '../../components/HeaderBackArrow';
import DropdownOptions from '../../components/DropdownOptions';
import Modal from 'react-modal';
import EditSubscriber from '../../components/EditSubscriber';
import SendInviteModal from '../../components/SendInviteModal';

const subscriberDetails = [
  { label: `CRM REF`, value: `3442312` },
  { label: `E-mail`, value: `ofer@smart.network` },
  { label: `First Name`, value: `Ofer` },
  { label: `Last Name`, value: `Tenenbaum` },
  { label: `Company`, value: `smart.network` },
  { label: `Tech Code`, value: `test` },
  { label: `Tech Mode`, value: `ON` },
];

const SubscriberSingle = () => {
  const [ modalOpen, setModalOpen ] = useState(false);
  const [ sendInvite, setSendInvite ] = useState(false);
  return (
    <div className="ui-top-content">
      <Modal isOpen={false}>
        <EditSubscriber />
      </Modal>
      <MainHeader leftSide={<HeaderBackArrow labelValue='Subscribers' />} mainLabel='Ofer Tenenbaum' rightSide='joooojj' />

      <div className="single-subscriber-wrapper">
        <div style={{ margin: '10px' }} className="ROUTER-LIST">

          <div onClick={() =>setSendInvite(true)} className="subscriber-router-list-header">
            <p>ROUTERS</p>
          </div>
          <SendInviteModal sendInvite={sendInvite}/>
          <ul className="subscriber-router-list">
            <li>
              <div className="subscriber-router-list-item">
                <div className="name-status-container">
                  <div className="name-container">
                    <p>OFER</p>
                  </div>
                  <div className="status-container">
                    <div className="ui-active-field">
                      <p>ACTIVE</p>
                    </div>
                    <div className="ui-vip-clients-field">
                      <p>VIP CLIENTS</p>
                    </div>
                    <div className="ui-more-field">
                      <p>5 MORE...</p>
                    </div>
                  </div>
                </div>

                <div className="traffic-bars-arrow-container">
                  <div>
                    <p>BARS PLACEHOLDER</p>
                  </div>
                  <div className="router-list-arrow-container">
                    <img className="router-list-arrow" src={require(`../../assets/images/router-list-arrow-right.png`)} />
                  </div>
                </div>
              </div>
            </li>
            <li>
              <div className="subscriber-router-list-item">
                <div className="name-status-container">
                  <div className="name-container">
                    <p>OFER</p>
                  </div>
                  <div className="status-container">
                    <div className="ui-active-field">
                      <p>ACTIVE</p>
                    </div>
                    <div className="ui-vip-clients-field">
                      <p>VIP CLIENTS</p>
                    </div>
                    <div className="ui-more-field">
                      <p>5 MORE...</p>
                    </div>
                  </div>
                </div>

                <div className="traffic-bars-arrow-container">
                  <div>
                    <p>BARS PLACEHOLDER</p>
                  </div>
                  <div className="router-list-arrow-container">
                    <img className="router-list-arrow" src={require(`../../assets/images/router-list-arrow-right.png`)} />
                  </div>
                </div>
              </div>
            </li>
            <li>
              <div className="subscriber-router-list-item">
                <div className="name-status-container">
                  <div className="name-container">
                    <p>OFER</p>
                  </div>
                  <div className="status-container">
                    <div className="ui-active-field">
                      <p>ACTIVE</p>
                    </div>
                    <div className="ui-vip-clients-field">
                      <p>VIP CLIENTS</p>
                    </div>
                    <div className="ui-more-field">
                      <p>5 MORE...</p>
                    </div>
                  </div>
                </div>

                <div className="traffic-bars-arrow-container">
                  <div>
                    <p>BARS PLACEHOLDER</p>
                  </div>
                  <div className="router-list-arrow-container">
                    <img className="router-list-arrow" src={require(`../../assets/images/router-list-arrow-right.png`)} />
                  </div>
                </div>
              </div>
            </li>
          </ul>
        </div>
        <div style={{ margin: '5px' }}>
          <SimpeMap />
        </div>
        <div className="subscriber-details-wrapper">
          <div className="subscriber-details-header">
            <p >SUBSCRIBER DETAILS</p>
            {/* <DropdownOptions firstButtonText='Add Router' secondButtonText='Edit' thirdButtonText='Delete' arrowLeft={true}/> */}
          </div>

          <ul className="subscriber-details-list">
            {
              subscriberDetails.map((el) => (
                <li onClick={<DropdownOptions />}>
                  <div className="subscriber-details-list-item">
                    <div className="subscriber-details-label">
                      <p>{el.label}</p>
                    </div>

                    <div className="subscriber-details-value">
                      <p>{el.value}</p>
                    </div>
                  </div>
                </li>
              ))
            }
          </ul>
        </div>
      </div>
    </div>
  )
};

export default SubscriberSingle;