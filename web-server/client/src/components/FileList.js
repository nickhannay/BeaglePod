import React from 'react'
import FileItem from './FileItem'
import axios from "axios";

const FileList = ({ files, removeFile }) => {
    const deleteFileHandler = async (_name) => {
        try {
            await axios.post("/delete", { name: _name }, {
              headers: {
                "Content-Type": "application/json",
              },
            });
            removeFile(_name)
          } catch (err) {
            console.log('Error deleting the file')
          }
    }
    return (
        <ul className="file-list">
            {
                files &&
                files.map(f => (<FileItem
                    key={f.name}
                    file={f}
                    deleteFile={deleteFileHandler} />))
            }
        </ul>
    )
}

export default FileList