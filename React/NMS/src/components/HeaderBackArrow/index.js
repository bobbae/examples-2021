import React from 'react';
import './style.css';

const HeaderBackArrow = ({ labelValue, boldLabel }) => (
  <div className="header-back-container">
    <div className="header-back-arrow-container">
      <img src={require(`../../assets/images/arrow-left.png`)} className="header-back-arrow" alt="Back" />
    </div>
    {
      labelValue &&
      <div className={boldLabel ? `header-back-label-bold` : `header-back-label`}>
        <p>{labelValue}</p>
      </div>
    }
  </div>
);

export default HeaderBackArrow;