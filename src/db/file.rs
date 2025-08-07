use std::{
    fmt::Debug,
    fs::{File, OpenOptions},
    io::{self, Read},
    path::Path,
};

use snafu::{Snafu, whatever};

const FILE_PREFIX: &[u8; 13] = b"mag1cian's db";

#[derive(Snafu, Debug)]
#[snafu(whatever, display("open db file failed: {message} at {loc}"))]
pub struct DBFileError {
    message: String,
    #[snafu(implicit)]
    loc: snafu::Location,
    #[snafu(source(from(io::Error, Some)))]
    source: Option<io::Error>,
}

/// first 16 Byte is mag1cian's db
pub struct DBFile {
    pub file: File,
}

impl DBFile {
    pub fn open(path: impl AsRef<Path> + Debug) -> Result<Self, DBFileError> {
        // check valid
        let mut f = whatever! {
        OpenOptions::new().read(true).write(true).create(true).open(path.as_ref()),
        "can not open {:?}",path
        };
        let mut buf = [0u8; 13];
        whatever!(f.read_exact(&mut buf), "read db file failed");
        if &buf[..] != FILE_PREFIX {
            whatever!("{path:?} is not valid")
        } else {
            Ok(Self { file: f })
        }
    }
}

#[cfg(test)]
mod tests {
    use std::io::Write;
    use std::{fs::File, io::Read};

    use tempfile::{tempdir, tempfile_in};

    use crate::db::file::{DBFile, FILE_PREFIX};

    #[test]
    fn test_db_file() {
        {
            let tmp_dir = tempdir().unwrap();
            let file_path = tmp_dir.path().join("dbfile");
            let dbfile = DBFile::open(file_path);
            assert!(dbfile.is_err())
        }
        {
            let tmp_dir = tempdir().unwrap();
            let file_path = tmp_dir.path().join("dbfile");
            let mut f = File::create(file_path.as_path()).unwrap();
            let sz = f.write(FILE_PREFIX).unwrap();
            assert_eq!(sz, FILE_PREFIX.len());

            let dbfile = DBFile::open(file_path);
            assert!(dbfile.is_ok());
        }
        {
            let tmp_dir = tempdir().unwrap();
            let file_path = tmp_dir.path().join("dbfile");
            let mut f = File::create(file_path.as_path()).unwrap();
            let sz = f.write(FILE_PREFIX).unwrap();
            assert_eq!(sz, FILE_PREFIX.len());
            let s = b"hello_db";
            f.write(s);

            let mut dbfile = DBFile::open(file_path).unwrap();
            let mut buf = Vec::<u8>::new();
            let sz = dbfile.file.read_to_end(&mut buf).unwrap();
            assert_eq!(s.len(), sz);
            assert_eq!(&buf[..], s);
        }
    }
}
