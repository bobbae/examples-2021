import React from 'react';
import './style.css';
import { LineChart, Line, Tooltip, Brush, XAxis, YAxis, CartesianGrid, ResponsiveContainer } from 'recharts';
import CustomTooltip from '../customTooltip';
//import Tooltip from '../../recharts/src/component/Tooltip';

const newData = [
  {
    dn: "tx",
    rx: 1,
    time: "17:49:29",
    timestamp: 1601888784000,
    timestamp_rx: 1601747100000,
    timestamp_tx: 1601747100000,
    top_user: null,
    tx: 2,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 2,
    time: "17:59:29",
    timestamp: 1601892384000,
    timestamp_rx: 1601747700000,
    timestamp_tx: 1601747700000,
    top_user: null,
    tx: 1,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 6,
    time: "18:09:29",
    timestamp: 1601895984000,
    timestamp_rx: 1601748300000,
    timestamp_tx: 1601748300000,
    top_user: null,
    tx: 4,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 7,
    time: "18:19:29",
    timestamp: 1601899584000,
    timestamp_rx: 1601749500000,
    timestamp_tx: 1601749500000,
    top_user: null,
    tx: 6,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 7,
    time: "18:19:29",
    timestamp: 1601903184000,
    timestamp_rx: 1601749500000,
    timestamp_tx: 1601749500000,
    top_user: null,
    tx: 6,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 7,
    time: "18:19:29",
    timestamp: 1601906784000,
    timestamp_rx: 1601749500000,
    timestamp_tx: 1601749500000,
    top_user: null,
    tx: 6,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 7,
    time: "18:19:29",
    timestamp: 1601910384000,
    timestamp_rx: 1601749500000,
    timestamp_tx: 1601749500000,
    top_user: null,
    tx: 6,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 7,
    time: "18:19:29",
    timestamp: 1601913984000,
    timestamp_rx: 1601749500000,
    timestamp_tx: 1601749500000,
    top_user: null,
    tx: 6,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 7,
    time: "18:19:29",
    timestamp: 1601917584000,
    timestamp_rx: 1601749500000,
    timestamp_tx: 1601749500000,
    top_user: null,
    tx: 6,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 7,
    time: "18:19:29",
    timestamp: 1601921184000,
    timestamp_rx: 1601749500000,
    timestamp_tx: 1601749500000,
    top_user: null,
    tx: 6,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 7,
    time: "18:19:29",
    timestamp: 1601924784000,
    timestamp_rx: 1601749500000,
    timestamp_tx: 1601749500000,
    top_user: null,
    tx: 6,
    up: "rx",
  },
];

const hourFormatter = (tickTimestamp) => {
  let fullDate = new Date(tickTimestamp);
  let fullHours = fullDate.toLocaleString('en-US', { hour: 'numeric', minute: 'numeric', hour12: true })
  return fullHours;
}

const BandwidthGraph = ({ active, payload, label }) => {
  return (
    <div id="bandwidth-usage" className="ui-box" style={{ padding: '20px', overflow: 'visible' }}>
      <div className="ui-graph-labels">
        <div className="ui-graph-labels-inner">
          <div className="ui-graph-main-label">
            <p>Bandwidth Usage</p>
          </div>
          <div className="ui-graph-time-label">
            <p>1 hour</p>
          </div>
        </div>
        <div className="ui-graph-date-label">
          <p>24 Jan 2020</p>
        </div>
      </div>
      <ResponsiveContainer width='100%' height={180}>
        <LineChart className="hide-last-tick"
          margin={{
            top: 0, right: -45, left: 0, bottom: 0,
          }}
          data={newData}>
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis allowDataOverflow={true} interval={0} tickFormatter={hourFormatter} tick={{ dx: 20 }} tickLine={false} dataKey='timestamp' />
          <YAxis tick={{ dy: 0 }} tickLine={false} orientation="right" type="number" domain={[0, 8]} />
          <Tooltip position={{ y: -115 }} cursor={false} content={<CustomTooltip label={label} payload={payload} active={active} />} />
          <Brush dataKey="rx" height={30} stroke="#8884d8" />
          <Line dataKey="rx" stroke="#6EE294" strokeWidth="4" dot={false} activeDot={false} />
          <Line dataKey="tx" stroke="#5F72FF" strokeWidth="4" dot={false} activeDot={false} />
        </LineChart>
      </ResponsiveContainer>

    </div>
  );
}

export default BandwidthGraph;