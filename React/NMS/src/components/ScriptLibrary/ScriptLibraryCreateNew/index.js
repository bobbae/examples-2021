import React from 'react';
import './style.css';

const ScriptLibraryCreateNew = () => (
  <div className="isp-modal-main">
    <div className="isp-modal-header">
      <div className="isp-modal-cancel-label">
        <p>Cancel</p>
      </div>
      <div className="isp-modal-main-label">
        <p>Create ISP Plan</p>
      </div>
      <div className="isp-modal-save-label">
        <p>Save</p>
      </div>
    </div>
    <div className="isp-modal-body">
      <div className="isp-modal-row isp-modal-long-value">
        <div className="isp-modal-row-label">
          <p>ISP Plan Name</p>
        </div>
        <div className="isp-modal-row-value">
          <p>$144 Valleywave</p>
        </div>
      </div>
      <div className="isp-modal-row-multi">
        <div className="isp-modal-row">
          <div className="isp-modal-row-label">
            <p>ISP Data Cap</p>
          </div>
          <div className="isp-modal-row-value">
            <p>23 GB</p>
          </div>
        </div>
        <div className="isp-modal-row">
          <div className="isp-modal-row-label">
            <p>Upstream</p>
          </div>
          <div className="isp-modal-row-value">
            <p>14 MB</p>
          </div>
        </div>
        <div className="isp-modal-row">
          <div className="isp-modal-row-label">
            <p>Downstream</p>
          </div>
          <div className="isp-modal-row-value">
            <p>14 MB</p>
          </div>
        </div>
      </div>



      <div className="isp-modal-row-multi">
        <div className="isp-modal-row isp-modal-long-value">
          <div className="isp-modal-row-label">
            <p>ISP Ref</p>
          </div>
          <div className="isp-modal-row-value">
            <p>Text...</p>
          </div>
        </div>
        <div className="isp-modal-row isp-modal-long-value">
          <div className="isp-modal-row-label">
            <p>Description</p>
          </div>
          <div className="isp-modal-row-value">
            <p>Text...</p>
          </div>
        </div>
      </div>
    </div>
  </div>
)

export default ScriptLibraryCreateNew;