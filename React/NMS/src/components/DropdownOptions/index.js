import React from 'react';
import './style.css';

const DropdownOptions = ({ arrowLeft, arrowRight, firstButtonText, secondButtonText, thirdButtonText }) => {
  let arrowPosition;
  if (arrowLeft) {
    arrowPosition = 'dropdown-left';
  } else if (arrowRight) {
    arrowPosition = 'dropdown-right';
  }
  return (
    <div className={`ui-dropdown-options ${arrowPosition}`}>
      <div className="ui-dropdown-options-button ui-button-blue">
        <p>{firstButtonText}</p>
      </div>
      <div className="ui-dropdown-options-button ui-button-blue">
        <p>{secondButtonText}</p>
      </div>
      <div className="ui-dropdown-options-button ui-button-red">
        <p>{thirdButtonText}</p>
      </div>
    </div>
  )
};

export default DropdownOptions;