import './style.css';
import React from 'react';
import MagnifyingGlass from '../../assets/images/magnifying-icon.png';

const SearchBar = ({ searchPlaceholder }) => (
  <div className="ui-search-big">
    <div className="ui-search-icon-wrapper">
      <img src={MagnifyingGlass} alt="Magnifying glass icon" />
    </div>
    <div className="ui-input-wrapper">
      <input type="text" placeholder={searchPlaceholder} />
    </div>
  </div>
)

export default SearchBar;