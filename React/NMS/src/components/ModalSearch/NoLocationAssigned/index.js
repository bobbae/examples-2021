import React from 'react';
import './style.css';

const NoLocationAssigned = ({ isLocationAssigned }) => {
  if (!isLocationAssigned) {
    return (
      <div className="modal-search-no-location-container">
        <div className="no-location-image-wrapper">
          <img className="no-location-image" src={require(`../../../assets/images/no-location.png`)} alt="No location assigned" />
        </div>
        <div className="no-location-text">
          <p>No Location Assigned</p>
        </div>
      </div>
    )
  }
  return null;
}

export default NoLocationAssigned;