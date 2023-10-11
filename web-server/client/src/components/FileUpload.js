import React, { Fragment, useState, useEffect } from "react";

import axios from "axios";

const FileUpload = ({files, setFiles, setMessage}) => {
  // Hooks
  const [currentFileLocal, setCurrentFileLocal] = useState({});
  const [currentFileName, setCurrentFileName] = useState("Choose File");


  const [singerName, setSingerName] = useState("Enter singer name here...");
  const [albumName, setAlbumName] = useState("Enter album name here...");

  const onSingerNameChange = (e) => {
    setSingerName(e.target.value);
  };

  const onAlbumNameChange = (e) => {
    setAlbumName(e.target.value);
  };

  const onFileChange = (e) => {
    const file = e.target.files[0];
    if(!file) return;
    file.isUploading = true;
    setCurrentFileLocal(file);
    setCurrentFileName(file.name);
  };

  const onFileSubmit = async (e) => {
    e.preventDefault();
    if (
      albumName === "Enter album name here..." ||
      singerName === "Enter singer name here..." ||
      albumName === "" ||
      singerName === ""
    ) {
      setMessage("No file uploaded - All fields should be provided");
    }
      
    currentFileLocal.isUploading = false;
    const formData = new FormData();
    formData.append('file', currentFileLocal, currentFileLocal.name);
    // TODO: maybe add other information here to the formData - i.e. singer, album, etc.
    formData.append('singer', singerName);
    formData.append('album', albumName);
    formData.append('song', currentFileLocal.name.slice(0, -4));

    

    try {
      await axios.post("/upload", formData, {
        headers: {
          "Content-Type": "multipart/form-data",
        },
      });
      currentFileLocal.isUploading = false;
      setFiles([...files, currentFileLocal])
      setMessage("File Uploaded!");
    } catch (err) {
      if (err.hasOwnProperty("response")) {
        if (err.response.status === 500) {
          setMessage("There was a problem with the server!");
        } else {
          setMessage(err.response.data.msg);
        }
      }
      else {
        console.log(err);
      }
    }
  };

  return (
    <Fragment>
      {
        <form onSubmit={onFileSubmit}>
          {
            <div>
              <label htmlFor="singerName">Singer:</label>
              <input
                type="text"
                className="custom-name-input ml-1 display:table-cell"
                id="singerName"
                placeholder={singerName}
                onChange={onSingerNameChange}
              />
              <br />
              <label htmlFor="singerName">Album:</label>
              <input
                type="text"
                className="custom-name-input ml-1"
                id="singerName"
                placeholder={albumName}
                onChange={onAlbumNameChange}
              />
              <br />
            </div>
          }
          <div className="custom-file mb4">
            <input
              type="file"
              className="custom-file-input"
              id="customFile"
              onChange={onFileChange}
            />
            <label className="custom-file-label" htmlFor="customFile">
              {currentFileName}
            </label>
          </div>

          <input
            type="submit"
            value="Upload"
            className="btn btn-primary btn-block mt-4"
          />
        </form>
      }
      
    </Fragment>
  );
};

export default FileUpload;