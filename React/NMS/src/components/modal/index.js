import React from 'react';
import './style.css';
import ModalDropdownRow from '../modalDropdownRow';

const Modal = ({ }) => (
  <div className="modal-main">
    <div className="ui-modal-header">
      <div className="ui-modal-cancel-label">
        <p>
          Cancel, test
        </p>
      </div>
      <div className="ui-modal-main-label">
        <p>
          Edit Network
        </p>
      </div>
      <div className="ui-modal-save-label">
        <p>
          Save
        </p>
      </div>
    </div>
    <div className="ui-modal-body">
      <div className="ui-modal-row-multi">
        <div className="ui-modal-row">
          <div className="ui-modal-row-label">
            <p>
              Network name
            </p>
          </div>
          <div className="ui-modal-row-value">
            <p>
              The Correa's
            </p>
          </div>
        </div>
        <div className="ui-modal-row">
          <div className="ui-modal-row-label">
            <p>
              CRM Ref
            </p>
          </div>
          <div className="ui-modal-row-value">
            <p>
              CRMTEST
            </p>
          </div>
        </div>
      </div>
      <div className="ui-modal-row">
        <div className="ui-modal-row-label">
          <p>
            ISP Plan
          </p>
        </div>
        <div className="ui-modal-row-value">
          <p>
            Graces Plan
          </p>
        </div>
      </div>
      <div className="ui-modal-row-with-heading">
        <div className="ui-modal-row-heading">
          <p>
            CALIBRATION
          </p>
        </div>
        <ModalDropdownRow />
      </div>
      <div className="ui-modal-row-with-heading">
        <div className="ui-modal-row-heading">
          <p>
            USER SPEED TEST
          </p>
        </div>
        <ModalDropdownRow />
      </div>
      <div className="ui-modal-row-with-heading">
        <div className="ui-modal-row-heading">
          <p>
            SHAPING
          </p>
        </div>
        <ModalDropdownRow />
      </div>
      <div className="ui-modal-row-with-heading">
        <div className="ui-modal-row-heading">
          <p>
            GEO Location
          </p>
        </div>
        <ModalDropdownRow />
      </div>
      <div className="ui-modal-row-with-heading">
        <div className="ui-modal-row-heading">
          <p>
            CALIBRATION
          </p>
        </div>
        <div className="ui-modal-row-multi">
          <div className="ui-modal-row">
            <div className="ui-modal-row-label">
              <p>
                ISP Plan
              </p>
            </div>
            <div className="ui-modal-row-value">
              <p>
                Graces Plan
              </p>
            </div>
          </div>
          <div className="ui-modal-row">
            <div className="ui-modal-row-label">
              <p>
                ISP Plan
              </p>
            </div>
            <div className="ui-modal-row-value">
              <p>
                Graces Plan
              </p>
            </div>
          </div>
          <div className="ui-modal-row">
            <div className="ui-modal-row-label">
              <p>
                ISP Plan
              </p>
            </div>
            <div className="ui-modal-row-value">
              <p>
                Graces Plan
              </p>
            </div>
          </div>
        </div>
      </div>
    </div>
    {/* <button style={{display: 'none'}} onClick={closeModal}>Close</button> */}
  </div>
)

export default Modal;