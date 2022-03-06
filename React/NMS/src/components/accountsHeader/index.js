import './style.css';
import React from 'react';

const AccountsHeader = ({ }) => (

  <div className="ui-account-header">
    <div className="ui-account-cancel-label">
      <p>Cancel</p>
    </div>
    <div className="ui-account-main-label">
      <p>Account </p>
    </div>
    <div className="ui-account-done-label">
      <p>Done</p>
    </div>
  </div>
)

export default AccountsHeader;