import React from 'react';
import './style.css';

const ModalSearchHeader = () => {
  return (
    <div className="modal-search-header">
      <div className="modal-search-button">
        <div className="modal-search-button-image-wrapper">
          <img src={require(`../../../assets/images/hide-icon-blue.png`)} className="modal-search-button-image" alt="Hide" />
        </div>
        <div className="modal-search-button-text">
          <p>Hide</p>
        </div>
      </div>
    </div>
  )
};

export default ModalSearchHeader;