import React from 'react';
import './style.css';

const MainHeader = ({ mainLabel, leftSide, rightSide, hasSearch }) => {
  if (!hasSearch) {
    return (
      <div className="ui-main-header">
        <div className="ui-main-header-side">
          {leftSide}
        </div>
        <div className="ui-main-header-label">
          <p>{mainLabel}</p>
        </div>
        <div className="ui-main-header-side">
          {rightSide}
        </div>
      </div>
    )
  }
  return (
    <div>
      usli smo pod search
    </div>
  )
};

export default MainHeader;