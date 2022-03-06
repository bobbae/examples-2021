import React from 'react';
import './style.css';
import { LineChart, Brush, Tooltip, Line, YAxis, XAxis, CartesianGrid } from 'recharts';
import CustomTooltip from '../customTooltip';
import {useHorizontalScroll} from './burazengija';
//import Brush from '../../recharts/src/cartesian/Brush';
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

const HealthGraph = ({ active, payload, label }) => {
  const scrollRef = useHorizontalScroll();
  return (
    <div id="health" ref={scrollRef} className="ui-box example" style={{ padding: '20px', overflowX: 'auto' }}>
      <div className="ui-graph-labels">
        <div className="ui-graph-labels-inner">
          <div className="ui-graph-main-label">
            <p>Health</p>
          </div>
          <div className="ui-graph-time-label">
            <p>Day</p>
          </div>
        </div>
        <div className="ui-graph-date-label">
          <p>24 Jan 2020</p>
        </div>
      </div>
        <LineChart height={180} width={700}
          margin={{
            top: 0, right: -45, left: 0, bottom: 0,
          }}
          data={newData}
          ref={scrollRef}>
          <CartesianGrid position="right" strokeDasharray="3 3" />
          <XAxis tick={{ dx: 20 }} tickFormatter={hourFormatter} interval={0} tickLine={false} dataKey='timestamp' />
     
          <YAxis style={{position: 'fixed'}} tickLine={false} orientation="right" type="number" domain={[0, 8]} />

          <Tooltip position={{ y: -115 }} cursor={false} allowEscapeViewBox={{ x: false, y: true }} content={<CustomTooltip label={label} payload={payload} active={active} />} />
          <Brush dataKey="rx" height={30} stroke="#8884d8" />
          <Line dataKey="rx" stroke="#5F72FF" strokeWidth="4" dot={false} activeDot={false} />
        </LineChart>
    </div>
  );
}

export default HealthGraph;