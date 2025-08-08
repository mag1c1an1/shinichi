mod file;
use file::DBFile;
use file::DBFileError;
use snafu::ResultExt;
use snafu::Snafu;

mod slice;
use std::{fs::File, path::Path};

use slice::Slice;

pub struct DB {
    file: DBFile,
}

#[derive(Debug, Snafu)]
pub enum DBError {
    #[snafu(display("open db failed : {source}"))]
    Open { source: DBFileError },
}

impl DB {
    pub fn open(db_path: impl AsRef<Path> + std::fmt::Debug) -> Result<Self, DBError> {
        let f = DBFile::open(db_path).context(OpenSnafu)?;
        Ok(DB { file: f })
    }

    pub fn put(key: Slice, value: Slice) -> Result<(), DBError> {
        todo!()
    }

    pub fn get(key: Slice) -> Result<Slice, DBError> {
        todo!()
    }
}
