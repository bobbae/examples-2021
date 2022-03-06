import React, { useState } from 'react';
import ScriptLibraryHeader from '../../components/ScriptLibrary/ScriptLibraryHeader';
import ScriptLibraryTable from '../../components/ScriptLibrary/ScriptLibraryTable';
import ReactModal from 'react-modal';
import ScriptLibraryCreateNew from '../../components/ScriptLibrary/ScriptLibraryCreateNew';
import './style.css';

const ScriptLibrary = () => {
  const [createNew, setCreateNew] = useState(false);
  return (
    <div className="ui-top-content">
      <ReactModal isOpen={createNew}>
        <ScriptLibraryCreateNew />
      </ReactModal>
      <ScriptLibraryHeader createNew={createNew} setCreateNew={setCreateNew} />
      <div className="script-library-wrapper">
      <ScriptLibraryTable />
      </div>
    </div>
  )
};

export default ScriptLibrary;