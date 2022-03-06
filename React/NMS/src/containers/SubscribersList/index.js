import React from 'react';
import SubscribersListHeader from '../../components/SubscribersList/SubscribersListHeader';
import SubscribersListTable from '../../components/SubscribersList/SubscribersListTable';
import './style.css';

const SubscribersList = () => {
  return (
    <div className="ui-top-content">
      <SubscribersListHeader />
      <div className="subscribers-list-wrapper">
      <SubscribersListTable />
      </div>
    </div>
  )
};

export default SubscribersList;