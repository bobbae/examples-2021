import './style.css';
import React from 'react';
import CheckIconBlue from '../../assets/images/check-blue.png';

const AccountOrganizationsList = ({ }) => (

  <div>
    <div className="ui-organization-list-label">
      <p>ORGANIZATIONS</p>
    </div>
    <ul className="ui-organization-list ui-multi-list">
      <li>
        <div className="ui-organization-wrapper">
          <div className="ui-organization-logo-wrapper ui-org-logo-green">
            <p>V</p>
          </div>
          <div className="ui-organization-name">
            <p>Valley Internet</p>
          </div>
        </div>
        <div>
          <img src={CheckIconBlue} alt="Check" />
        </div>
      </li>
      <li>
        <div className="ui-organization-wrapper">
          <div className="ui-organization-logo-wrapper ui-org-logo-blue">
            <p>A</p>
          </div>
          <div className="ui-organization-name">
            <p>Apple Test Org</p>
          </div>
        </div>
        <div>
          <img src={CheckIconBlue} alt="Check" />
        </div>
      </li>
      <li>
        <div className="ui-organization-wrapper">
          <div className="ui-organization-logo-wrapper ui-org-logo-orange">
            <p>T</p>
          </div>
          <div className="ui-organization-name">
            <p>Test Org</p>
          </div>
        </div>
        <div>
          <img src={CheckIconBlue} alt="Check" />
        </div>
      </li>
    </ul>
  </div>
)

export default AccountOrganizationsList;