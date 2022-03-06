import React from 'react';
import './style.css';
import TechModeDropdownRow from './TechModeDropdownRow';

const EditSubscriber = () => {
  return (
    <div className="modal-main">
      <div className="modal-header">
        <div className="modal-cancel-label">
          <p>Cancel</p>
        </div>
        <div className="modal-main-label">
          <p>Edit Subscriber</p>
        </div>
        <div className="modal-save-label">
          <p>Save</p>
        </div>
      </div>
      <div className="modal-body">
        <div className="modal-row-multi">
          <div className="modal-row modal-long-value">
            <div className="modal-row-label">
              <p>First Name</p>
            </div>
            <div className="modal-row-value">
              <p>Paul</p>
            </div>
          </div>
          <div className="modal-row modal-long-value">
            <div className="modal-row-label">
              <p>Last Name</p>
            </div>
            <div className="modal-row-value">
              <p>Fields</p>
            </div>
          </div>
          <div className="modal-row modal-long-value">
            <div className="modal-row-label">
              <p>E-mail</p>
            </div>
            <div className="modal-row-value-edit-sub">
              <input placeholder="ja sam placeholder"/>
              <div className="ui-input-error">
                <p>Input error message</p>
              </div>
            </div>
          </div>
        </div>
        <div className="modal-row">
          <div className="modal-row-label">
            <p>Password</p>
          </div>
          <div className="modal-row-value">
            <p>Reset Password</p>
          </div>
        </div>
        <div className="modal-row-multi">
          <div className="modal-row modal-long-value">
            <div className="modal-row-label">
              <p>Company</p>
            </div>
            <div className="modal-row-value">
              <p>Enter...</p>
            </div>
          </div>
          <div className="modal-row modal-long-value">
            <div className="modal-row-label">
              <p>CRM Ref</p>
            </div>
            <div className="modal-row-value">
              <p>6433f3</p>
            </div>
          </div>
          <div className="modal-row modal-long-value">
            <div className="modal-row-label">
              <p>Tech Code</p>
            </div>
            <div className="modal-row-value">
              <p>3030833940911558</p>
            </div>
          </div>
        </div>
        <TechModeDropdownRow />
      </div>
    </div>
  )
};

export default EditSubscriber;