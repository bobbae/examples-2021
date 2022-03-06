import React from 'react';
import SpeedtestHeader from '../../components/speedtest/SpeedtestHeader';
import SpeedtestList from '../../components/speedtest/SpeedtestList';
import './style.css';

function Speedtest() {
  return (
    <div className="ui-top-content">     
      <SpeedtestHeader />
      <div className="ui-speedtest-wrapper">
        <SpeedtestList />
      </div>
    </div>
  )
};

export default Speedtest;