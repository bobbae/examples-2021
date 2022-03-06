import React, { useState } from 'react';
import MainHeader from '../../components/MainHeader';
import EditLogoBig from '../../components/Preferences/EditLogoBig';
import MaintenanceHours from '../../components/Preferences/MaintenanceHours';
import PreferencesDetails from '../../components/Preferences/PreferencesDetails';
import PreferencesSettings from '../../components/Preferences/PreferencesSettings';
import PreferencesOther from '../../components/Preferences/PreferencesOther';
import './style.css';

const Preferences = () => {
  return (
    <div className="ui-top-content">
      <MainHeader mainLabel='Preferences' />
      <EditLogoBig />
      <PreferencesDetails />
      <PreferencesSettings />
      <PreferencesOther />

      {/* Maintenance hours is new screen  */}
      <MaintenanceHours />
    </div>
  )
};

export default Preferences;