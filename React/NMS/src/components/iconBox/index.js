import './style.css';
import React from 'react';
import UserIcon from '../../assets/images/user.png';
import DropdownArrow from '../../assets/images/dropdown-arrow.png';

const IconBox = () => {
  return (
    <div className="ui-icon-box">
      <div className="ui-box-image-label-wrapper">
        <div className="ui-box-image-wrapper">
          <img className="ui-box-icon" src={UserIcon} alt="User icon" />
        </div>
        <div className="ui-icon-box-label">
          <p>Subscriber</p>
        </div>
      </div>
      <div className="ui-icon-box-text">
        <p>John Miller</p>
      </div>
      <div>
        <img className="ui-dropdown-arrow" src={DropdownArrow} alt="Dropdown arrow" />
      </div>
    </div>
  )
}

export default IconBox;