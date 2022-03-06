import React from 'react';
import TopContentSearch from '../../components/topContentSearch';
import NetworksTableNew from '../../components/networksTable/networksTableNew';
import ModalSearch from '../../components/ModalSearch';

function Networks() {
  return (
    <div className="ui-top-content">
      <TopContentSearch />
      <ModalSearch />
      <NetworksTableNew />
    </div>
  );
}

export default Networks;