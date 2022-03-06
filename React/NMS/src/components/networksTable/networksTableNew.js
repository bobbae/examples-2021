import React from 'react';
import './style.css';
import DownloadArrow from '../../assets/images/arrow-dl.png';
import UploadArrow from '../../assets/images/arrow-up.png';
import BlueCheckIcon from '../../assets/images/check-blue.png';
import RouterIconGray from '../../assets/images/router-icon.png';
import HealthBar from './calcHealth';

const data = JSON.parse('[{"isp_plan_name":"Graces Plan","last_seen":1600271754520,"enable_shaping":"NO_SBW","latitude":null,"publicIP":"10.38.223.2","crm_ref":"2222","mode":"sbw","enable_user_calibration":"org_default","smart_user_name":"Pavel Popov","rosVersion":"6.44","wan_rx":-1,"enable_calibration":"org_default","smart_user":"2a7b9103-3b1f-4342-9f71-1aa152f31976","longitude":null,"smart_user_email":"pavel@smart.network","wan_tx":-1,"network_name":"Lukas","health":-1,"labels":[],"device_ip":"10.38.223.2","smart_router_id":"346b4c44-b3b6-463a-89e8-50c87d492804","serial":"49CA044BF9C4","status":"not_responding","_id":"346b4c44-b3b6-463a-89e8-50c87d492804","ack":null,"network_stats":{"lastPingSuccess":-1,"bw_usage":-1,"last_calib":{"receive":96144450,"use_for_isp_speeds":false,"transmit":6748474,"used_dn":"usedRx","usedRx":32024,"used_up":"usedTx","unshapped":false,"dn":"receive","time":1600234863490,"usedTx":24912,"up":"transmit","type":"calib_a"}},"app_settings":{"isp_plan":"Graces Plan","isp_downstream":86,"isp_upstream":6,"isp_datacap":600},"isp_downstream":86,"isp_upstream":6},{"isp_plan_name":"MTS 100 Mb","last_seen":1600348683099,"enable_shaping":"no","latitude":37.808342,"publicIP":"94.243.57.18","crm_ref":"18918","mode":"sr","enable_user_calibration":"org_default","smart_user_name":"Pavel Popov","rosVersion":"6.46.1 (stable)","wan_rx":10512,"enable_calibration":"org_default","smart_user":"2a7b9103-3b1f-4342-9f71-1aa152f31976","longitude":-122.411564,"smart_user_email":"pavel@smart.network","wan_tx":10552,"network_name":"Pavel’s Home Net","health":50,"labels":["Tag 2","Test tag"],"device_ip":"94.243.57.18","smart_router_id":"40464883-6ac2-416e-b31f-8d92b61c3c33","serial":"7022068A3BBA","status":"enabled","_id":"40464883-6ac2-416e-b31f-8d92b61c3c33","ack":null,"network_stats":{"lastPingSuccess":50,"bw_usage":0.02,"last_calib":{"receive":53836144,"use_for_isp_speeds":false,"transmit":77440480,"used_dn":"usedRx","usedRx":5296,"used_up":"usedTx","unshapped":false,"dn":"receive","time":1600346840968,"usedTx":3392,"up":"transmit","type":"calib_a"}},"app_settings":{"isp_plan":"MTS 100 Mb","isp_downstream":100,"isp_upstream":100,"isp_datacap":0},"isp_downstream":100,"isp_upstream":100},{"isp_plan_name":"Test Plan","last_seen":1600348682330,"enable_shaping":"org_default","latitude":38.243118,"publicIP":"12.167.108.243","mode":"sr","enable_user_calibration":"org_default","smart_user_name":"Pavel Popov","rosVersion":"6.45.1 (stable)","wan_rx":2560,"enable_calibration":"org_default","smart_user":"2a7b9103-3b1f-4342-9f71-1aa152f31976","longitude":-122.039799,"smart_user_email":"pavel@smart.network","wan_tx":2336,"network_name":"Pavel’s Test (multiLAN)","health":92,"labels":[],"device_ip":"12.167.108.243","smart_router_id":"59c90553-7f01-461d-a138-276cd3974c60","serial":"5AC205FDB926","status":"enabled","_id":"59c90553-7f01-461d-a138-276cd3974c60","ack":null,"network_stats":{"lastPingSuccess":92,"bw_usage":0,"last_calib":{"receive":57771301,"use_for_isp_speeds":false,"transmit":69907322,"used_dn":"usedRx","usedRx":336,"used_up":"usedTx","unshapped":false,"dn":"receive","time":1600314430460,"usedTx":336,"up":"transmit","type":"calib_a"}},"app_settings":{"isp_plan":"Test Plan","isp_downstream":100,"isp_upstream":100,"isp_datacap":100},"isp_downstream":100,"isp_upstream":100},{"isp_plan_name":"MTS 100 Mb","last_seen":1600348682331,"enable_shaping":"org_default","latitude":38.394323,"publicIP":"12.167.108.244","crm_ref":"OnVPNss","mode":"sr","enable_user_calibration":"org_default","smart_user_name":"Grace Garalde-Correa","rosVersion":"6.45.2 (stable)","wan_rx":248,"enable_calibration":"org_default","smart_user":"7410f5bd-9fa1-4859-ba5d-cc52606155be","longitude":38.394346,"smart_user_email":"grace@smart.network","wan_tx":208,"network_name":"Test routers","health":96,"labels":[],"device_ip":"12.167.108.244","smart_router_id":"d796c561-41bb-45b3-bda2-8f66c97be4bf","serial":"6659050916C2","status":"enabled","_id":"d796c561-41bb-45b3-bda2-8f66c97be4bf","ack":null,"network_stats":{"lastPingSuccess":96,"bw_usage":0,"last_calib":{"receive":81041856,"use_for_isp_speeds":false,"transmit":92653130,"used_dn":"usedRx","usedRx":0,"used_up":"usedTx","unshapped":false,"dn":"receive","time":1600314449553,"usedTx":0,"up":"transmit","type":"calib_a"}},"app_settings":{"isp_plan":"MTS 100 Mb","isp_downstream":100,"isp_upstream":100,"isp_datacap":0},"isp_downstream":100,"isp_upstream":100},{"isp_plan_name":"Graces Plan","last_seen":1600348682343,"enable_shaping":"org_default","latitude":38.394346,"publicIP":"73.92.223.55","crm_ref":"CRMTESTCHANGE","mode":"sr","enable_user_calibration":"org_default","smart_user_name":"Grace Garalde-Correa","rosVersion":"6.46.2 (stable)","wan_rx":0,"enable_calibration":"org_default","smart_user":"7410f5bd-9fa1-4859-ba5d-cc52606155be","longitude":-121.930007,"smart_user_email":"grace@smart.network","wan_tx":0,"network_name":"Graces Network","health":100,"labels":["test","california","grace","Vacaville","North Point","Test tag"],"device_ip":"73.92.223.55","smart_router_id":"3fa119cb-0555-4094-a0a6-33a8ac11b772","serial":"665905AA91D7","status":"ack","_id":"3fa119cb-0555-4094-a0a6-33a8ac11b772","ack":null,"network_stats":{"lastPingSuccess":100,"bw_usage":0,"last_calib":{"receive":89462274,"use_for_isp_speeds":false,"transmit":6029704,"used_dn":"usedRx","usedRx":1080,"used_up":"usedTx","unshapped":false,"dn":"receive","time":1600321631887,"usedTx":552,"up":"transmit","type":"calib_a"}},"app_settings":{"isp_plan":"Graces Plan","isp_downstream":86,"isp_upstream":6,"isp_datacap":600},"isp_downstream":86,"isp_upstream":6}]');
console.log(data);

const NetworksTableNew = () => {
  return (
    <div className="table-div">
      <table className="ui-multi-row ui-heading-sort-align">
        <tr className="ui-network-labels">
          <th>
            <div>
              <p>Name</p>
            </div>
          </th>
          <th>
            <div>
              <p>Last 90 min.</p>
            </div>
          </th>
          <th>
            <div>
              <p>Traffic (absolute)</p>
            </div>
          </th>
          <th>
            <div>
              <p>Health</p>
            </div>
          </th>
          <th>
            <div>
              <p>Device IP</p>
            </div>
          </th>
          <th>
            <div >
              <p>Status</p>
            </div>
          </th>
        </tr>
        <tr className="ui-table-label">
          <td>
            <div>
              <p>Networks</p>
            </div>
          </td>
        </tr>

        {data.map((element) => (
          <tr className="ui-content-row">
            <td>
              <div className="ui-network-status-container">
                <div className="ui-network-checkbox ui-icon-checked">
                  <img src={BlueCheckIcon} alt="Check icon" className="ui-check-icon" />
                </div>
                <div className="ui-network-image-container">
                  <img className="ui-network-image" src={RouterIconGray} alt="Network logo" />
                </div>
                <div>
                  <div className="ui-network-label-container">
                    <p>{element.network_name}</p>
                  </div>
                  <div className="ui-network-status">
                    <div className="ui-active-field">
                      <p>ACTIVE</p>
                    </div>
                    <div className="ui-vip-clients-field">
                      <p>VIP CLIENTS</p>
                    </div>
                    <div className="ui-more-field">
                      <p>
                        5 MORE...
                      </p>
                    </div>
                  </div>
                </div>
              </div>
            </td>
            <td>
              <div className="ui-flex-center">
                <h2>Graph</h2>
              </div>
            </td>
            <td>
              <div className="ui-traffic-speed">
                <div className="ui-speed-inner">
                  <div className="ui-flex-center">
                    <div>
                      <img className="ui-speed-arrow" src={DownloadArrow} alt="Download arrow" />
                    </div>
                    <div>
                      <p className=" ui-speed-label-green">21.4 Mb/s</p>
                    </div>
                  </div>
                  <div className="ui-progress-gradient ui-gradient-green ui-progress-bar-small">
                    <div className="ui-progress-number-container">
                      <div>
                        <p>21</p>
                      </div>
                      <div>
                        <p>5</p>
                      </div>
                      <div>
                        <p>20</p>
                      </div>
                    </div>
                    <span className="ui-progress-background"></span>
                  </div>
                </div>
                <div className="ui-speed-inner ">
                  <div className="ui-flex-center">
                    <div>
                      <img className="ui-speed-arrow" src={UploadArrow} alt="Upload arrow" />
                    </div>
                    <div>
                      <p className="ui-speed-label-blue ">4.8 Mb/s</p>
                    </div>
                  </div>
                  <div className="ui-progress-gradient ui-gradient-blue ui-progress-bar-small ">
                    <div className="ui-progress-number-container">
                      <div>
                        <p>1</p>
                      </div>
                      <div>
                        <p>5</p>
                      </div>
                      <div>
                        <p>20</p>
                      </div>
                    </div>
                    <div className="ui-progress-background"></div>
                  </div>
                </div>
              </div>
            </td>
            <td>
              {<HealthBar primiStatus={element.status} primiHealth={element.health} />}
            </td>
            <td>
              <div className="ui-basic-row-item ui-device-ip-text">
                {element.publicIP}
              </div>
            </td>
            <td>
              <div className="ui-basic-row-item ui-status-text">
                {element.status}
              </div>
            </td>
          </tr>
        ))}
      </table>
    </div>
  )
};

export default NetworksTableNew;