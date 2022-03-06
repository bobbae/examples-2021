import React, { useState } from 'react';
import RouterTrafficHeader from '../../components/routerTraffic/routerTrafficHeader';
import './style.css';
import ServiceDisplay from '../../components/routerTraffic/ServiceDisplay';
import DeviceDisplay from '../../components/routerTraffic/DeviceDisplay';
import TrafficDisplay from '../../components/routerTraffic/TrafficDisplay';

const trafficData = [
  { userProfile: `alice`, userName: `Alice`, userGraphData: `User graph data`, deviceList: [
    { deviceIcon: `laptop`, deviceName: `Alice's Dell XPS`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
    { deviceIcon: `mobile`, deviceName: `Alice's iPhone Xr`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
    { deviceIcon: `mobile`, deviceName: `Alice's Galaxy S12`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
    { deviceIcon: `mobile`, deviceName: `Alice's iPhone 11+`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
  ]},

  { userProfile: `jack`, userName: `Jack`, userGraphData: `User graph data`, deviceList: [
    { deviceIcon: `laptop`, deviceName: `Jack's Dell XPS`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
    { deviceIcon: `mobile`, deviceName: `Jack's iPhone Xr`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
    { deviceIcon: `mobile`, deviceName: `Jack's Galaxy S12`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
    { deviceIcon: `mobile`, deviceName: `Jack's iPhone 11+`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
  ]},

  { userProfile: `lucy`, userName: `Lucy`, userGraphData: `User graph data`, deviceList: [
    { deviceIcon: `laptop`, deviceName: `Lucy's Dell XPS`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
    { deviceIcon: `mobile`, deviceName: `Lucy's iPhone Xr`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
    { deviceIcon: `mobile`, deviceName: `Lucy's Galaxy S12`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
    { deviceIcon: `mobile`, deviceName: `Lucy's iPhone 11+`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
  ]},

  { userProfile: `john`, userName: `John`, userGraphData: `User graph data`, deviceList: [
    { deviceIcon: `laptop`, deviceName: `John's Dell XPS`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
    { deviceIcon: `mobile`, deviceName: `John's iPhone Xr`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
    { deviceIcon: `mobile`, deviceName: `John's Galaxy S12`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
    { deviceIcon: `mobile`, deviceName: `John's iPhone 11+`, deviceGraphData: `Device graph data`, serviceList: [
      { serviceLogo: `netflix`, serviceName: `Netflix`, serviceGraphData: `Netflix graph data` },
      { serviceLogo: `dropbox`, serviceName: `Dropbox`, serviceGraphData: `Dropbox graph data` },
      { serviceLogo: `amazon`, serviceName: `Amazon Prime`, serviceGraphData: `Amazon graph data` },
    ] },
  ]},
];

const wanTrafficData = [
  { logo: 'netflix', service: 'Netflix', graphData: 'Netflix graph data' },
  { logo: 'dropbox', service: 'Dropbox', graphData: 'Dropbox graph data' },
  { logo: 'netflix', service: 'Google Services', graphData: 'Google services graph data' },
  { logo: 'netflix', service: 'Apple', graphData: 'Apple graph data' }
];

const lanDeviceData = [
  {
    deviceIcon: 'laptop', deviceName: 'Alice\'s Dell XPS', graphData: 'Alices DELL graph data', deviceDropdown: [
      { logo: 'netflix', service: 'Netflix', graphData: 'Netflix graph data' },
      { logo: 'dropbox', service: 'Dropbox', graphData: 'Dropbox graph data' },
      { logo: 'netflix', service: 'Amazon Services', graphData: 'Google services graph data' },
    ]
  },
  {
    deviceIcon: 'mobile', deviceName: 'iPhone Xr', graphData: 'iPhone Xr graph data', deviceDropdown: [
      { logo: 'netflix', service: 'Netflix', graphData: 'Netflix graph data' },
      { logo: 'dropbox', service: 'Dropbox', graphData: 'Dropbox graph data' },
      { logo: 'netflix', service: 'Google Services', graphData: 'Google services graph data' },
    ]
  },
  {
    deviceIcon: 'mobile', deviceName: 'Galaxy S12', graphData: 'Galaxy S12 graph data', deviceDropdown: [
      { logo: 'netflix', service: 'Netflix', graphData: 'Netflix graph data' },
      { logo: 'dropbox', service: 'Dropbox', graphData: 'Dropbox graph data' },
      { logo: 'netflix', service: 'Google Services', graphData: 'Google services graph data' },
    ]
  },
  {
    deviceIcon: 'mobile', deviceName: 'iPhone 11', graphData: 'iPhone 11 graph data', deviceDropdown: [
      { logo: 'netflix', service: 'Netflix', graphData: 'Netflix graph data' },
      { logo: 'dropbox', service: 'Dropbox', graphData: 'Dropbox graph data' },
      { logo: 'netflix', service: 'Google Services', graphData: 'Google services graph data' },
    ]
  },
]

const RouterTraffic = () => {
  const [lanView, setLanView] = useState(false);
  const [serviceView, setServiceView] = useState(-1);
  const [serviceData, setServiceData] = useState({});

  const [deviceServicesDropdown, setDeviceServicesDropdown] = useState(-1);

  const closeLanView = () => {
    setLanView(false);
  }
  const requestLanView = () => {
    setLanView(true);
  }
  return (
    <div className="ui-top-content">
      <RouterTrafficHeader requestLanView={requestLanView} closeLanView={closeLanView} lanView={lanView} />
      <div className={`ui-router-traffic-wrapper ${lanView ? 'lan-mobile-view' : ''}`}>
        <TrafficDisplay lanDeviceData={lanDeviceData} trafficData={trafficData} lanView={lanView} setServiceData={setServiceData} wanTrafficData={wanTrafficData} serviceData={serviceData} serviceView={serviceView} setServiceView={setServiceView}/>
        <DeviceDisplay lanView={lanView} deviceServicesDropdown={deviceServicesDropdown} setDeviceServicesDropdown={setDeviceServicesDropdown} lanDeviceData={lanDeviceData}/>
        <ServiceDisplay serviceData={serviceData} serviceView={serviceView} lanView={lanView} setLanView={setLanView} />
      </div>
    </div>
  )
};

export default RouterTraffic;