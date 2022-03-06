import './style.css';
import React from 'react';
import SearchBar from '../../../components/searchBar';

const IspPlansHeader = ({setCreateNew, createNew}) => {
  return (
    <div className="isp-header-container">
      <div className="isp-header-label-icons">
        <div className="isp-header-icon-wrapper">
          <img className="isp-header-icon" src={require(`../../../assets/images/check-circle.png`)} alt="Confirm" />
        </div>
        <div className="isp-header-label">
          <p>ISP Plans</p>
        </div>
        <div onClick={() => setCreateNew(!createNew)} className="isp-header-icon-wrapper">
          <img className="isp-header-icon" src={require(`../../../assets/images/add-new-blue.png`)} alt="Create new" />
        </div>
      </div>
      <SearchBar searchPlaceholder='Search ISP...' />
    </div>
  )
};

export default IspPlansHeader;