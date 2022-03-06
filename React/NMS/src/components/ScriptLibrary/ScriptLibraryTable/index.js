import React from 'react';
import './style.css';

const data = [
  {
    scriptName: `Disable Netflow`,
    owner: `superuser@smart.network`,
    created: `12/05/2019 @ 04:10 am`
  },
  {
    scriptName: `Enable Netflow`,
    owner: `superuser@smart.network`,
    created: `12/05/2019 @ 04:10 am`
  },
  {
    scriptName: `Fix on-up Netflow`,
    owner: `superuser@smart.network`,
    created: `12/05/2019 @ 04:10 am`
  },
  {
    scriptName: `Fix on-up Netflow`,
    owner: `superuser@smart.network`,
    created: `12/05/2019 @ 04:10 am`
  },
  {
    scriptName: `Fix on-up Netflow`,
    owner: `superuser@smart.network`,
    created: `12/05/2019 @ 04:10 am`
  },
  {
    scriptName: `Fix on-up Netflow`,
    owner: `superuser@smart.network`,
    created: `12/05/2019 @ 04:10 am`
  },
  {
    scriptName: `Fix on-up Netflow`,
    owner: `superuser@smart.network`,
    created: `12/05/2019 @ 04:10 am`
  },
  {
    scriptName: `Fix on-up Netflow`,
    owner: `superuser@smart.network`,
    created: `12/05/2019 @ 04:10 am`
  },
  {
    scriptName: `Fix on-up Netflow`,
    owner: `superuser@smart.network`,
    created: `12/05/2019 @ 04:10 am`
  },
  {
    scriptName: `Fix on-up Netflow`,
    owner: `superuser@smart.network`,
    created: `12/05/2019 @ 04:10 am`
  },
  {
    scriptName: `Fix on-up Netflow`,
    owner: `superuser@smart.network`,
    created: `12/05/2019 @ 04:10 am`
  },
  {
    scriptName: `Fix on-up Netflow`,
    owner: `superuser@smart.network`,
    created: `12/05/2019 @ 04:10 am`
  },
  {
    scriptName: `Fix on-up Netflow`,
    owner: `superuser@smart.network`,
    created: `12/05/2019 @ 04:10 am`
  },
]

const ScriptLibraryTable = () => {
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
              <p>Owner</p>
            </div>
          </th>
          <th>
            <div >
              <p>Created</p>
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
                      <p>{el.scriptName}</p>
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
                <div className="ui-basic-row-item ui-device-ip-text">
                  <p>{el.owner}</p>
                </div>
              </td>
              <td>
                <div className="ui-basic-row-item ui-status-text">
                  <p>{el.created}</p>
                </div>
              </td>
            </tr>
          ))
        }
      </table>
    </div>
  )
};

export default ScriptLibraryTable;