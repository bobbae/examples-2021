import React from 'react';
import './accounts.css';
import AccountsHeader from '../../components/accountsHeader';
import ActiveAccountsList from '../../components/activeAccountsList';
import AccountOrganizationsList from '../../components/accountOrganizationsList';
import SwitchAccountsList from '../../components/switchAccountsList';

function Accounts() {
  return (
    <div className="ui-top-content">
      <AccountsHeader />
      <div className="ui-account-wrapper">
        <ActiveAccountsList />
        <AccountOrganizationsList />
        <SwitchAccountsList />
      </div>
    </div>
  );
}

export default Accounts;