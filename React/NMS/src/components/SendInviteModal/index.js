import React from 'react';
import './style.css';
import Modal from 'react-modal';

const SendInviteModal = () => {
  return (
    <Modal className={'react-modal-send-invite'} isOpen={false}>
      <div className="send-invite-main">
        <div className="send-invite-header">
          <div className="send-invite-label">
            <p>Say hello to Ofer</p>
          </div>
          <div className="send-invite-info">
            <p>Send a welcome message and ask user to download smart.network app</p>
          </div>
        </div>
        <div className="send-invite-preview">
          <div className="send-invite-preview-label">
            <p>PREVIEW</p>
          </div>
          <div className="send-invite-input">
            <textarea type="text" placeholder="Enter invite message" />
          </div>
        </div>
        <div className="send-invite-buttons">
          <div className="send-invite-cancel">
            <p>Cancel</p>
          </div>
          <div className="send-invite-send">
            <p>Send Invite</p>
          </div>
        </div>
      </div>
    </Modal>
  )
}

export default SendInviteModal;