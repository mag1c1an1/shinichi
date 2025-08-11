mod file;
use bytes::Bytes;
use file::DBFile;
use file::DBFileError;
use snafu::ResultExt;
use snafu::Snafu;

mod slice;
use std::path::Path;

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

    pub fn update(&mut self, key: &Slice, value: &Slice) -> Result<(), DBError> {
        self.put(key, value)
    }

    pub fn delete(&mut self, key: &Slice) -> Result<(), DBError> {
        self.put(key, &Slice(Bytes::new()))
    }

    pub fn get(&mut self, key: &Slice) -> Result<Option<Slice>, DBError> {
        self.file.get(key).context(FileSnafu)
    }
}

#[cfg(test)]
mod tests {

    use crate::{DB, db::slice::ToSlice};

    #[test]
    fn test_put() {
        let mut db = DB::open("./db.shi").unwrap();
        db.put(&"hello".to_slice(), &"world".to_slice()).unwrap();
    }
}
