use std::ptr::NonNull;

use crate::alloc::Deallocation;

pub struct Bytes {
    ptr: NonNull<u8>,
    len: usize,
    deallocation: Deallocation,
}

impl Bytes {
    pub(crate) unsafe fn new(ptr: NonNull<u8>, len: usize) {
        todo!()
    }

    pub fn as_slice(&self) -> &[u8] {
        todo!()
    }
}
