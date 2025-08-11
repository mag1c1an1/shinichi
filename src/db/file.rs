use std::{
    fmt::Debug,
    fs::{File, OpenOptions, exists},
    io::{self, Read, Seek, Write},
    path::Path,
};

use bytes::{Buf, Bytes, BytesMut};
use snafu::{ResultExt, Snafu, whatever};

use crate::db::slice::Slice;

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
/// Append only
pub struct DBFile {
    file: File,
    // KV pair numbers
    pair_nums: usize,
    // last kv pair length
    last_kv_len: Option<usize>,
}

impl DBFile {
    /// open
    pub fn open(path: impl AsRef<Path> + Debug) -> Result<Self, DBFileError> {
        if whatever!(exists(path.as_ref()), "") {
            let mut f = whatever!(
                OpenOptions::new()
                    .read(true)
                    .write(true)
                    .append(true)
                    .open(path.as_ref()),
                "can not open {:?}",
                path
            );
            // check valid
            let mut buf = [0u8; 13];
            f.seek(io::SeekFrom::Start(0)).whatever_context("")?;
            whatever!(f.read_exact(&mut buf), "");
            if &buf[..] != FILE_PREFIX {
                whatever!("{path:?} is not valid")
            } else {
                // read all pairs
                let (nums, last_kv_len) = pair_nums_of_file(&mut f)?;
                return Ok(Self {
                    file: f,
                    pair_nums: nums,
                    last_kv_len: Some(last_kv_len),
                });
            }
        } else {
            return Self::create(path.as_ref());
        }
    }

    fn create(path: &Path) -> Result<Self, DBFileError> {
        // create new file
        let mut f = OpenOptions::new()
            .create(true)
            .write(true)
            .read(true)
            .append(true)
            .open(path)
            .whatever_context("")?;
        let x = f.write(&FILE_PREFIX[..]).whatever_context("")?;
        println!(" create {x}");
        println!("{}", f.metadata().unwrap().len());
        Ok(Self {
            file: f,
            pair_nums: 0,
            last_kv_len: None,
        })
    }

    /// write kv pair
    pub fn write_pair(&mut self, kv_pair: &KVPair) -> Result<(), DBFileError> {
        // key_len
        whatever!(
            self.write_all(&(kv_pair.key.len() as u16).to_be_bytes()),
            "write key len failed"
        );
        // value_len
        whatever!(
            self.write_all(&(kv_pair.value.len() as u16).to_be_bytes()),
            "write value len failed"
        );
        // tomb
        whatever!(self.write_all(&(0u16).to_be_bytes()), "write tomb failed");
        // key
        whatever!(self.write_all(&kv_pair.key.0), "write key failed");
        // value
        whatever!(self.write_all(&kv_pair.value.0), "write value failed");
        self.pair_nums += 1;
        self.last_kv_len = Some(kv_pair.total_len());
        Ok(())
    }

    pub fn get(&mut self, key: &Slice) -> Result<Option<Slice>, DBFileError> {
        let mut ret = None;

        let mut buf = BytesMut::with_capacity(MAX_SLICE_LEN * 2);

        let mut cursor = FILE_PREFIX.len();

        self.file.seek(io::SeekFrom::Start(cursor as u64)).unwrap();
        for _ in 0..self.pair_nums {
            // read meta (5b)
            whatever!(
                self.file.read_exact(&mut buf[cursor..cursor + META_LEN]),
                ""
            );

            let k_len = buf.get_u16();
            let v_len = buf.get_u16();
            let tomb = buf.get_u8();
            cursor += 5;

            if tomb > 0 {
                continue;
            }

            if buf.capacity() - cursor < (k_len + v_len) as usize {
                buf.clear();
                cursor = 0
            }

            whatever!(
                self.file
                    .read_exact(&mut buf[cursor..cursor + (k_len + v_len) as usize]),
                ""
            );

            // need copy ?
            let key_slice = Slice(Bytes::copy_from_slice(
                &buf[cursor..cursor + k_len as usize],
            ));
            cursor += k_len as usize;

            let val_slice = Slice(Bytes::copy_from_slice(
                &buf[cursor..cursor + v_len as usize],
            ));
            cursor += v_len as usize;

            if key == &key_slice {
                ret = Some(val_slice)
            }
        }
        Ok(ret)
    }
}

fn pair_nums_of_file(f: &mut File) -> Result<(usize, usize), DBFileError> {
    let file_len = f.metadata().whatever_context("")?.len();
    if file_len == FILE_PREFIX.len() as u64 {
        // no kv pairs
        return Ok((0, 0));
    }
    f.seek(io::SeekFrom::Start(FILE_PREFIX.len() as u64))
        .whatever_context("")?;
    let mut buf = BytesMut::with_capacity(1024);
    let mut nums = 0;
    let mut cursor = 0;
    loop {
        if buf.capacity() - cursor < META_LEN {
            cursor = 0;
            buf.clear();
        }
        f.read_exact(&mut buf[cursor..META_LEN])
            .whatever_context("")?;
        let key_len = buf.get_u16();
        let value_len = buf.get_u16();
        let _tomb = buf.get_u8();
        cursor += META_LEN;
        nums += 1;
        let kv_len = (key_len + value_len) as usize;
        let now = f
            .seek(io::SeekFrom::Current(kv_len as i64))
            .whatever_context("")?;
        if now == file_len {
            // success
            return Ok((nums, META_LEN + kv_len));
        } else if now > file_len {
            whatever!("kv not match")
        } else {
            continue;
        }
    }
}

impl Write for DBFile {
    fn write(&mut self, buf: &[u8]) -> io::Result<usize> {
        self.file.write(buf)
    }

    fn flush(&mut self) -> io::Result<()> {
        self.file.flush()
    }
}

impl Read for DBFile {
    fn read(&mut self, buf: &mut [u8]) -> io::Result<usize> {
        self.file.read(buf)
    }
}

const MAX_SLICE_LEN: usize = 65536;
const META_LEN: usize = 2 + 2 + 1;

/// layout [keylen,valuelen,deleted,key,value]
/// 2b,2b,1b
pub struct KVPair<'a> {
    key: &'a Slice,
    value: &'a Slice,
}

impl<'a> KVPair<'a> {
    pub fn new(k: &'a Slice, v: &'a Slice) -> Self {
        Self { key: k, value: v }
    }
    // meta + kv_len
    pub fn total_len(&self) -> usize {
        META_LEN + self.key.len() + self.value.len()
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
            // create
            let tmp_dir = tempdir().unwrap();
            let file_path = tmp_dir.path().join("dbfile");
            let dbfile = DBFile::open(file_path);
            assert!(dbfile.is_ok())
        }
        {
            let tmp_dir = tempdir().unwrap();
            let file_path = tmp_dir.path().join("dbfile");
            {
                let mut f = File::create(file_path.as_path()).unwrap();
                let sz = f.write(FILE_PREFIX).unwrap();
                assert_eq!(sz, FILE_PREFIX.len());
                // file close here
            }

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
            f.write(s).unwrap();

            // seek is not used
            let mut dbfile = DBFile::open(file_path).unwrap();
            let mut buf = Vec::<u8>::new();
            let sz = dbfile.file.read_to_end(&mut buf).unwrap();
            assert_eq!(s.len(), sz);
            assert_eq!(&buf[..], s);
        }
    }

    #[test]
    fn test_create() {
        let f = DBFile::open("sasdfs").unwrap();
    }
}
