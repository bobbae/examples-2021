import React from 'react';
import GoogleMapReact from 'google-map-react';
import './style.css';
import InfoWindowContent from './infoWindowContent';

const InfoWindow = props => (
  <InfoWindowContent />
);

const AnyReactComponent = ({ text }) => (
  <div className="info-window-container">
    <img src={require('../../assets/images/bridge-green-marker.png')} style={{ width: '30px', height: '34px' }} alt="Map marker" />
    <InfoWindow />
  </div>
);

export class MapContainer extends React.Component {
  render() {
    return (
      <div style={{ height: '650px', width: '100%' }}>
        <GoogleMapReact
          bootstrapURLKeys={{ key: "AIzaSyClhpytnSsGpLP5r6MFq05qM3iHHkuSSCI" }}
          defaultCenter={{
            lat: 38.1715307692773,
            lng: -121.93831743140169
          }}
          zoom={9}
          yesIWantToUseGoogleMapApiInternals>
            <AnyReactComponent
              lat="38.1715307692773"
              lng="-121.93831743140169"
              text="JOOOJ DEBILU"/>
        </GoogleMapReact>
      </div>
    );
  }
}

export default MapContainer;