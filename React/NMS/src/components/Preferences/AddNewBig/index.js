import React from 'react';
import './style.css';

const AddNewBig = () => {
  return (
    <div className="maintenance-add-new-button-wrapper">
      <div className="maintenance-add-new-button">
        <img className="maintenance-add-new-button-image" src={require('../../../assets/images/add-new-blue-big.png')} />
      </div>
    </div>
  )
}

export default AddNewBig;