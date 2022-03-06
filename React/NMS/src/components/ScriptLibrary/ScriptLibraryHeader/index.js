import './style.css';
import React from 'react';
import SearchBar from '../../../components/searchBar';

const ScriptLibraryHeader = ({setCreateNew, createNew}) => {
  return (
    <div className="isp-header-container">
      <div className="isp-header-label-icons">
        <div className="isp-header-icon-wrapper">
          <img className="isp-header-icon" src={require(`../../../assets/images/check-circle.png`)} alt="Confirm" />
        </div>
        <div className="isp-header-label">
          <p>Script Library</p>
        </div>
        <div onClick={() => setCreateNew(!createNew)} className="isp-header-icon-wrapper">
          <img className="isp-header-icon" src={require(`../../../assets/images/add-new-blue.png`)} alt="Create new" />
        </div>
      </div>
      <SearchBar searchPlaceholder='Search script...' />
    </div>
  )
};

export default ScriptLibraryHeader;