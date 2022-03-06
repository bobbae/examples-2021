import React from 'react';
import './style.css';

const data = [
  {
    name: `Arnold`,
    routers: `109`,
    company: `12/05/2019 @ 04:10 am`,
    email: `superuser@smart.network`,
    crmRef: `11111221111111`,
    lastSeen: `12:34 AM`,
  },
  {
    name: `Grace`,
    routers: `109`,
    company: `12/05/2019 @ 04:10 am`,
    email: `superuser@smart.network`,
    crmRef: `11111221111111`,
    lastSeen: `12:34 AM`,
  },
  {
    name: `Subject`,
    routers: `109`,
    company: `12/05/2019 @ 04:10 am`,
    email: `superuser@smart.network`,
    crmRef: `11111221111111`,
    lastSeen: `12:34 AM`,
  },
  {
    name: `Test`,
    routers: `109`,
    company: `12/05/2019 @ 04:10 am`,
    email: `superuser@smart.network`,
    crmRef: `11111221111111`,
    lastSeen: `12:34 AM`,
  },
  {
    name: `Beth`,
    routers: `109`,
    company: `12/05/2019 @ 04:10 am`,
    email: `superuser@smart.network`,
    crmRef: `11111221111111`,
    lastSeen: `12:34 AM`,
  },

]

const SubscribersListTable = () => {
  return (
    <div className="table-container">
      <table className="table-without-heading">
        <tr className="ui-network-labels">
          <th>
            <div>
              <p>Name</p>
            </div>
          </th>
          <th>
            <div>
              <p># Routers</p>
            </div>
          </th>
          <th>
            <div >
              <p>Company</p>
            </div>
          </th>
          <th>
            <div >
              <p>Email</p>
            </div>
          </th>
          <th>
            <div >
              <p>CRM Ref</p>
            </div>
          </th>
          <th>
            <div >
              <p>Last Seen</p>
            </div>
          </th>
        </tr>
        {
          data.map((el) => (
            <tr className="table-without-heading-row">
              <td>
                <div className="ui-network-status-container">
                  <div>
                    <div className="ui-network-label-container">
                      <p>{el.name}</p>
                    </div>
                    <div className="ui-network-status">
                      <div className="ui-active-field">
                        <p>ACTIVE</p>
                      </div>
                      <div className="ui-vip-clients-field">
                        <p>VIP CLIENTS</p>
                      </div>
                      <div className="ui-more-field">
                        <p>5 MORE...</p>
                      </div>
                    </div>
                  </div>
                </div>
              </td>
              <td>
                <div className="subscribers-basic-row-item">
                  <p>{el.routers}</p>
                </div>
              </td>
              <td>
                <div className="subscribers-basic-row-item">
                  <p>{el.company}</p>
                </div>
              </td>
              <td>
                <div className="subscribers-basic-row-item">
                  <p>{el.email}</p>
                </div>
              </td>
              <td>
                <div className="subscribers-basic-row-item">
                  <p>{el.crmRef}</p>
                </div>
              </td>
              <td>
                <div className="subscribers-basic-row-item">
                  <p>{el.lastSeen}</p>
                </div>
              </td>
            </tr>
          ))
        }
      </table>
    </div>
  )
};

export default SubscribersListTable;