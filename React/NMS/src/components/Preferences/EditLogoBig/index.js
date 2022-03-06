import React from 'react';
import './style.css';

const EditLogoBig = () => {
  return (
    <div className="ui-logo-edit">
      <div className="ui-logo-current-wrapper">
        <div className="ui-logo-current-image-wrapper">
          <img className="ui-logo-current-image" src={require('../../../assets/images/amazon-logo.png')} className="ui-logo-current" alt="Logo" />
        </div>
        <div className="ui-logo-edit-button">
          <p>EDIT</p>
        </div>
      </div>
    </div>
  )
}

export default EditLogoBig;