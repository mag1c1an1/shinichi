use std::{fs::File, path::Path};

use bytes::Bytes;

/// for now just a wrapper on `Bytes`
pub struct Slice(Bytes);

pub trait AsSlice {
    fn as_slice(&self) -> Slice {
        todo!()
    }
}

mod db;


pub struct DB {
    file: File,
}

enum DBError {}





impl DB {
    pub fn open(db_path: impl AsRef<Path>) -> Result<Self, DBError> {
        todo!()
        // let db = DB {
        //     file: File::open(db_path)?,
        // };
    }

    pub fn put(key: Slice, value: Slice) {
        todo!()
    }

    pub fn get(key: Slice) -> Result<Slice, DBError> {
        todo!()
    }
}

pub fn add(left: u64, right: u64) -> u64 {
    left + right
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}

mod atomic {
    //! tmp in here

    use std::{
        sync::{Arc, atomic::AtomicI32},
        thread,
    };

    fn one() -> bool {
        let y = Arc::new(AtomicI32::new(0));
        let x = Arc::new(AtomicI32::new(0));

        let j1 = {
            let y = Arc::clone(&y);
            let x = Arc::clone(&x);
            thread::spawn(move || {
                let r1 = y.load(std::sync::atomic::Ordering::Relaxed);
                x.store(r1, std::sync::atomic::Ordering::Relaxed);
                r1
            })
        };
        let j2 = {
            let y = Arc::clone(&y);
            let x = Arc::clone(&x);
            thread::spawn(move || {
                let r2 = x.load(std::sync::atomic::Ordering::Relaxed);
                y.store(42, std::sync::atomic::Ordering::Relaxed);
                r2
            })
        };
        let r1 = j1.join().unwrap();
        let r2 = j2.join().unwrap();
        r1 == r2 && r1 == 42
    }
    #[test]
    fn test_one() {
        loop {
            if one() {
                print!("it is truth");
                break;
            }
        }
    }
}
