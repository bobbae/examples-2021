import React from 'react';
import SearchBar from '../../../components/searchBar';
import './style.css';

const SubscribersListHeader = () => {
  return (
    <div className="subscribers-header-container">
      <div className="subscribers-header-label-icons">
        <div className="subscribers-header-icon-wrapper">
          <img className="subscribers-header-icon" src={require(`../../../assets/images/check-circle.png`)} alt="Confirm" />
        </div>
        <div className="subscribers-header-label">
          <p>Subscribers</p>
        </div>
        <div className="subscribers-header-icon-wrapper">
          <img className="subscribers-header-icon" src={require(`../../../assets/images/add-new-blue.png`)} alt="Create new" />
        </div>
      </div>
      <SearchBar searchPlaceholder='Search subscriber...' />
    </div>
  )
};

export default SubscribersListHeader;