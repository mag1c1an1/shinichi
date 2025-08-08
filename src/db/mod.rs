mod file;
use bytes::Buf;
use file::DBFile;
use file::DBFileError;
use snafu::ResultExt;
use snafu::Snafu;

mod slice;
use std::io::Read;
use std::io::Write;
use std::{fs::File, path::Path};

use slice::Slice;

use crate::db::file::KVPair;

pub struct DB {
    file: DBFile,
}

#[derive(Debug, Snafu)]
pub enum DBError {
    #[snafu(display("open db failed : {source}"))]
    File { source: DBFileError },
}

impl DB {
    pub fn open(db_path: impl AsRef<Path> + std::fmt::Debug) -> Result<Self, DBError> {
        let f = DBFile::open(db_path).context(FileSnafu)?;
        Ok(DB { file: f })
    }

    pub fn put(&mut self, key: &Slice, value: &Slice) -> Result<(), DBError> {
        self.file
            .write_pair(&KVPair::new(key, value))
            .context(FileSnafu)?;
        Ok(())
    }

    pub fn get(&self,key: &Slice) -> Result<Option<Slice>, DBError> {
        self.file.get(key)
    }
}
