import React from 'react';
import { AreaChart, Area } from 'recharts';

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
    rx: 3,
    time: "18:09:29",
    timestamp: 1601895984000,
    timestamp_rx: 1601748300000,
    timestamp_tx: 1601748300000,
    top_user: null,
    tx: 5,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 1,
    time: "18:19:29",
    timestamp: 1601899584000,
    timestamp_rx: 1601749500000,
    timestamp_tx: 1601749500000,
    top_user: null,
    tx: 7,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 5,
    time: "18:19:29",
    timestamp: 1601903184000,
    timestamp_rx: 1601749500000,
    timestamp_tx: 1601749500000,
    top_user: null,
    tx: 3,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 2,
    time: "18:19:29",
    timestamp: 1601906784000,
    timestamp_rx: 1601749500000,
    timestamp_tx: 1601749500000,
    top_user: null,
    tx: 4,
    up: "rx",
  },
  {
    dn: "tx",
    rx: 6,
    time: "18:19:29",
    timestamp: 1601910384000,
    timestamp_rx: 1601749500000,
    timestamp_tx: 1601749500000,
    top_user: null,
    tx: 3,
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

const BandwidthGraphSnapshot = () => {
  return (
    <AreaChart
      margin={{
        top: 0, right: 0, left: 0, bottom: 0,
      }}
      data={newData} width={100} height={30}
    >
<defs>
          
          <linearGradient id="colorBlue" x1="0" y1="0" x2="0" y2="1">
            <stop offset="100%" stopColor="#5F72FF" stopOpacity={0.5} />
          </linearGradient>
          <linearGradient id="colorGreen" x1="0" y1="0" x2="0" y2="1">
            <stop offset="100%" stopColor="#6EE294" stopOpacity={0.9} />
          </linearGradient>
          
        </defs>
          <Area dataKey="tx" stroke="#5F72FF" fillOpacity={0.4} fill="url(#colorBlue)" strokeWidth="1" dot={false} activeDot={false} />
          <Area dataKey="rx" stroke="#6EE294" fillOpacity={0.6} fill="url(#colorGreen)" strokeWidth="1" dot={false} activeDot={false} />

    </AreaChart>

  );
}

export default BandwidthGraphSnapshot;